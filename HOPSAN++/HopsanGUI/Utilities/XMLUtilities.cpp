#include "XMLUtilities.h"
#include <QMessageBox>


QDomElement loadXMLDomDocument(QFile &rFile, QDomDocument &rDomDocument, QString rootTagName)
{
    QString errorStr;
    int errorLine, errorColumn;
    if (!rDomDocument.setContent(&rFile, false, &errorStr, &errorLine, &errorColumn))
    {
        QMessageBox::information(0, "Hopsan GUI",
                                 QString("Parse error at line %1, column %2:\n%3")
                                 .arg(errorLine)
                                 .arg(errorColumn)
                                 .arg(errorStr));
    }
    else
    {
        QDomElement xmlRoot = rDomDocument.documentElement();
        if (xmlRoot.tagName() != rootTagName)
        {
            QMessageBox::information(0, "Hopsan GUI",
                                     QString("The file is has the wrong Root Tag Name: ")
                                     + xmlRoot.tagName() + "!=" + rootTagName);
        }
        else
        {
            return xmlRoot;
        }
    }
    return QDomElement(); //NULL
}


void appendRootXMLProcessingInstruction(QDomDocument &rDomDocument)
{
    QDomNode xmlProcessingInstruction = rDomDocument.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    rDomDocument.insertBefore(xmlProcessingInstruction, rDomDocument.firstChild());
}

//! @brief Helper function for adding one initially empty Dom node
//! @todo maybe retunr reference (is there a difference)
//! @returns The new sub element dom node
QDomElement appendDomElement(QDomElement &rDomElement, const QString element_name)
{
    QDomElement subDomElement = rDomElement.ownerDocument().createElement(element_name);
    rDomElement.appendChild(subDomElement);
    return subDomElement;
}

//! @brief Helper function for adding Dom elements containing one text node
void appendDomTextNode(QDomElement &rDomElement, const QString element_name, const QString text)
{
    //Only write tag if both name and value is non empty
    if (!element_name.isEmpty() && !text.isEmpty())
    {
        QDomDocument ownerDomDocument = rDomElement.ownerDocument();
        QDomElement subDomElement = ownerDomDocument.createElement(element_name);
        subDomElement.appendChild(ownerDomDocument.createTextNode(text));
        rDomElement.appendChild(subDomElement);
    }
}

void appendDomBooleanNode(QDomElement &rDomElement, const QString element_name, const bool value)
{
    if(value)
    {
        appendDomTextNode(rDomElement, element_name, HMF_TRUETAG);
    }
    else
    {
        appendDomTextNode(rDomElement, element_name, HMF_FALSETAG);
    }
}

//! @brief Helper function for adding Dom elements containing one text node (based on a double value)
void appendDomValueNode(QDomElement &rDomElement, const QString element_name, const double val)
{
    QString tmp_string;
    tmp_string.setNum(val);
    appendDomTextNode(rDomElement, element_name, tmp_string);
}


void appendDomValueNode2(QDomElement &rDomElement, const QString element_name, const double a, const double b)
{
    QString num,str;
    num.setNum(a);
    str.append(num);
    str.append(" ");
    num.setNum(b);
    str.append(num);
    appendDomTextNode(rDomElement, element_name, str);
}


void appendDomValueNode3(QDomElement &rDomElement, const QString element_name, const double a, const double b, const double c)
{
    QString num,str;
    num.setNum(a);
    str.append(num);
    str.append(" ");
    num.setNum(b);
    str.append(num);
    str.append(" ");
    num.setNum(c);
    str.append(num);
    appendDomTextNode(rDomElement, element_name, str);
}


void appendDomValueNodeN(QDomElement &rDomElement, const QString element_name, const QVector<qreal> &rValues)
{
    QString num,str;
    for (int i=0; i<rValues.size(); ++i)
    {
        num.setNum(rValues[i]);
        str.append(num);
        str.append(" ");
    }
    str.chop(1); //Remove last space
    appendDomTextNode(rDomElement, element_name, str);
}



