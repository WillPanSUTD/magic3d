//#include "StdAfx.h"
#include "ToolKit.h"
#include <windows.h>

namespace MagicCore
{
    ToolKit* ToolKit::mpToolKit = NULL;

    ToolKit::ToolKit(void) : 
        mAppRunning(true), 
        mIsONIInitialized(false),
        mMousePressLocked(false)
    {
    }

    ToolKit* ToolKit::GetSingleton()
    {
        if (mpToolKit == NULL)
        {
            mpToolKit = new ToolKit;
        }
        return mpToolKit;
    }

    double ToolKit::GetTime()
    {
        static __int64 start = 0;
        static __int64 frequency = 0;

        if (start == 0)
        {
            QueryPerformanceCounter((LARGE_INTEGER*)&start);
            QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
            return 0.0f;
        }

        __int64 counter = 0;
        QueryPerformanceCounter((LARGE_INTEGER*)&counter);
        return (double) ((counter - start) / double(frequency));
    }

    bool ToolKit::FileOpenDlg(std::string& selectFileName, char* filterName)
    {
        char szFileName[MAX_PATH] = "";
        OPENFILENAME file = { 0 };
        file.lStructSize = sizeof(file);
        file.lpstrFile = szFileName;
        file.nMaxFile = MAX_PATH;
        file.lpstrFilter = filterName;
        //file.Flags = OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_ALLOWMULTISELECT|OFN_NOCHANGEDIR;
        file.Flags = OFN_NOCHANGEDIR;
        // �������ļ��ĶԻ���
        if(::GetOpenFileName(&file))
        {
            selectFileName = szFileName;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool ToolKit::FileSaveDlg(std::string& selectFileName, char* filterName)
    {
        char szFileName[MAX_PATH] = "";
        OPENFILENAME file = { 0 };
        file.lStructSize = sizeof(file);
        file.lpstrFile = szFileName;
        file.nMaxFile = MAX_PATH;
        file.lpstrFilter = filterName;
        file.nFilterIndex = 1;//Ĭ��ѡ���һ��
        file.Flags = OFN_NOCHANGEDIR;
        // �������ļ��ĶԻ���
        if(::GetSaveFileName(&file))
        {
            selectFileName = szFileName;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool ToolKit::IsAppRunning()
    {
        return mAppRunning;
    }

    void ToolKit::SetAppRunning(bool bRunning)
    {
        mAppRunning = bRunning;
    }

    bool ToolKit::IsONIInitialized(void)
    {
        return mIsONIInitialized;
    }

    void ToolKit::SetONIInitialized(bool bInitialized)
    {
        mIsONIInitialized = bInitialized;
    }

    MagicDGP::Vector3 ToolKit::ColorCoding(float f)
    {
        MagicDGP::Vector3 colorV;
        if (f >= 0 && f < 0.2)
        {
            colorV[0] = f * 5.0;
            colorV[1] = 0;
            colorV[2] = 0;
        }
        else if (f >= 0.2 && f < 0.4)
        {
            colorV[0] = 1;
            colorV[1] = (f - 0.2) * 5;
            colorV[2] = 0;
        }
        else if (f >= 0.4 && f < 0.6)
        {
            colorV[0] = 1 - (f - 0.4) * 5;
            colorV[1] = 1;
            colorV[2] = 0;
        }
        else if (f >= 0.6 && f < 0.8)
        {
            colorV[0] = 0;
            colorV[1] = 1;
            colorV[2] = (f - 0.6) * 5;
        }
        else if (f >= 0.8 && f < 1)
        {
            colorV[0] = 0;
            colorV[1] = 1 - (f - 0.8) * 5;
            colorV[2] = 1;
        }
        else if (f >= 1 && f <= 1.2)
        {
            colorV[0] = (f - 1) * 5;
            colorV[1] = 0; 
            colorV[2] = 1;
        }
        else if (f > 1.2)
        {
            colorV[0] = 1;
            colorV[1] = 0;
            colorV[2] = 1;
        }
        else
        {
            colorV[0] = 0;
            colorV[1] = 0;
            colorV[2] = 0;
        }
        return colorV;
    }

    void ToolKit::OpenWebsite(std::string& address)
    {
        ShellExecute(NULL, "open", address.c_str(), NULL, NULL, SW_SHOW);
    }

    void ToolKit::SetMousePressLocked(bool locked)
    {
        mMousePressLocked = locked;
    }

    bool ToolKit::IsMousePressLocked() const
    {
        return mMousePressLocked;
    }

    ToolKit::~ToolKit(void)
    {
    }
}

