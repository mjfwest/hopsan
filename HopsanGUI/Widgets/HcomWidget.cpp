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
//! @file   HcomWidget.cpp
//! @author Robert Braun <robert.braun@liu.se>
//! @date   2012-09-19
//! @version $Id$
//!
//! @brief Contains the HCOM terminal widget
//!


#include <QDateTime>
#include <QMessageBox>
#include <QWidget>
#include <QToolButton>
#include <QGridLayout>
#include <QTextBlock>
#include <QScrollBar>


#include <cmath>

#include "global.h"
#include "Configuration.h"
#include "CoreAccess.h"
#include "DesktopHandler.h"
#include "HcomHandler.h"
#include "Utilities/GUIUtilities.h"
#include "Widgets/HcomWidget.h"
#include "GUIObjects/GUIModelObject.h"
#include "GUIPort.h"


TerminalWidget::TerminalWidget(QWidget *pParent)
    : QWidget(pParent)
{
    this->setMouseTracking(true);

    QPushButton *pClearMessageWidgetButton = new QPushButton("Clear Messages");
    mpAbortHCOMWidgetButton = new QPushButton("Abort Script");
    mpAbortHCOMWidgetButton->setDisabled(true);
    QFont tempFont = pClearMessageWidgetButton->font();
    tempFont.setBold(true);
    pClearMessageWidgetButton->setFont(tempFont);
    pClearMessageWidgetButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    mpAbortHCOMWidgetButton->setFont(tempFont);
    mpAbortHCOMWidgetButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    QToolButton *pShowErrorMessagesButton = new QToolButton();
    pShowErrorMessagesButton->setIcon(QIcon(QString(ICONPATH) + "Hopsan-ShowErrorMessages.png"));
    pShowErrorMessagesButton->setCheckable(true);
    pShowErrorMessagesButton->setChecked(true);
    pShowErrorMessagesButton->setToolTip("Show Error Messages");

    QToolButton *pShowWarningMessagesButton = new QToolButton();
    pShowWarningMessagesButton->setIcon(QIcon(QString(ICONPATH) + "Hopsan-ShowWarningMessages.png"));
    pShowWarningMessagesButton->setCheckable(true);
    pShowWarningMessagesButton->setChecked(true);
    pShowWarningMessagesButton->setToolTip("Show Warning Messages");

    QToolButton *pShowInfoMessagesButton = new QToolButton();
    pShowInfoMessagesButton->setIcon(QIcon(QString(ICONPATH) + "Hopsan-ShowInfoMessages.png"));
    pShowInfoMessagesButton->setCheckable(true);
    pShowInfoMessagesButton->setChecked(true);
    pShowInfoMessagesButton->setToolTip("Show Info Messages");

    QToolButton *pShowDebugMessagesButton = new QToolButton();
    pShowDebugMessagesButton->setIcon(QIcon(QString(ICONPATH) + "Hopsan-ShowDebugMessages.png"));
    pShowDebugMessagesButton->setCheckable(true);
    pShowDebugMessagesButton->setChecked(false);
    pShowDebugMessagesButton->setToolTip("Show Debug Messages");

    mpGroupByTagCheckBox = new QCheckBox("Group Similar Messages");
    mpGroupByTagCheckBox->setChecked(gpConfig->getGroupMessagesByTag());

    mpConsole = new TerminalConsole(this);
    mpHandler = new HcomHandler(mpConsole);

    QGridLayout *pLayout = new QGridLayout(this);
    pLayout->addWidget(mpConsole,0,0,1,1);
    pLayout->addWidget(mpConsole,0,0,1,7);
    size_t x = 0;
    pLayout->addWidget(pClearMessageWidgetButton,1,x++,1,1);
    pLayout->addWidget(mpAbortHCOMWidgetButton,1,x++,1,1);
    pLayout->addWidget(pShowErrorMessagesButton,1,x++,1,1);
    pLayout->addWidget(pShowWarningMessagesButton,1,x++,1,1);
    pLayout->addWidget(pShowInfoMessagesButton,1,x++,1,1);
    pLayout->addWidget(pShowDebugMessagesButton,1,x++,1,1);
    pLayout->addWidget(mpGroupByTagCheckBox, 1,x++,1,1);
    pLayout->setContentsMargins(4,4,4,4);
    this->setLayout(pLayout);

    this->installEventFilter(this);
    this->setMouseTracking(true);
    this->setMinimumHeight(0);

    connect(pClearMessageWidgetButton, SIGNAL(clicked()),mpConsole, SLOT(clear()));
    connect(mpAbortHCOMWidgetButton, SIGNAL(clicked()),mpConsole, SLOT(abortHCOM()));
    connect(pShowErrorMessagesButton, SIGNAL(toggled(bool)), mpConsole, SLOT(showErrorMessages(bool)));
    connect(pShowWarningMessagesButton, SIGNAL(toggled(bool)), mpConsole, SLOT(showWarningMessages(bool)));
    connect(pShowInfoMessagesButton, SIGNAL(toggled(bool)), mpConsole, SLOT(showInfoMessages(bool)));
    connect(pShowDebugMessagesButton, SIGNAL(toggled(bool)), mpConsole, SLOT(showDebugMessages(bool)));
    connect(mpGroupByTagCheckBox, SIGNAL(toggled(bool)), mpConsole, SLOT(setGroupByTag(bool)));
}


