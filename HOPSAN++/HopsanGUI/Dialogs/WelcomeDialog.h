//!
//! @file   WelcomeDialog.h
//! @author Robert Braun <robert.braun@liu.se>
//! @date   2010-XX-XX
//!
//! @brief Contains a class for the Welcome dialog
//!
//$Id: WelcomeDialog.h 2427 2010-12-30 21:14:01Z petno25 $

#ifndef WELCOMEDIALOG_H
#define WELCOMEDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QCheckBox>
#include <QListWidget>

class MainWindow;

class WelcomeDialog : public QDialog
{
    Q_OBJECT

public:
    WelcomeDialog(MainWindow *parent = 0);

protected:
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual bool focusNextPrevChild(bool next);

private slots:
    void createNewModel();
    void loadExistingModel();
    void loadLastSession();
    void openRecentModel();

private:
    void updateGraphics();

    QLabel *mpHeading;
    QLabel *mpActionText;

    QPushButton *mpNew;
    QPushButton *mpOpen;
    QPushButton *mpLastSession;

    QIcon *mpNewIcon;
    QIcon *mpOpenIcon;
    QIcon *mpLastSessionIcon;

    QIcon *mpNewActiveIcon;
    QIcon *mpOpenActiveIcon;
    QIcon *mpLastSessionActiveIcon;

    QListWidget *mpRecentList;
    QStringList mModelList;

    QCheckBox *mpDontShowMe;
};

#endif // WELCOMEDIALOG_H