void parseDomValueNode3(QDomElement domElement, double &rA, double &rB, double &rC)
{
    QStringList poseList = domElement.text().split(" ");
    rA = poseList[0].toDouble();
    rB = poseList[1].toDouble();
    rC = poseList[2].toDouble();
}

void parseDomValueNode2(QDomElement domElement, double &rA, double &rB)
{
    QStringList poseList = domElement.text().split(" ");
    rA = poseList[0].toDouble();
    rB = poseList[1].toDouble();
}

qreal parseDomValueNode(QDomElement domElement)
{
    return domElement.text().toDouble();
}

bool parseDomBooleanNode(QDomElement domElement)
{
    return (domElement.text() == "true");
}

//void setAttribute(QDomElement &rDomElement, QString attrname, QString text)
//{
//    rDomElement.setAttribute(attrname, text);
//}

//void setAttribute(QDomElement &rDomElement, QString attrname, int text)
//{
//    rDomElement.setAttribute(attrname, text);
//}

void appendPoseTag(QDomElement &rDomElement, qreal x, qreal y, qreal th, bool flipped)
{
    QDomElement pose = appendDomElement(rDomElement, HMF_POSETAG);
    pose.setAttribute("x",x);
    pose.setAttribute("y",y);
    pose.setAttribute("a",th);
    pose.setAttribute("flipped", flipped);
}

void appendPortPoseTag(QDomElement &rDomElement, QString name, qreal x, qreal y, qreal th)
{
    QDomElement pose = appendDomElement(rDomElement, HMF_PORTPOSETAG);
    pose.setAttribute("name",name);
    pose.setAttribute("x",x);
    pose.setAttribute("y",y);
    pose.setAttribute("a",th);
}

void appendCoordinateTag(QDomElement &rDomElement, qreal x, qreal y)
{
    QDomElement pose = appendDomElement(rDomElement, HMF_COORDINATETAG);
    pose.setAttribute("x",x);
    pose.setAttribute("y",y);
}

void appendViewPortTag(QDomElement &rDomElement, qreal x, qreal y, qreal zoom)
{
    QDomElement pose = appendDomElement(rDomElement, HMF_VIEWPORTTAG);
    pose.setAttribute("x",x);
    pose.setAttribute("y",y);
    pose.setAttribute("zoom",zoom);
}

void appendSimulationTimeTag(QDomElement &rDomElement, qreal start, qreal step, qreal stop)
{
    QDomElement simu = appendDomElement(rDomElement, HMF_SIMULATIONTIMETAG);
    simu.setAttribute("start", start);
    simu.setAttribute("timestep", step);
    simu.setAttribute("stop", stop);
}

void parsePoseTag(QDomElement domElement, qreal &rX, qreal &rY, qreal &rTheta, bool &rFlipped)
{
    rX = domElement.attribute("x").toDouble();
    rY = domElement.attribute("y").toDouble();
    rTheta = domElement.attribute("a").toDouble();
    rFlipped = (domElement.attribute("flipped") == "1");
}

void parsePortPoseTag(QDomElement domElement, QString &rName, qreal &rX, qreal &rY, qreal &rTheta)
{
    rName = domElement.attribute("name");
    bool dummy;
    parsePoseTag(domElement, rX, rY, rTheta, dummy);
}

void parseCoordinateTag(QDomElement domElement, qreal &rX, qreal &rY)
{
    rX = domElement.attribute("x").toDouble();
    rY = domElement.attribute("y").toDouble();
}

void parseViewPortTag(QDomElement domElement, qreal &rX, qreal &rY, qreal &rZoom)
{
    rX = domElement.attribute("x").toDouble();
    rY = domElement.attribute("y").toDouble();
    rZoom = domElement.attribute("zoom").toDouble();
}

void parseSimulationTimeTag(QDomElement domElement, qreal &rStart, qreal &rStep, qreal &rStop)
{
    rStart = domElement.attribute("start").toDouble();
    rStep = domElement.attribute("timestep").toDouble();
    rStop = domElement.attribute("stop").toDouble();
}

