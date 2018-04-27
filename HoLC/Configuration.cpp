/*-----------------------------------------------------------------------------

 Copyright 2017 Hopsan Group

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 The full license is available in the file GPLv3.
 For details about the 'Hopsan Group' or information about Authors and
 Contributors see the HOPSANGROUP and AUTHORS files that are located in
 the Hopsan source code root directory.

-----------------------------------------------------------------------------*/

//Qt includes
#include <QDomElement>
#include <QDesktopServices>
#include <QMessageBox>
//#include <QMap>
//#include <QAction>
//#include <QApplication>
#include <QMainWindow>

#include "Configuration.h"
#include "Utilities/XMLUtilities.h"

//! @class Configuration
//! @brief The Configuration class is used as a global XML-based storage for program configuration variables
//!
//! Use loadFromXml or saveToXml functions to read or write to holcconfig.xml. Use get, set, add and clear functions to access the data.
//!

#if QT_VERSION >= 0x050000
#include <QStandardPaths>
QString getStandardLocation(QStandardPaths::StandardLocation type)
{
    QString location;
    QStringList locations = QStandardPaths::standardLocations(type);
    if (!locations.isEmpty())
    {
        // Take first reported location
        location = locations.first();
        // Append '/' to end if not already present
        if (location[location.size()-1] != '/')
        {
            location.append('/');
        }
    }
    return location;
}
#endif


Configuration::Configuration(QWidget *pParent)
{
    mpParent = pParent;
    connect(this, SIGNAL(configChanged()), SLOT(saveToXml()));
}


//! @brief Saves the current settings to holcconfig.xml
void Configuration::saveToXml()
{
#if QT_VERSION >= 0x050000
    QString configPath = getStandardLocation(QStandardPaths::DataLocation);
#else
    QString configPath = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/Hopsan/";
#endif

    // Write to holcconfig.xml
    QDomDocument domDocument;
    QDomElement configRoot = domDocument.createElement("holcconfig");
    configRoot.setAttribute("version", "1.0");
    domDocument.appendChild(configRoot);

    QDomElement settings = appendDomElement(configRoot,"settings");
    appendDomBooleanNode(settings, "alwayssavebeforecompiling", mAlwaysSaveBeforeCompiling);
    appendDomTextNode(settings, "projectpath", mProjectPath);
    appendDomTextNode(settings, "hopsanpath", mHopsanPath);
    appendDomTextNode(settings, "compilerpath", mCompilerPath);
    appendDomBooleanNode(settings, "usetextwrapping", mUseTextWrapping);

    QDomElement recentLibs = appendDomElement(configRoot,"recentlibraries");
    foreach(const QString &lib, mRecentLibraries)
    {
        appendDomTextNode(recentLibs, "recentlibrary", lib);
    }

    appendRootXMLProcessingInstruction(domDocument);

    //Save to file
    if(!QDir(configPath).exists())
    {
        QDir().mkpath(configPath);
    }
    QFile xmlsettings(configPath + "holcconfig.xml");
    if (!xmlsettings.open(QIODevice::WriteOnly | QIODevice::Text))  //open file
    {
        QMessageBox::critical(mpParent, "Error", "Failed to open config file for writing: "+configPath+"holcconfig.xml");
        return;
    }
    QTextStream out(&xmlsettings);
    domDocument.save(out, 4);
}


//! @brief Updates all settings from holcconfig.xml
void Configuration::loadFromXml()
{
    //Read from holcdefaults.xml
    loadDefaultsFromXml();

    //Read from holcconfig.xml
#if QT_VERSION >= 0x050000
    QFile file(getStandardLocation(QStandardPaths::DataLocation) +"holcconfig.xml");
#else
    QFile file(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/Hopsan/" + QString("holcconfig.xml"));
#endif

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //QMessageBox::warning(mpParent, "Warning", "Unable to find configuration file. Configuration file was recreated with default settings.");
        return;
    }

    QDomDocument domDocument;
    QString errorStr;
    int errorLine, errorColumn;
    if (!domDocument.setContent(&file, false, &errorStr, &errorLine, &errorColumn))
    {
        QMessageBox::information(mpParent, mpParent->tr("HoLC"),
                                 mpParent->tr("holcconfig.xml: Parse error at line %1, column %2:\n%3")
                                 .arg(errorLine)
                                 .arg(errorColumn)
                                 .arg(errorStr));
    }
    else
    {
        QDomElement configRoot = domDocument.documentElement();
        if (configRoot.tagName() != "holcconfig")
        {
            QMessageBox::information(mpParent, mpParent->tr("HoLC"),
                                     "The file is not an HoLC Configuration File. Incorrect hmf root tag name: "
                                     + configRoot.tagName() + " != holcconfig");
        }
        else
        {
            //Load user settings
            QDomElement settingsElement = configRoot.firstChildElement("settings");
            loadUserSettings(settingsElement);

            //Load style settings
            QDomElement styleElement = configRoot.firstChildElement("style");
            loadStyleSettings(styleElement);

            //Load recent libraries
            QDomElement recentLibsElement = configRoot.firstChildElement("recentlibraries");
            loadRecentLibraries(recentLibsElement);
        }
    }
    file.close();
}