//! @brief Reimplementation of QWidget::sizeHint(), used to reduce the size of the message widget when docked
QSize TerminalWidget::sizeHint() const
{
    QSize size = QWidget::sizeHint();
    //Set very small height. A minimum apperantly stops at resonable size.
    size.rheight() = 1; //pixels
    return size;
}


//! @brief Loads terminal widget settings from configuration object
//! This is needed because terminal widget must be created before configuration
//! (so that config can print messages), which means that message widgets cannot
//! load the config directly in the constructor.
void TerminalWidget::loadConfig()
{
    mpGroupByTagCheckBox->setChecked(gpConfig->getGroupMessagesByTag());
    mpConsole->mHistory = gpConfig->getTerminalHistory();

    if(!gpConfig->getHcomWorkingDirectory().isEmpty())
        mpHandler->setWorkingDirectory(gpConfig->getHcomWorkingDirectory());
}



void TerminalWidget::saveConfig()
{
    if(mpConsole->mHistory.size() > gpConfig->getTerminalHistory().size())
    {
        mpConsole->mHistory.prepend("--- "+QDateTime::currentDateTime().toString()+" ---");
        gpConfig->storeTerminalHistory(mpConsole->mHistory);
    }
    gpConfig->setHcomWorkingDirectory(mpConsole->getHandler()->getWorkingDirectory());
}


void TerminalWidget::mouseMoveEvent(QMouseEvent *event)
{
    mpConsole->setFrameShape(QFrame::NoFrame);

    //qDebug() << "Mouse move event!";

    QWidget::mouseMoveEvent(event);
}


bool TerminalWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseMove)
    {
        //qDebug() << "Ate an event!";

        mpConsole->setFrameShape(QFrame::NoFrame);

        return true;
    }
    else
    {
        return QObject::eventFilter(obj, event);
    }
}


void TerminalWidget::setAbortButtonEnabled(bool enable)
{
    mpAbortHCOMWidgetButton->setEnabled(enable);
}

void TerminalWidget::printMessage(const GUIMessage &rMessage)
{
    mpConsole->printMessage(rMessage);
}


//! @brief Constructor for HCOM console widget
TerminalConsole::TerminalConsole(TerminalWidget *pParent)
    : QTextEdit(pParent)
{
    mpTerminal = pParent;
    mpCoreAccess = new CoreMessagesAccess;

    this->setReadOnly(false);
    this->setMouseTracking(true);

    //Setup font
    QFont monoFont = QFont("Monospace", 9, 50);
    monoFont.setStyleHint(QFont::TypeWriter);
    this->setFont(monoFont);

    //Setup colors
    //this->setTextColor(QColor("Black"));
    //this->setStyleSheet(QString::fromUtf8("QTextEdit {background-color: white; border: 1px solid gray;}"));

    mGroupByTag = gpConfig->getGroupMessagesByTag();

    mDontPrint = false;
    mShowErrorMessages = true;
    mShowWarningMessages = true;
    mShowInfoMessages = true;
    mShowDebugMessages = false;

    mCurrentHistoryItem=-1;
    this->append(">> ");

    //mpErrorSound = new QSound(QString(SOUNDSPATH)+"error.wav");
}


void TerminalConsole::printFirstInfo()
{
    this->print("--------------------------------------");
    this->print(" Hopsan HCOM Terminal                   ");
    this->print(" Write \"help\" for a list of commands. ");
    this->print("--------------------------------------");
}


HcomHandler *TerminalConsole::getHandler()
{
    return mpTerminal->mpHandler;
}

