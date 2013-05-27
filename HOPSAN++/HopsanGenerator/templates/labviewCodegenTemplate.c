/*
 * codegen.c is a shared piece of source code that essentially represents the engine of an SIT-compatible dll
 * It should be compiled with all custom DLLs produced to work with SIT.
 * There are interdependent structures between the user code (template.c) and the engine (codegen.c).
 */

#include "SIT_API.h"
// model.h is dynamically generated by user codegen
#include "model.h"

#define EXT_IN		0
#define EXT_OUT		1

#ifdef VXWORKS
# include <private/mathP.h>
# include <private/semLibP.h>
# define HANDLE SEM_ID
# define INFINITE WAIT_FOREVER
HANDLE CreateSemaphore(long lpSemaphoreAttributes, long lInitialCount, long lMaximumCount, char* lpName)
{
	return semBCreate(SEM_Q_PRIORITY, SEM_FULL);
}
void WaitForSingleObject(HANDLE hHandle, long dwMilliseconds)
{
	semTake(hHandle, dwMilliseconds);
}
void ReleaseSemaphore(HANDLE hSemaphore, long lReleaseCount, long lpPreviousCount)
{
	semGive(hSemaphore);
}
void CloseHandle(HANDLE hObject)
{
	semFlush(hObject);
	semDelete(hObject);
}
#else
# include <windows.h>
#endif

Parameters rtParameter[2];
long READSIDE = 0;

struct { 
	long stopExecutionFlag;
	const char *errmsg;
	HANDLE flip;
	unsigned long inCriticalSection;
	long SetParamTxStatus;
	double timestamp;
} NIRT_system;

extern const double baserate;
extern const char * const ModelName;
extern const char * const build;
extern const long ParameterSize;
extern const long SignalSize;
extern const long InportSize;
extern const long OutportSize;
extern const ExtIOAttributes rtInportAttribs[];
extern const ExtIOAttributes rtOutportAttribs[];
extern const ParameterAttributes rtParamAttribs[];
extern const SignalAttributes rtSignalAttribs[];
extern const Parameters initParams;

void SetErrorMessage(char *ErrMsg, long Error)
{
	if (Error) 
		NIRT_system.stopExecutionFlag = 1;
	if (!NIRT_system.errmsg)
		NIRT_system.errmsg = ErrMsg;	
}

DLL_EXPORT long NIRT_ModelError(char* Errmsg, long* msglen)
{
	if (!NIRT_system.errmsg)
		return *msglen = 0; // No error
	if (*msglen > 0) {
		if (*msglen > (long)strlen(NIRT_system.errmsg)) 
			*msglen = strlen(NIRT_system.errmsg);
		strncpy(Errmsg, NIRT_system.errmsg, *msglen);
	}
	return NI_ERROR;	
}

DLL_EXPORT long NIRT_InitializeModel(double finaltime, double *outTimeStep, long *num_in, long *num_out, long* num_tasks) 
{
	NIRT_system.SetParamTxStatus = NI_OK;
	NIRT_system.timestamp = 0.0;
	memcpy(&rtParameter[0], &initParams, sizeof(Parameters));
	memcpy(&rtParameter[1], &initParams, sizeof(Parameters));
	if ((NIRT_system.flip = CreateSemaphore(NULL, 1, 1, NULL)) == NULL)
		SetErrorMessage("Failed to create semaphore.", 1);
	NIRT_GetModelSpec(NULL, 0, outTimeStep, num_in, num_out, num_tasks);
	return USER_Initialize();
}

long NI_ProbeOneSignal(long idx, double *value, long len, long *count)
{
	char *ptr = (char *)rtSignalAttribs[idx].addr;
  	long subindex = 0;
  	long sublength = rtSignalAttribs[idx].dimX * rtSignalAttribs[idx].dimY;
  	while ((subindex < sublength) && (*count < len))
    	value[(*count)++] = GetValueByDataType(ptr, subindex++, rtSignalAttribs[idx].datatype);
  	return *count;
}

DLL_EXPORT long NIRT_ProbeSignals(long *sigindices, long numsigs, double *value, long* len)
{
	int i, j, count = 0, idx;
	if (!NIRT_system.inCriticalSection)
    	SetErrorMessage("SignalProbe should only be called between ScheduleTasks and PostOutputs", 1);
  	if ((*len > 1) && (numsigs > 0)) {
	    value[count++] = sigindices[0];
	    value[count++] = 0;
  	}

  	for (i = 1; (i < numsigs) && (count < *len); i++) {
	    idx = sigindices[i];
	    if (idx < 0)
    	  	break;
    	if (idx < SignalSize)
      		NI_ProbeOneSignal(idx, value, *len, (long *)&count);
  	}

  	*len = count;
	return count;	
}

