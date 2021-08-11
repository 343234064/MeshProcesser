#include <ShObjIdl_core.h>
#include "MeshProcesser.h"

#define WRITE_LOG(msg) Log+=msg;Log+='\n';

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


std::string OpenBrowseFileDialog()
{
    IFileOpenDialog* pfd = NULL;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pfd));

    std::string SelectedFile;
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
                    SelectedFile = UTF8FilePath;
                }
                CoTaskMemFree(pszFilePath);

                
            }
            psiaResult->Release();
        }
        pfd->Release();
    }

    return SelectedFile;
}

std::pair<std::string, MeshFileType> OpenSaveFileDialog()
{
    IFileSaveDialog* pfd = NULL;
    HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pfd));

    std::string SelectedFile;
    MeshFileType FileType;
    if (SUCCEEDED(hr))
    {
        DWORD dwFlags;
        COMDLG_FILTERSPEC rgSpec[] =
        {
            { L"OBJ", L"*.obj" },
            { L"FBX", L"*.fbx" }
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
                    UINT SelectedFileType;
                    pfd->GetFileTypeIndex(&SelectedFileType);
                    std::string Ext;
                    switch (SelectedFileType)
                    {
                    case 1:FileType = MeshFileType::Obj; Ext = ".obj"; break;
                    case 2:FileType = MeshFileType::Fbx; Ext = ".fbx"; break;
                    default:
                        FileType = MeshFileType::Obj;
                        break;
                    }
                    std::wstring FilePath = std::wstring(pszFilePath);
                    std::string UTF8FilePath = ToUtf8(FilePath);
                    SelectedFile = UTF8FilePath;

                    size_t DotPos = SelectedFile.find_last_of(".");
                    if (DotPos == std::string::npos)
                    {
                        SelectedFile += Ext;
                    }
                    else
                    {
                        std::string CurrExt = SelectedFile.substr(DotPos);
                        if (CurrExt != Ext)
                            SelectedFile += Ext;
                    }
                }
                CoTaskMemFree(pszFilePath);

            }
            psiaResult->Release();
        }
        pfd->Release();
    }

    return std::pair<std::string, MeshFileType>(SelectedFile, FileType);
}


void MeshProcesser::LoadMesh()
{
    CurrentMeshPath = OpenBrowseFileDialog();

    CurrentMesh.Clear();

    if (!CurrentMesh.LoadMesh(CurrentMeshPath, ErrorString, 1024))
        CurrentMeshPath = std::string("Load file failed");

    if (ErrorString[0] != '\0') {
        WRITE_LOG(ErrorString);
    }
    else
        WRITE_LOG("Done.");

	return ;
}


void MeshProcesser::SaveMesh()
{
    if (!CurrentMesh.IsLoaded()) {
        WRITE_LOG("Please first load a mesh.")
        return;
    }

    std::pair<std::string, MeshFileType> FilePath = OpenSaveFileDialog();
    bool Result = CurrentMesh.SaveMesh(FilePath.first, FilePath.second, ErrorString, 1024);

    if (Result) {
        WRITE_LOG("Saved To:");
    }
    else
    {
        WRITE_LOG("Failed To Save:");
    }
    WRITE_LOG(FilePath.first);
}


void MeshProcesser::OptimizeMesh()
{
    if (!CurrentMesh.IsLoaded()) {
        WRITE_LOG("Please first load a mesh.")
            return;
    }

    if (EnableVertexCacheOptimization)
        CurrentMesh.OptimizeVertexCache();

    WRITE_LOG("Optimize Vertex Cache Done.");

    if (EnableOverdrawOptimization)
        CurrentMesh.OptimizeOverdraw();

    WRITE_LOG("Optimize Overdraw Done.");
}


void MeshProcesser::AdjacencyData()
{

}