////! @brief Obtains messages from core and prints them in the message widget
//void TerminalConsole::printCoreMessages()
//{
//    int nmsg = mpCoreAccess->getNumberOfMessages();
//    //nmsg = 0; //!< @warning Fix for Petter should not be checked into the repository

//    //bool playErrorSound = false;
//    for (int idx=0; idx < nmsg; ++idx)
//    {
//        QString message, type, tag;
//        mpCoreAccess->getMessage(message, type, tag);
//        //if(type == "error")
//            //playErrorSound = true;
//        if(type == "fatal")
//        {
//            QMessageBox::critical(this, "Fatal Error", message+"\n\nProgram is unstable and MUST BE RESTARTED!", "Ok");

//        }
//        mNewMessageList.append(GUIMessage(message, type, tag));
//        updateNewMessages();
//    }
////    if(playErrorSound)
////    {
////        mpErrorSound->play();
////    }
//}


void TerminalConsole::printFatalMessage(QString message)
{
    printMessage(GUIMessage(message.prepend("Error: "), "", Fatal), true);
    QMessageBox::critical(this, "Fatal Error", message+"\n\nProgram is unstable and MUST BE RESTARTED!", "Ok");
}


void TerminalConsole::printErrorMessage(QString message, QString tag, bool timeStamp)
{
    printMessage(GUIMessage(message.prepend("Error: "), tag, Error), timeStamp);
}


void TerminalConsole::printWarningMessage(QString message, QString tag, bool timeStamp)
{
    printMessage(GUIMessage(message.prepend("Warning: "), tag, Warning), timeStamp);
}


void TerminalConsole::printInfoMessage(QString message, QString tag, bool timeStamp)
{
    printMessage(GUIMessage(message.prepend("Info: "), tag, Info), timeStamp);
}


void TerminalConsole::print(QString message)
{
    printMessage(GUIMessage(message, "", UndefinedMessageType), false);
}


void TerminalConsole::printDebugMessage(QString message, QString tag, bool timeStamp)
{
    printMessage(GUIMessage(message.prepend("Debug: "), tag, Debug), timeStamp);
}


void TerminalConsole::printMessage(const GUIMessage &rMessage, bool timeStamp)
{
    if(mDontPrint) return;

    // Only show message if its type shall be shown
    if( (rMessage.mType == Info    && mShowInfoMessages)     ||
        (rMessage.mType == Warning && mShowWarningMessages)  ||
        (rMessage.mType == Error   && mShowErrorMessages)    ||
        (rMessage.mType == Debug   && mShowDebugMessages)    ||
        (rMessage.mType == UndefinedMessageType)             ||
        (rMessage.mType == Fatal) )
    {
        QString output = rMessage.mMessage;
        if(timeStamp && !rMessage.mTimestamp.isEmpty())
        {
            output.prepend("["+rMessage.mTimestamp+"] ");
        }

        // Message is tagged, and group by tag setting is active
        if(mGroupByTag && !rMessage.mTag.isEmpty() && (rMessage.mTag == mLastTag) )
        {
            ++mSubsequentTags;
            this->undo();
            output.append(QString("    (%1 similar)").arg(mSubsequentTags));
        }
        // Message is not tagged, or group by tag setting is not active
        else
        {
            mSubsequentTags = 1;
            mLastTag =rMessage.mTag;
        }


        this->moveCursor( QTextCursor::End, QTextCursor::MoveAnchor );
        this->moveCursor( QTextCursor::StartOfLine, QTextCursor::MoveAnchor );
        setOutputColor(rMessage.mType);
        this->insertPlainText(output+"\n");
        this->moveCursor( QTextCursor::End, QTextCursor::MoveAnchor );

        // Reset default color
        setOutputColor(UndefinedMessageType);
    }
}



//! @brief Clear function for message widget, this will empty the message widget and also remove all messages from the list
void TerminalConsole::clear()
{
    QTextEdit::clear();
    append(">> ");
}


//! @brief Aborts CHOM operation
void TerminalConsole::abortHCOM()
{
    getHandler()->abortHCOM();
}


//! @brief Tells the message widget wether or not messages shall be grouped by tags
//! @param value True means that messages shall be grouped
void TerminalConsole::setGroupByTag(bool value)
{
    mGroupByTag = value;
    gpConfig->setGroupMessagesByTag(value);
}


//! @brief Tells the message widget wether or not it shall show error messages
//! @param value True means show messages
void TerminalConsole::showErrorMessages(bool value)
{
    mShowErrorMessages = value;
}


