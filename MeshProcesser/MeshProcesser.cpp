#include <ShObjIdl_core.h>
#include "MeshProcesser.h"



std::string ToUtf8(const std::wstring& str)
{
    std::string ret;
    int len = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), (int)str.length(), NULL, 0, NULL, NULL);
    if (len > 0)
    {
        ret.resize(len);
        WideCharToMultiByte(CP_UTF8, 0, str.c_str(), (int)str.length(), &ret[0], len, NULL, NULL);
    }
    return ret;
}


std::string BrowseFile()
{
    IFileOpenDialog* pfd = NULL;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pfd));

    std::string SeletedFile;
    if (SUCCEEDED(hr))
    {
        DWORD dwFlags;
        COMDLG_FILTERSPEC rgSpec[] =
        {
            { L"OBJ/FBX", L"*.obj;*.fbx" }
        };

        hr = pfd->GetOptions(&dwFlags);
        hr = pfd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);
        hr = pfd->SetFileTypes(ARRAYSIZE(rgSpec), rgSpec);

        hr = pfd->Show(NULL);
        if (SUCCEEDED(hr))
        {
            IShellItem* psiaResult;
            hr = pfd->GetResult(&psiaResult);
            if (SUCCEEDED(hr))
            {
                PWSTR pszFilePath = NULL;
                hr = psiaResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                if (SUCCEEDED(hr))
                {
                    std::wstring FilePath = std::wstring(pszFilePath);
                    std::string UTF8FilePath = ToUtf8(FilePath);
                    SeletedFile = UTF8FilePath;
                }
                CoTaskMemFree(pszFilePath);

                
            }
            psiaResult->Release();
        }
        pfd->Release();
    }
    CoUninitialize();

    return SeletedFile;
}


void MeshProcesser::LoadMesh()
{
    CurrentMeshPath = BrowseFile();

    CurrentMesh.Clear();

    if (!CurrentMesh.LoadMesh(CurrentMeshPath))
        CurrentMeshPath = std::string("Load file failed");


	return ;
}