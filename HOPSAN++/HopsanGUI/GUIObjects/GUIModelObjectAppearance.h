/*-----------------------------------------------------------------------------
 This source file is part of Hopsan NG

 Copyright (c) 2011 
    Mikael Axin, Robert Braun, Alessandro Dell'Amico, Björn Eriksson,
    Peter Nordin, Karl Pettersson, Petter Krus, Ingo Staack

 This file is provided "as is", with no guarantee or warranty for the
 functionality or reliability of the contents. All contents in this file is
 the original work of the copyright holders at the Division of Fluid and
 Mechatronic Systems (Flumes) at Linköping University. Modifying, using or
 redistributing any part of this file is prohibited without explicit
 permission from the copyright holders.
-----------------------------------------------------------------------------*/

//!
//! @file   ModelObjectAppearance.h
//! @author Peter Nordin <peter.nordin@liu.se>
//! @date   2010-04-22
//!
//! @brief Contains appearance data to be used by GuiModelObjects and library widget
//!
//$Id$

#ifndef ModelObjectAppearance_H
#define ModelObjectAppearance_H

#include <QString>
#include <QPointF>
#include "common.h"

#include "Utilities/XMLUtilities.h"
#include "GUIPortAppearance.h"

//Define for the root xml element name, and the element name for each component (modelobject)
#define CAF_VERSION "version"
#define CAF_ROOT "hopsanobjectappearance"
#define CAF_MODELOBJECT "modelobject"

enum AbsoluteRelativeEnumT {Absolute, Relative};

QDomElement appendOrGetCAFRootTag(QDomElement parentElement);

class ModelObjectIconAppearance
{
public:
    ModelObjectIconAppearance();
    QString mRelativePath;
    QString mAbsolutePath;
    QString mRotationBehaviour;
    qreal mScale;
    bool mIsValid;
};

class ModelObjectAnimationData
{
public:
    void readFromDomElement(QDomElement &rDomElement, QString basePath);
    void saveToDomElement(QDomElement &rDomElement);

    QString baseIconPath;
    QList<int> movableIdx;
    QStringList movableIconPaths;
    QVector<QStringList> dataPorts;
    QVector<QStringList> dataNames;
    QStringList multipliers;
    QStringList divisors;
    QVector<double> speedX;
    QVector<double> speedY;
    QVector<double> speedTheta;
    QVector<double> resizeX;
    QVector<double> resizeY;
    QVector<double> initScaleX;
    QVector<double> initScaleY;
    QVector<int> scaleDataIdx1;
    QVector<int> scaleDataIdx2;
    QVector<double> startX;
    QVector<double> startY;
    QVector<double> startTheta;
    QVector<double> initColorR;
    QVector<double> initColorG;
    QVector<double> initColorB;
    QVector<double> initColorA;
    QVector<double> colorR;
    QVector<double> colorG;
    QVector<double> colorB;
    QVector<double> colorA;
    QVector<int> colorDataIdx;
    QVector<double> transformOriginX;
    QVector<double> transformOriginY;
    QVector<bool> isAdjustable;
    QVector<double> adjustableMinX;
    QVector<double> adjustableMaxX;
    QVector<double> adjustableMinY;
    QVector<double> adjustableMaxY;
    QStringList adjustablePort;
    QStringList adjustableDataName;
    QVector<double> adjustableGainX;
    QVector<double> adjustableGainY;
    QVector<QStringList> movablePortNames;
    QVector<QList<double> > movablePortStartX;
    QVector<QList<double> > movablePortStartY;
    QList<int> movableRelatives;

    //Container-specific data
    double flowSpeed;
};


class ModelObjectAppearance
{
public:
    ModelObjectAppearance();
    void setTypeName(const QString type);
    void setSubTypeName(const QString subtype);
    void setDisplayName(const QString name);
    void setHelpText(const QString text);
    void setBasePath(const QString path);
    void setIconPath(const QString path, const GraphicsTypeEnumT gfxType, const AbsoluteRelativeEnumT absrel);
    void setIconScale(const qreal scale, const GraphicsTypeEnumT gfxType);

    QString getTypeName() const;
    QString getSubTypeName() const;
    QString getDisplayName() const;
    QString getNonEmptyName() const;
    QString getHelpPicture() const;
    QString getHelpText() const;
    QString getBasePath() const;
    QString getFullAvailableIconPath(GraphicsTypeEnumT gfxType=UserGraphics);
    QString getIconPath(const GraphicsTypeEnumT gfxType, const AbsoluteRelativeEnumT absrel);
    qreal   getIconScale(const GraphicsTypeEnumT gfxType=UserGraphics);
    QString getIconRotationBehaviour(const GraphicsTypeEnumT gfxType=UserGraphics);
    QPointF getNameTextPos() const;
    QString getSourceCodeFile() const;
    QString getLibPath() const;
    bool isRecompilable() const;

    ModelObjectAnimationData *getAnimationDataPtr();

    
    PortAppearanceMapT &getPortAppearanceMap();
    void erasePortAppearance(const QString portName);
    void addPortAppearance(const QString portName, PortAppearance *pPortAppearance=0);

    bool hasIcon(const GraphicsTypeEnumT gfxType);

    void readFromDomElement(QDomElement domElement);
    void saveToDomElement(QDomElement &rDomElement);
    void saveSpecificPortsToDomElement(QDomElement &rDomElement, const QStringList &rParametNames);
    void saveToXMLFile(QString filename);

private:
    QString mTypeName;
    QString mSubTypeName;
    QString mDisplayName;
    QString mSourceCode;
    QString mLibPath;
    bool mIsRecompilable;
    QString mHelpPicture;
    QString mHelpText;
    ModelObjectIconAppearance mIsoIconAppearance;
    ModelObjectIconAppearance mUserIconAppearance;
    QString mDefaultMissingIconPath;
    QPointF mNameTextPos;
    QStringList mReplacementObjects;

    PortAppearanceMapT mPortAppearanceMap;

    //BaseDir for relative paths
    QString mBasePath;

    //Private help functions
    QDomElement addModelObjectRootElement(QDomElement parentDomElement);
    void setRelativePathFromAbsolute();
    void setAbsoultePathFromRelative();
    void refreshIconValid();
    GraphicsTypeEnumT selectAvailableGraphicsType(const GraphicsTypeEnumT type);

    ModelObjectAnimationData mAnimationData;
};


#endif // APPEARANCEDATA_H