//! @brief Tells the message widget wether or not it shall show warning messages
//! @param value True means show messages
void TerminalConsole::showWarningMessages(bool value)
{
    mShowWarningMessages = value;
}


//! @brief Tells the message widget wether or not it shall show info messages
//! @param value True means show messages
void TerminalConsole::showInfoMessages(bool value)
{
    mShowInfoMessages = value;
}


//! @brief Tells the message widget wether or not it shall show debug messages
//! @param value True means show messages
void TerminalConsole::showDebugMessages(bool value)
{
    mShowDebugMessages = value;
}

bool TerminalConsole::getDontPrint() const
{
    return mDontPrint;
}

void TerminalConsole::setDontPrint(const bool value)
{
    mDontPrint = value;
}



void TerminalConsole::setOutputColor(MessageTypeEnumT type)
{
    if (type == Error || type == Fatal)
    {
        this->setTextColor(QColor("Red"));
    }
    else if (type == Warning)
    {
        this->setTextColor(QColor(216, 115, 0));
    }
    else if (type == Info)
    {
        this->setTextColor("BLACK");
    }
    else if (type == Debug)
    {
        this->setTextColor("BLUE");
    }
    else
    {
        this->setTextColor("BLACK");
    }
}


bool TerminalConsole::isOnLastLine()
{
    int row = this->textCursor().blockNumber();
    int col = this->textCursor().columnNumber();
    int rows = this->document()->blockCount();

    return (row == rows-1 && col > 2);
}


void TerminalConsole::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Home)
    {
        handleHomeKeyPress();
        return;
    }
    else if(event->key() == Qt::Key_Escape)
    {
        handleEscapeKeyPress();
        return;
    }

    if(isOnLastLine())
    {
        int col = this->textCursor().columnNumber();
        if((event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Left) && col == 3)
        {
            return;     //Don't allow backspace or left key if on first allowed character
        }
        else if(event->key() == Qt::Key_Up)
        {
            cancelAutoComplete();
            handleUpKeyPress();
            return;
        }
        else if(event->key() == Qt::Key_Down)
        {
            cancelAutoComplete();
            handleDownKeyPress();
            return;
        }
        else if(event->key() == Qt::Key_Tab)
        {
            handleTabKeyPress();
            return;
        }
        else if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        {
            cancelRecentHistory();
            cancelAutoComplete();
            handleEnterKeyPress();
            return;
        }
        cancelRecentHistory();
        cancelAutoComplete();
        QTextEdit::keyPressEvent(event);
    }
    else
    {
        if(event->key() == Qt::Key_Up || event->key() == Qt::Key_Down || event->key() == Qt::Key_Left || event->key() == Qt::Key_Right)
        {
            //Move cursor back to last line
            this->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
        }
        else if(event->key() == Qt::Key_C)
        {
            int len1 = this->toPlainText().size();
            QTextEdit::keyPressEvent(event);
            int len2 = this->toPlainText().size();
            if(len1!=len2)
            {
                undo();
            }
        }
        else
        {
            QList<int> otherAllowedKeys = QList<int>() << Qt::Key_Control << Qt::Key_PageUp << Qt::Key_PageDown;
            if(otherAllowedKeys.contains(event->key()))
            {
                QTextEdit::keyPressEvent(event);
            }
        }
    }
}


//! @brief Handles enter key press (i.e. execute command)
void TerminalConsole::handleEnterKeyPress()
{
    //Parse command
    QString cmd = this->document()->lastBlock().text();
    cmd = cmd.right(cmd.size()-3);

    if(cmd.startsWith("--- "))  //! @todo What does this do?
    {
        return;
    }

    //Append new line (for message output appearing below command)
    this->moveCursor( QTextCursor::End, QTextCursor::MoveAnchor );
    this->append("");

    if(!cmd.isEmpty())
    {
        mpTerminal->setAbortButtonEnabled(true);
        //Execute command
        getHandler()->executeCommand(cmd);

        //Add command to history
        mHistory.prepend(cmd);
        mpTerminal->setAbortButtonEnabled(false);
    }

    //Insert new command line
    this->moveCursor( QTextCursor::End, QTextCursor::MoveAnchor );
    this->setOutputColor(UndefinedMessageType);
    this->insertPlainText(">> ");
    this->moveCursor( QTextCursor::End, QTextCursor::MoveAnchor );

    //Scroll to bottom
    this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum());
}