void Configuration::loadDefaultsFromXml()
{
    QString execPath = qApp->applicationDirPath().append('/');

    //Read from holcdefaults.xml
    QFile file(execPath + "../HoLC/holcdefaults");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::information(mpParent, mpParent->tr("Hopsan"),
                                 "Unable to read default configuration file. Please reinstall program.");

        qApp->quit();
    }
    QDomDocument domDocument;
    QString errorStr;
    int errorLine, errorColumn;
    if (!domDocument.setContent(&file, false, &errorStr, &errorLine, &errorColumn))
    {
        QMessageBox::information(mpParent, mpParent->tr("Hopsan"),
                                 mpParent->tr("holcdefaults: Parse error at line %1, column %2:\n%3")
                                 .arg(errorLine)
                                 .arg(errorColumn)
                                 .arg(errorStr));
    }
    else
    {
        QDomElement configRoot = domDocument.documentElement();
        if (configRoot.tagName() != "holcdefaults")
        {
            QMessageBox::information(mpParent, mpParent->tr("Hopsan"),
                                     "The file is not an Hopsan Configuration File. Incorrect hmf root tag name: "
                                     + configRoot.tagName() + " != holcdefaults");
        }
        else
        {
                //Load default user settings
            QDomElement settingsElement = configRoot.firstChildElement("settings");
            loadUserSettings(settingsElement);

                //Load default GUI style
            QDomElement styleElement = configRoot.firstChildElement("style");
            loadStyleSettings(styleElement);
        }
    }
    file.close();

    if(mHopsanPath.isEmpty())
    {
        QString testPath = QDir(execPath+"../").absolutePath();

        bool success = true;
        QString lib, includeDir;
    #ifdef _WIN32
        lib = testPath+"/bin/hopsancore.dll";
    #else
        lib = testPath+"/bin/libhopsancore.so";
    #endif
        if(!QFile::exists(lib))
        {
            success = false;
        }
        includeDir = testPath+"/HopsanCore/include";
        if(!QFile::exists(includeDir+"/HopsanCore.h"))
        {
            success = false;
        }

        if(success)
        {
            mHopsanPath = testPath;
            qDebug() << "Found Hopsan installation at: " << testPath << "\n";
        }
    }

    if(mCompilerPath.isEmpty())
    {
        QString testDir = QDir(execPath+"../mingw64/bin").absolutePath();
        if(QFile().exists(testDir+"/g++.exe"))
        {
            setCompilerPath(testDir);
            qDebug() << "Found MinGW64 installation at: " << testDir << "\n";
        }

    }

    return;
}

bool Configuration::getAlwaysSaveBeforeCompiling() const
{
    return mAlwaysSaveBeforeCompiling;
}

//! @brief Utility function that loads user settings from dom element
void Configuration::loadUserSettings(QDomElement &rDomElement)
{
    mAlwaysSaveBeforeCompiling = parseDomBooleanNode(rDomElement.firstChildElement("alwayssavebeforecompiling"), false);
    mProjectPath = rDomElement.firstChildElement("projectpath").text();
    mHopsanPath = rDomElement.firstChildElement("hopsanpath").text();
    mCompilerPath = rDomElement.firstChildElement("compilerpath").text();
    mUseTextWrapping = parseDomBooleanNode(rDomElement.firstChildElement("usetextwrapping"), false);

    //Default to installed gcc path in linux (if file exists and no other path specified)
    if(mCompilerPath.isEmpty())
    {
#ifdef __linux__
        if(QFile::exists("/usr/bin/gcc"))
        {
            mCompilerPath = "/usr/bin";
        }
#elif _WIN32
        if(QFile::exists(qApp->applicationDirPath()+"/../Dependencies/tools/mingw32/bin/g++.exe"))
        {
            mCompilerPath = qApp->applicationDirPath()+"/../Dependencies/tools/mingw32/bin";
        }
#endif
    }
}