DLL_EXPORT long NIRT_GetSignalSpec(long* sidx, char* ID, long* ID_len, char* blkname, long* bnlen, long *portnum, char* signame, long* snlen, long *dattype, long* dims, long* numdim)
{

  long sigidx = *sidx;
  int i = 0;
  char *IDblk = 0;
  int IDport = 0;
  if (sigidx<0) {
    // check if ID has been specified.
    if (ID && *ID_len>0) {
      // parse ID into blkname and port
      i = strrchr(ID, ':') - ID;

      // malformed ID
      if (i<=0)
        return SignalSize;
      ID[i] = 0;
      IDblk = ID;
      IDport = atoi(ID+i+1);

      // lookup the table for matching ID
      for (i=0;i<SignalSize;i++) {
        if (!strcmp(IDblk,rtSignalAttribs[i].blockname) && IDport==(rtSignalAttribs[i].
             portno+1))
          break;
      }

      if (i<SignalSize)
        sigidx = *sidx = i;
      else
        return SignalSize;
    } else                             // no index or ID specified.
      return SignalSize;
  }

  if (sigidx>=0 && sigidx<SignalSize) {
    if (ID) {
      // no need for return string to be null terminated!
      // 10 to accomodate ':', port number and null character
      char *tempID = (char *)calloc(strlen(rtSignalAttribs[sigidx].blockname) + 10,
        sizeof(char));
//      sprintf(tempID,"%s:%d",rtSignalAttribs[sigidx].blockname,rtSignalAttribs[sigidx].
//              portno+1);
      if ((long)strlen(tempID)<*ID_len)
        *ID_len = strlen(tempID);
      strncpy(ID, tempID, *ID_len);
      free(tempID);
    }

    if (blkname) {
      // no need for return string to be null terminated!
      if ((long)strlen(rtSignalAttribs[sigidx].blockname)<*bnlen)
        *bnlen = strlen(rtSignalAttribs[sigidx].blockname);
      strncpy(blkname, rtSignalAttribs[sigidx].blockname, *bnlen);
    }

    if (signame) {
      // no need for return string to be null terminated!
      if ((long)strlen(rtSignalAttribs[sigidx].signalname)<*snlen)
        *snlen = strlen(rtSignalAttribs[sigidx].signalname);
      strncpy(signame, rtSignalAttribs[sigidx].signalname, *snlen);
    }

    if (portnum)
      *portnum = rtSignalAttribs[sigidx].portno;
    if (dattype)
      *dattype = rtSignalAttribs[sigidx].datatype;
    if (numdim && dims && (dims + 1)) {
      *numdim = 2;
      dims[0] = rtSignalAttribs[sigidx].dimX;
      dims[1] = rtSignalAttribs[sigidx].dimY;
    } else
      *numdim = 0;                     // set numdim to 0 to indicate that no enough memory to return the info.
    return 0;
  }

  return SignalSize;									   
									   
}

DLL_EXPORT long NIRT_GetParameterIndices(long* indices, long* len)
{
  	long i;
  	for (i = 0; i < ParameterSize && i < *len; i++)
    	indices[i] = i;
  	*len = i;
  	return NI_OK;	
}

DLL_EXPORT long NIRT_GetParameterSpec(long* pidx, char* ID, long* ID_len, char* paramname, long *pnlen, long *dattype, long* dims, long* numdim)
{

  int i= 0;
  int paramidx = *pidx;
  if (paramidx<0) {
    // check if ID has been specified.
    if (ID && *ID_len>0) {
      // lookup the table for matching ID
      for (i=0;i<ParameterSize;i++) {
        if (!strcmp(ID,rtParamAttribs[i].paramname))
          break;
      }

      if (i<ParameterSize)
        paramidx = *pidx = i;
      else
        return ParameterSize;
    } else                             // no index or ID specified.
      return ParameterSize;
  }

  if (paramidx>=0 && paramidx<ParameterSize) {
    if (ID) {
      if ((long)strlen(rtParamAttribs[paramidx].paramname)<*ID_len)
        *ID_len = strlen(rtParamAttribs[paramidx].paramname);
      strncpy(ID, rtParamAttribs[paramidx].paramname, *ID_len);
    }

    if (paramname) {
      // no need for return string to be null terminated!
      if ((long)strlen(rtParamAttribs[paramidx].paramname)<*pnlen)
        *pnlen = strlen(rtParamAttribs[paramidx].paramname);
      strncpy(paramname, rtParamAttribs[paramidx].paramname, *pnlen);
    }

    if (dattype)
      *dattype = rtParamAttribs[paramidx].datatype;
    if (!numdim)
      return 0;
    if (numdim && dims && (dims + 1)) {
      	*numdim = 2;
    	dims[0] = rtParamAttribs[paramidx].dimX;
    	dims[1] = rtParamAttribs[paramidx].dimY;
    } else
      *numdim = 0;                     
    return 0;
  }

  return ParameterSize;	
}

DLL_EXPORT long NIRT_GetParameter(long index, long subindex, double* val)
{
  	char* ptr = NULL;
  	if (index >= ParameterSize || index < 0)
	    return NI_ERROR;

  	if (subindex >= rtParamAttribs[index].dimX * rtParamAttribs[index].dimY)
	    return NI_ERROR;
  	ptr = (char *)(&rtParameter[READSIDE]);
  	ptr += (int)rtParamAttribs[index].addr;
  	*val = GetValueByDataType(ptr, subindex, rtParamAttribs[index].datatype);
  	return NI_OK;	
}

