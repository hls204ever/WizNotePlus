﻿#include "WizCommonUI.h"

#include <QClipboard>
#include <QApplication>
#include <QUuid>
#include <QDir>
#include <QDebug>
#include <QByteArray>
#include <QSaveFile>

#include "WizMisc.h"
#include "utils/WizPathResolve.h"
#include "share/WizMisc.h"


WizCommonUI::WizCommonUI(QObject* parent)
    : QObject(parent)
{
}

QString WizCommonUI::loadTextFromFile(const QString& strFileName)
{
    QString strText;
    ::WizLoadUnicodeTextFromFile(strFileName, strText);
    return strText;
}

bool WizCommonUI::saveTextToFile(const QString &strFileName, const QString &strText, const QString &strCharset)
{
    QString charset =  strCharset.toLower();
    if (charset == "unicode" || charset == "utf-8") {
        return ::WizSaveUnicodeTextToUtf8File(strFileName, strText, false);
    } else if (charset == "utf-16") {
        return ::WizSaveUnicodeTextToUtf16File(strFileName, strText);
    } else {
        return ::WizSaveUnicodeTextToUtf8File(strFileName, strText);
    }
}

QString WizCommonUI::clipboardToImage(int hwnd, const QString& strOptions)
{
    Q_UNUSED(hwnd);
    //
    QClipboard* clipboard = QApplication::clipboard();
    if (!clipboard)
        return CString();
    //
    //
    QImage image = clipboard->image();
    if (image.isNull())
        return CString();
    //
    CString strTempPath = ::WizGetCommandLineValue(strOptions, "TempPath");
    if (strTempPath.isEmpty())
    {
        strTempPath = Utils::WizPathResolve::tempPath();
    }
    else
    {
        ::WizPathAddBackslash(strTempPath);
        ::WizEnsurePathExists(strTempPath);
    }
    //
    CString strFileName = strTempPath + WizIntToStr(WizGetTickCount()) + ".png";
    if (!image.save(strFileName))
        return CString();
    //
    return strFileName;
}

QString WizCommonUI::LoadTextFromFile(const QString& strFileName)
{
    return loadTextFromFile(strFileName);
}

bool WizCommonUI::SaveTextToFile(const QString &strFileName, const QString &strText, const QString &strCharset)
{
    return saveTextToFile(strFileName, strText, strCharset);
}

QString WizCommonUI::ClipboardToImage(int hwnd, const QString& strOptions)
{
    return clipboardToImage(hwnd, strOptions);
}

QString WizCommonUI::GetSpecialFolder(const QString &bstrFolderName)
{
    if (bstrFolderName == "TemporaryFolder") {
        return Utils::WizPathResolve::tempPath();
    } else if (bstrFolderName == "AppPath") {
        return Utils::WizPathResolve::appPath();
    } else {
        return "";
    }
}

QString WizCommonUI::GetATempFileName(const QString &bstrFileExt)
{
    QString strTempFileName = QUuid::createUuid().toString() + bstrFileExt;
    return QDir(GetSpecialFolder("TemporaryFolder")).absoluteFilePath(strTempFileName);
}

/**
 * @brief Creates the directory path dirPath.
 * 
 *      The function will create all parent directories necessary to create the directory.
 * 
 * @param bstrPath 
 * @return true 
 * @return false 
 */
bool WizCommonUI::CreateDirectory(const QString &bstrPath)
{
    return QDir().mkpath(bstrPath);
}

/**
 * @brief Download resource to file.
 * 
 * @param bstrURL 
 * @param bstrFileName 
 * @return true 
 * @return false 
 */
bool WizCommonUI::URLDownloadToFile(const QString &bstrURL, const QString &bstrFileName, bool isImage)
{
    return WizURLDownloadToFile(bstrURL, bstrFileName, isImage);
}


bool WizCommonUI::Base64ToFile(const QString &base64, const QString &fileName)
{
    QByteArray buffer = QByteArray::fromBase64(base64.toUtf8());
    QSaveFile file(fileName);
    file.open(QIODevice::WriteOnly);
    file.write(buffer);
    file.commit();
    return true;
}