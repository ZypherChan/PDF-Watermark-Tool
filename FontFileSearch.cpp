#include <QFont>
#include <qt_windows.h>

#pragma warning(disable: 4996)   

bool geFontFilePathByQFont(const QFont& font, QString& fileName)
{
    QString sNonExactMatched;
    QString sFaceName = font.family();

    OSVERSIONINFO os;
    os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    ::GetVersionEx(&os);

    QString sName = QString::fromLatin1("Software\\Microsoft\\Windows%1\\CurrentVersion\\")
        .arg((os.dwPlatformId & VER_PLATFORM_WIN32_NT) ? QString::fromLatin1(" nt") : QString());

    // prepare wide registry base
    std::wstring base = QString(sName).toStdWString();

    LONG nRes;
    HKEY hFontSubstitutes = NULL;
    DWORD nDataSize = 512;

    HKEY hFonts = NULL;
    std::wstring regFonts = base + L"Fonts\\";
    ::RegOpenKeyExW(HKEY_LOCAL_MACHINE, regFonts.c_str(), 0, KEY_READ, &hFonts);
    if (hFonts != NULL)
    {
        DWORD nIndex = 0;
        DWORD nValNameSize = 256;
        bool n_BT_flag = sFaceName.contains(QLatin1String(" BT"));

        for (;;)
        {
            std::wstring valNameBuf;
            std::vector<BYTE> dataBuf;
            DWORD type = 0;
            nValNameSize = 256;
            nDataSize = 4096;
            valNameBuf.resize(nValNameSize);
            dataBuf.resize(nDataSize);

            nRes = ::RegEnumValueW(hFonts, nIndex, &valNameBuf[0], &nValNameSize, NULL, &type, dataBuf.data(), &nDataSize);

            if (nRes == ERROR_MORE_DATA)
            {
                // resize and retry
                valNameBuf.resize(nValNameSize + 1);
                dataBuf.resize(nDataSize);
                nRes = ::RegEnumValueW(hFonts, nIndex, &valNameBuf[0], &nValNameSize, NULL, &type, dataBuf.data(), &nDataSize);
            }

            if (nRes == ERROR_SUCCESS)
            {
                ++nIndex;
                QString sValueName = QString::fromWCharArray(valNameBuf.c_str(), (int)nValNameSize);
				//qDebug() << "Registry font entry:" << sValueName;
                if (type == REG_SZ || type == REG_EXPAND_SZ)
                {
                    // data is wide string (RegEnumValueW returns bytes of wide chars)
                    QString foundFile;
                    if (nDataSize >= sizeof(wchar_t))
                        foundFile = QString::fromWCharArray((wchar_t*)dataBuf.data(), (int)(nDataSize / sizeof(wchar_t)) - 1);
                    else
                        foundFile.clear();

                    fileName = foundFile;

                    // Trim and match "(TrueType)"
                    if (sValueName.contains(QLatin1String("(TrueType)")))
                    {
                        sValueName.replace(QLatin1String("(TrueType)"), QString());
                        sValueName.replace(QLatin1String("-Regular"), QString());
                        sValueName.replace(QLatin1String("Regular"), QString());
                        // Handle " BT" marker
                        bool cur_BT = sValueName.contains(QLatin1String(" BT"));
                        sValueName.replace(QLatin1String(" BT"), QLatin1String(" "));
                        bool bBold = false;
                        if (cur_BT)
                        {
                            sValueName.replace(QLatin1String(" Extra Bold "), QLatin1String(" XBd "));
                        }
                        else
                        {
                            if (!sValueName.contains(QLatin1String(" Extra Bold ")))
                            {
                                QString before = sValueName;
                                sValueName.replace(QLatin1String(" Bold "), QLatin1String(" "));
                                bBold = (before != sValueName);
                            }
                        }
                        bool bItalic = sValueName.contains(QLatin1String(" Italic "));
                        sValueName.replace(QLatin1String(" Italic "), QLatin1String(" "));
                        QString sValNoSpace = sValueName;
                        sValNoSpace.remove(QLatin1Char(' '));
                        QString sFaceNoSpace = sFaceName;
                        sFaceNoSpace.remove(QLatin1Char(' '));
                        if (sValNoSpace == sFaceNoSpace)
                        {
                            sNonExactMatched = fileName;
                            
                            if (font.bold() == bBold && font.italic() == bItalic)
                                break;
                        }
                        if (sValNoSpace.contains(sFaceNoSpace + QLatin1String("&")) || sValNoSpace.contains(QLatin1String("&") + sFaceNoSpace))
                        {
                            sNonExactMatched = fileName;
                            break;
                        }
                    }
                }
            }
            else if (nRes == ERROR_NO_MORE_ITEMS)
            {
                fileName = sNonExactMatched;
                break;
            }
            else
            {
                fileName.clear();
                break;
            }
        }
        ::RegCloseKey(hFonts);
    }

    if (!fileName.isEmpty())
    {
        if (!fileName.contains(QLatin1Char('\\')))
        {
            // fileName is simple file name -> prepend Windows\fonts
            int nLen = ::GetWindowsDirectoryW(NULL, 0);
            if (nLen)
            {
                std::wstring winDir;
                winDir.resize(nLen);
                ::GetWindowsDirectoryW(&winDir[0], nLen);
                // ensure null-termination
                winDir.resize(wcslen(winDir.c_str()));
                QString testpath = QString::fromWCharArray(winDir.c_str());
                testpath += QString::fromLatin1("\\fonts\\");
                fileName = testpath + fileName;
            }
        }
    }

    return (!fileName.isEmpty());
}