DLL_EXPORT long NIRT_SetParameter(long index, long subindex, double val)
{
  	char* ptr = NULL;
  	if (index >= ParameterSize) {
	  	NIRT_system.SetParamTxStatus = NI_ERROR;
	    return NIRT_system.SetParamTxStatus;
    }
  	if (index < 0) {
	    if (NIRT_system.SetParamTxStatus == NI_ERROR) {
	 		memcpy(&rtParameter[READSIDE], &rtParameter[1-READSIDE], sizeof(Parameters));

      		// reset the status.
      		NIRT_system.SetParamTxStatus = NI_OK;
      		return NI_ERROR;
	    }

	    // commit changes
	    WaitForSingleObject(NIRT_system.flip, INFINITE);
	    READSIDE = 1 - READSIDE;
	    ReleaseSemaphore(NIRT_system.flip, 1, NULL);

	    // Copy back the newly set parameters to the writeside.
	    memcpy(&rtParameter[1-READSIDE], &rtParameter[READSIDE], sizeof(Parameters));
    	return NI_OK;
  	}

  	// verify that subindex is within bounds.
  	if (subindex >= rtParamAttribs[index].dimX * rtParamAttribs[index].dimY) {
	  	NIRT_system.SetParamTxStatus = NI_ERROR;
	    return NIRT_system.SetParamTxStatus;
    }
  	ptr = (char *)(&rtParameter[1-READSIDE]);
  	ptr += (int)rtParamAttribs[index].addr;
  	return SetValueByDataType(ptr, subindex, val, rtParamAttribs[index].datatype);
}

DLL_EXPORT long NIRT_Schedule(double *inData, double *outData, double *outTime, long *dispatchtasks)
{
	if (outTime)
		*outTime = NIRT_system.timestamp;
	WaitForSingleObject(NIRT_system.flip, INFINITE);
	if (NIRT_system.inCriticalSection > 0) {
		SetErrorMessage("Each call to Schedule() MUST be followed by a call to ModelUpdate() before Schedule() is called again.", 1);
		ReleaseSemaphore(NIRT_system.flip, 1, NULL);
		return NI_ERROR;
	}
	USER_TakeOneStep(inData, outData, NIRT_system.timestamp);
	NIRT_system.inCriticalSection++;
	return NI_OK;
}

DLL_EXPORT long NIRT_ModelUpdate()
{
	if (NIRT_system.inCriticalSection) {
		NIRT_system.inCriticalSection--;
		NIRT_system.timestamp += baserate;
		ReleaseSemaphore(NIRT_system.flip, 1, NULL);
	} else 
		SetErrorMessage("Model Update Failed", 1);
	return NIRT_system.inCriticalSection;
}


DLL_EXPORT long NIRT_GetExtIOSpec(long index, long *idx, char* name, long* tid, long *type, long *dims, long* numdims)
{
	const ExtIOAttributes* base;
	if (index==-1)
    	return InportSize + OutportSize;
  	
    if (index >= InportSize) {
	    index -= InportSize;
	    base = rtOutportAttribs;
	    if (type)
	    	*type = EXT_OUT;
    } else {
	    base = rtInportAttribs;
	    if (type)
	    	*type = EXT_IN;
    }
    	
    if (idx)
    	*idx = index;
  	
    if (name) {
    	int sz = strlen(name);
    	strncpy(name, base[index].name, sz-1);
    	name[sz-1]= 0;
  	}

  	if (tid)
	    *tid = 0;
  	
	if (numdims && dims && (dims + 1)) {
		*numdims = 2;
      	dims[0] = base[index].dimX;
      	dims[1] = base[index].dimY;     
  	} else {
		*numdims = 0; 	
  	}
  	return 0;	
}

DLL_EXPORT long NIRT_FinalizeModel(void) {
	CloseHandle(NIRT_system.flip);
	return USER_Finalize();
}

DLL_EXPORT long NIRT_GetModelSpec(char* name, long *namelen, double *baseTimeStep, long *outNumInports, long *outNumOutports, long *numtasks)
{
	if (namelen) {
		long i, length;
		length = (long)strlen(ModelName);
		if (*namelen > length)
			*namelen = length;

		strncpy(name, ModelName, *namelen);

		for(i=0; i<*namelen; i++)
			name[i] = (char) tolower(name[i]);
	}
	if (baseTimeStep)
		*baseTimeStep = baserate;
	
	if (outNumInports)
		*outNumInports = InportSize;
	
	if (outNumOutports)
		*outNumOutports = OutportSize;
	
	if (numtasks) 
		*numtasks = 1; 
	return NI_OK;	
}

DLL_EXPORT long NIRT_GetBuildInfo(char* detail, long* len)
{
  	if (*len>=(long)strlen(build))
    	*len = strlen(build)+1;
  	strncpy(detail,build,*len);
  	return 1;	
}