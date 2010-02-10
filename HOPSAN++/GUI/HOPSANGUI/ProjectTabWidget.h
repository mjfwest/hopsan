//!
//! @file   ProjectTabWidget.h
//! @author Björn Eriksson <bjorn.eriksson@liu.se>
//! @date   2010-02-05
//!
//! @brief Contains classes for Project Tabs
//!
//$Id$

#ifndef PROJECTTABWIDGET_H
#define PROJECTTABWIDGET_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QTabWidget>


class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:
    GraphicsScene();
};


class ComponentGuiClass; //Forward declaration

class GraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    GraphicsView(QWidget *parent = 0);
    ~GraphicsView();

    //QByteArray *data;
    //QDataStream *stream;
    //QString *text;
    ComponentGuiClass *guiComponent;

protected:
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
};


//class Component : public QGraphicsWidget
//{
//    Q_OBJECT
//
//public:
//    Component(QString componentName, QGraphicsItem *parent = 0);
//
//};


class ProjectTab : public QWidget
{
    Q_OBJECT

public:
    ProjectTab(QWidget *parent = 0);

    bool isSaved;

};


class ProjectTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    ProjectTabWidget(QWidget *parent = 0);

    size_t numberOfUntitledTabs;

public slots:
    void addProjectTab();
    void saveProjectTab();
    void closeProjectTab(int index);
    void closeAllProjectTabs();

};

#endif // PROJECTTABWIDGET_H
