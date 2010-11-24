#ifndef GUICOMPONENT_H
#define GUICOMPONENT_H

#include "GUIModelObject.h"
#include "common.h"
#include "../Utilities/XMLUtilities.h"
#include <assert.h>

//Forward declarations
class ProjectTabWidget;
class GUIConnector;
class GUIPort;
class GUIContainerObject;

class GUIComponent : public GUIModelObject
{
    Q_OBJECT
public:
    GUIComponent(GUIModelObjectAppearance* pAppearanceData, QPoint position, qreal rotation, GUIContainerObject *system, selectionStatus startSelected = DESELECTED, graphicsType gfxType = USERGRAPHICS, QGraphicsItem *parent = 0);
    ~GUIComponent();

    QVector<QString> getParameterNames();
    QString getParameterUnit(QString name);
    QString getParameterDescription(QString name);
    double getParameterValue(QString name);
    void setParameterValue(QString name, double value);
    void setGlobalParameter(QString name, QString gPar);
    void rememberGlobalParameter(QString name, QString parameterName);
    void forgetGlobalParameter(QString name);
    bool hasGlobalParameter(QString name);
    QString getGlobalParameterKey(QString parameterName);

    //void saveToTextStream(QTextStream &rStream, QString prepend=QString());

    void setName(QString name, renameRestrictions renameSettings=UNRESTRICTED);
    QString getTypeName();
    QString getTypeCQS();

    enum { Type = GUICOMPONENT };
    int type() const;

protected:
    void saveCoreDataToDomElement(QDomElement &rDomElement);

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
//    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    void openPropertiesDialog();

    void createPorts();

private:
    QMap<QString, QString> mGlobalParameters;
};

#endif // GUICOMPONENT_H