//! @brief Utility function that loads style settings from dom element
void Configuration::loadStyleSettings(QDomElement &rDomElement)
{
    QDomElement paletteElement = rDomElement.firstChildElement("palette");
    if(!paletteElement.isNull())
    {
        double red, green, blue;
        QColor windowText, button, light, dark, mid, text, bright_text, base, window;
        parseRgbString(paletteElement.attribute("windowText"), red, green, blue);
        windowText.setRgb(red, green, blue);
        parseRgbString(paletteElement.attribute("button"), red, green, blue);
        button.setRgb(red, green, blue);
        parseRgbString(paletteElement.attribute("light"), red, green, blue);
        light.setRgb(red, green, blue);
        parseRgbString(paletteElement.attribute("dark"), red, green, blue);
        dark.setRgb(red, green, blue);
        parseRgbString(paletteElement.attribute("mid"), red, green, blue);
        mid.setRgb(red, green, blue);
        parseRgbString(paletteElement.attribute("text"), red, green, blue);
        text.setRgb(red, green, blue);
        parseRgbString(paletteElement.attribute("bright_text"), red, green, blue);
        bright_text.setRgb(red, green, blue);
        parseRgbString(paletteElement.attribute("base"), red, green, blue);
        base.setRgb(red, green, blue);
        parseRgbString(paletteElement.attribute("window"), red, green, blue);
        window.setRgb(red, green, blue);
        mPalette = QPalette(windowText, button, light, dark, mid, text, bright_text, base, window);
    }
    QDomElement styleSheetElement = rDomElement.firstChildElement("stylesheet");
    if(!styleSheetElement.isNull())
    {
        mStyleSheet.append(styleSheetElement.text());
    }
}

void Configuration::loadRecentLibraries(QDomElement &rDomElement)
{
    mRecentLibraries.clear();
    QDomElement libElement = rDomElement.firstChildElement("recentlibrary");
    while(!libElement.isNull())
    {
        mRecentLibraries.append(libElement.text());
        libElement = libElement.nextSiblingElement("recentlibrary");
    }
}


//! @brief Returns which library style to use
QString Configuration::getProjectPath() const
{
    return mProjectPath;
}

QString Configuration::getHopsanPath() const
{
    return mHopsanPath;
}

QString Configuration::getIncludePath() const
{
    return mHopsanPath+"/HopsanCore/include";
}

QString Configuration::getHopsanCoreLibPath() const
{
#ifdef __linux__
    return mHopsanPath+"/bin/libhopsancore.so";
#elif _WIN32
    return mHopsanPath+"/bin/hopsancore.dll";
#endif
}

QString Configuration::getCompilerPath() const
{
    return mCompilerPath;
}

bool Configuration::getUseTextWrapping() const
{
    return mUseTextWrapping;
}

void Configuration::setProjectPath(const QString &value)
{
    mProjectPath = value;
    emit configChanged();
}

void Configuration::setHopsanPath(const QString &value)
{
    mHopsanPath = value;
    emit configChanged();
}

void Configuration::setCompilerPath(const QString &value)
{
    mCompilerPath = value;
    emit configChanged();
}

void Configuration::setUseTextWrapping(const bool &value)
{
    mUseTextWrapping = value;
    emit configChanged();
}

void Configuration::addRecentLibrary(const QString &value)
{
    mRecentLibraries.append(value);
    mRecentLibraries.removeDuplicates();
    while(mRecentLibraries.size() > 10)
    {
        mRecentLibraries.removeFirst();
    }
}


QPalette Configuration::getPalette()
{
    return mPalette;
}


//! @brief Returns the current style sheet
QString Configuration::getStyleSheet()
{
    return mStyleSheet;
}

//! @brief Returns a list of recent libraries
QStringList Configuration::getRecentLibraries()
{
    return mRecentLibraries;
}


void Configuration::setAlwaysSaveBeforeCompiling(const bool &value)
{
    mAlwaysSaveBeforeCompiling = value;
    emit configChanged();
}