void TerminalConsole::handleUpKeyPress()
{
    if(mHistory.isEmpty()) { return; }

    this->moveCursor( QTextCursor::End, QTextCursor::MoveAnchor );
    this->moveCursor( QTextCursor::StartOfLine, QTextCursor::MoveAnchor );
    this->moveCursor( QTextCursor::End, QTextCursor::KeepAnchor );
    this->textCursor().removeSelectedText();

    ++mCurrentHistoryItem;
    if(mCurrentHistoryItem > mHistory.size()-1)
    {
        mCurrentHistoryItem = mHistory.size()-1;
    }
    setOutputColor(UndefinedMessageType);
    this->insertPlainText(">> " + mHistory.at(mCurrentHistoryItem));

    this->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
}


void TerminalConsole::handleDownKeyPress()
{
    if(mCurrentHistoryItem == -1) { return; }

    this->moveCursor( QTextCursor::End, QTextCursor::MoveAnchor );
    this->moveCursor( QTextCursor::StartOfLine, QTextCursor::MoveAnchor );
    this->moveCursor( QTextCursor::End, QTextCursor::KeepAnchor );
    this->textCursor().removeSelectedText();

    setOutputColor(UndefinedMessageType);
    --mCurrentHistoryItem;
    if(mCurrentHistoryItem == -1)
    {
        this->insertPlainText(">> ");
    }
    else
    {
        this->insertPlainText(">> " + mHistory.at(mCurrentHistoryItem));
    }

    this->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
}


//! @brief Handles tab key press (used for autocomplete
//! @todo This functionality belongs in the HCOM handler I think
void TerminalConsole::handleTabKeyPress()
{
    ++mCurrentAutoCompleteIndex;
    if(mCurrentAutoCompleteIndex > mAutoCompleteResults.size()-1)
    {
        mCurrentAutoCompleteIndex = 0;
    }

    if(mAutoCompleteResults.isEmpty())
    {
        mAutoCompleteFilter = this->document()->lastBlock().text();
        mAutoCompleteFilter = mAutoCompleteFilter.right(mAutoCompleteFilter.size()-3);
        QStringList args;
        splitWithRespectToQuotations(mAutoCompleteFilter, ' ', args);

        QStringList availableCommands = getHandler()->getCommands();
        for(int c=0; c<availableCommands.size(); ++c)
        {
            if(availableCommands[c].startsWith(mAutoCompleteFilter))
            {
                mAutoCompleteResults.append(availableCommands[c]);
            }
        }

        QStringList variableCmds = QStringList() << "disp" << "chpv" << "chpvr" << "chpvl" << "adpv" << "adpvl" << "adpvr" << "peek" << "poke" << "alias" << "aver" << "min" << "max" << "rmvar" << "chdfsc" << "didfsc";
        for(int c=0; c<variableCmds.size(); ++c)
        {
            if(args[0] == variableCmds[c])
            {
                QStringList variables;
                if(args.size() > 1)
                {
                    getHandler()->getLogVariablesThatStartsWithString(args.last()/*mAutoCompleteFilter.right(mAutoCompleteFilter.size()-variableCmds[c].size())*/,variables);
                }
                else
                {
                    getHandler()->getLogVariablesThatStartsWithString("",variables);
                }
                for(int v=0; v<variables.size(); ++v)
                {
                    QString temp;
                    for(int i=0; i<args.size()-1; ++i)
                    {
                        temp.append(args[i]+" ");
                    }
                    variables[v].prepend(temp/*variableCmds[c]*/);
                }
                mAutoCompleteResults.append(variables);
            }
        }

        QStringList parameterCmds = QStringList() << "chpa " << "dipa ";
        for(int c=0; c<parameterCmds.size(); ++c)
        {
            if(mAutoCompleteFilter.startsWith(parameterCmds[c]))
            {
                QStringList parameters;
                getHandler()->getParameters(mAutoCompleteFilter.right(mAutoCompleteFilter.size()-parameterCmds[c].size())+"*",parameters);
                for(int v=0; v<parameters.size(); ++v)
                {
                    parameters[v].prepend(parameterCmds[c]);
                }
                mAutoCompleteResults.append(parameters);
            }
        }

        QStringList componentCmds = QStringList() << "reco ";
        for(int c=0; c<componentCmds.size(); ++c)
        {
            if(mAutoCompleteFilter.startsWith(componentCmds[c]))
            {
                QList<ModelObject*> components;
                QStringList componentNames;
                getHandler()->getComponents(mAutoCompleteFilter.right(mAutoCompleteFilter.size()-componentCmds[c].size())+"*",components);
                for(int v=0; v<components.size(); ++v)
                {
                    componentNames.append(components[v]->getName());
                    componentNames.last().prepend(componentCmds[c]);
                }
                mAutoCompleteResults.append(componentNames);
            }
        }

        QStringList portCmds = QStringList() << "elog " << "dlog ";
        for(int c=0; c<portCmds.size(); ++c)
        {
            if(mAutoCompleteFilter.startsWith(portCmds[c]))
            {
                QList<Port*> ports;
                QStringList portNames;
                getHandler()->getPorts(mAutoCompleteFilter.right(mAutoCompleteFilter.size()-portCmds[c].size())+"*",ports);
                for(int v=0; v<ports.size(); ++v)
                {
                    portNames.append(ports[v]->getParentModelObjectName()+"."+ports[v]->getName());
                    portNames.last().prepend(portCmds[c]);
                }
                mAutoCompleteResults.append(portNames);
            }
        }

        QStringList directoryCmds = QStringList() << "cd ";
        for(int c=0; c<directoryCmds.size(); ++c)
        {
            if(mAutoCompleteFilter.startsWith(directoryCmds[c]))
            {
                QString pwd = getHandler()->getWorkingDirectory();
                QString dir = pwd;
                QString filter = mAutoCompleteFilter.right(mAutoCompleteFilter.size()-directoryCmds[c].size());
                dir.append("/"+filter);

                QDir parentDir = QDir(dir.left(dir.lastIndexOf("/")));
                QString lastFilterDir = filter.right(filter.size()-filter.lastIndexOf("/")-1);
                QString leftFilterDir = "";
                if(filter.contains("/"))
                {
                    leftFilterDir = filter.left(filter.lastIndexOf("/"))+"/";
                }
                QStringList subDirs = parentDir.entryList(QStringList() << lastFilterDir+"*", QDir::Dirs | QDir::NoDotAndDotDot);

                for(int d=0; d<subDirs.size(); ++d)
                {
                    subDirs[d].prepend(leftFilterDir);
                    mAutoCompleteResults.append(directoryCmds[c]+subDirs[d]);
                }
            }
        }

        QStringList fileCmds = QStringList() << "exec " << "edit ";
        for(int c=0; c<fileCmds.size(); ++c)
        {
            if(mAutoCompleteFilter.startsWith(fileCmds[c]))
            {
                QString pwd = getHandler()->getWorkingDirectory();
                QString dir = pwd;
                QString filter = mAutoCompleteFilter.right(mAutoCompleteFilter.size()-fileCmds[c].size());
                dir.append("/"+filter);

                QDir parentDir = QDir(dir.left(dir.lastIndexOf("/")));
                QString lastFilterDir = filter.right(filter.size()-filter.lastIndexOf("/")-1);
                QString leftFilterDir = "";
                if(filter.contains("/"))
                {
                    leftFilterDir = filter.left(filter.lastIndexOf("/"))+"/";
                }
                QStringList subDirs = parentDir.entryList(QStringList() << lastFilterDir+"*", QDir::AllEntries);

                for(int d=0; d<subDirs.size(); ++d)
                {
                    subDirs[d].prepend(leftFilterDir);
                    mAutoCompleteResults.append(fileCmds[c]+subDirs[d]);
                }
            }
        }

        mCurrentAutoCompleteIndex = 0;
    }

    if(mAutoCompleteResults.isEmpty()) { return; }

    this->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    this->moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    this->moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
    this->textCursor().removeSelectedText();

    QString autoString = mAutoCompleteResults.at(mCurrentAutoCompleteIndex);
    this->setOutputColor(UndefinedMessageType);
    this->insertPlainText(">> "+autoString);

    this->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
}


void TerminalConsole::handleHomeKeyPress()
{
    this->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    this->moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    this->moveCursor(QTextCursor::NextWord, QTextCursor::MoveAnchor);
}

void TerminalConsole::handleEscapeKeyPress()
{
    this->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    this->moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    this->moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
    this->textCursor().removeSelectedText();

    this->setOutputColor(UndefinedMessageType);
    this->insertPlainText(">> ");

    this->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
}


void TerminalConsole::cancelAutoComplete()
{
    mAutoCompleteFilter.clear();
    mAutoCompleteResults.clear();
}

void TerminalConsole::cancelRecentHistory()
{
    mCurrentHistoryItem=-1;
}

