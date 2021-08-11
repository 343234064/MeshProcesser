#include "Mesh.h"
#include <iostream>
#include <map>
#include <assimp\postprocess.h>
#include "meshoptimizer\meshoptimizer.h"

void CheckNameExist(std::map<std::string, int>& NameMap, std::string& ToCheck)
{
	if (NameMap.count(ToCheck) == 0)
	{
		NameMap.insert(std::pair<std::string, int>(ToCheck, 1));
	}
	else
	{
		std::string NewName;
		int NameCount = NameMap[ToCheck] + 1;
		while (true)
		{
			NewName = ToCheck + "_" + std::to_string(NameCount);
			if (NameMap.count(NewName) == 0)
			{
				break;
			}
			NameCount++;
			if (NameCount >= 100)
			{
				break;
			}
		}
		  
		ToCheck = NewName;
		NameMap[ToCheck] = NameCount;
		NameMap.insert(std::pair<std::string, int>(ToCheck, 1));
	}
}

Node* MeshContainer::GatherNodeList(aiNode* node, std::map<std::string, int>& NameMap)
{
	Node* NewNode = new Node();
	NewNode->Name = node->mName.C_Str();

	CheckNameExist(NameMap, NewNode->Name);

	for (int i = 0; i < node->mNumMeshes; i++)
	{
		NewNode->MeshIdx.push_back(node->mMeshes[i]);
	}

	for (int i = 0; i < node->mNumChildren; i++)
	{
		Node* ChildNode = GatherNodeList(node->mChildren[i], NameMap);
		NewNode->Childs.push_back(std::unique_ptr<Node>(ChildNode));
	}

	return NewNode;
}

bool MeshContainer::LoadMesh(std::string filename, char* ErrorString, int StringLength)
{
	const aiScene* pScene = Importer.ReadFile(filename,
		aiProcess_Triangulate |
		aiProcess_ValidateDataStructure |
		aiProcess_ImproveCacheLocality |
		aiProcess_OptimizeMeshes |
		aiProcess_RemoveRedundantMaterials
	);

	if (ErrorString != nullptr) {
		const char* Message = Importer.GetErrorString();
		sprintf_s(ErrorString, StringLength, Message);
	}

	if (pScene == NULL)
		return false;

	MeshScene = pScene;

	std::map<std::string, int> NameMap;
	Node* Root = GatherNodeList(MeshScene->mRootNode, NameMap);
	RootNode.reset(Root);


	for (int i = 0; i < MeshScene->mNumMeshes; i++)
	{
		aiMesh* MeshData = MeshScene->mMeshes[i];

		int NumMeshVertices = MeshData->mNumVertices;
		int NumMeshFaces = MeshData->mNumFaces;
		std::string MeshName = MeshData->mName.C_Str();

		CheckNameExist(NameMap, MeshName);

		MeshList.push_back(Mesh(MeshData, NumMeshVertices, NumMeshFaces, MeshName));

		NumTotalMeshes++;
		NumTotalVertices += NumMeshVertices;
		NumTotalFaces += NumMeshFaces;
	}
	
	Loaded = true;

	return true;
}

bool MeshContainer::SaveMesh(std::string Filename, MeshFileType FileType, char* ErrorString, int StringLength)
{
	if (!Loaded) return true;

	std::string FileTypeStr;
	switch (FileType)
	{
	case MeshFileType::Obj:
		FileTypeStr = "obj"; break;
	case MeshFileType::Fbx:
		FileTypeStr = "fbx"; break;
	default:
		FileTypeStr = "obj";
		break;
	}

	aiReturn Result = Exporter.Export(MeshScene, FileTypeStr, Filename);
	/*
	int n = Exporter.GetExportFormatCount();
	for (int i = 0; i < n; i++) {
		const aiExportFormatDesc* formatDescription = Exporter.GetExportFormatDescription(i);
		std::cout << formatDescription->description << std:: endl;
		std::cout << formatDescription->fileExtension << std::endl;
		std::cout << formatDescription->id << std::endl;
		std::cout << "============================" << std::endl;
	}*/
	if (ErrorString != nullptr) {
		const char* Message = Exporter.GetErrorString();
		sprintf_s(ErrorString, StringLength, Message);
	}

	if (Result != aiReturn::aiReturn_SUCCESS)
	{
		return false;
	}

	return true;
}



void MeshContainer::Clear()
{
	/*
	Objects of this class are generally maintained and owned by Assimp, not by the caller.
	You shouldn't want to instance it, nor should you ever try to delete a given scene on your own.
	*/
	MeshScene = nullptr;

	Importer.FreeScene();


	RootNode.reset();
	MeshList.clear();

	NumTotalMeshes = 0;
	NumTotalVertices = 0;
	NumTotalFaces = 0;

	Loaded = false;
}


void MeshContainer::OptimizeVertexCache()
{
	if (!Loaded) return;

	for (int i = 0; i < MeshList.size(); i++)
	{
		Mesh& MeshObj = MeshList[i];
		aiMesh* MeshData = MeshObj.MeshData;

		aiFace* Faces = MeshData->mFaces;
		meshopt_optimizeVertexCache(Faces->mIndices, Faces->mIndices, Faces->mNumIndices, MeshData->mNumVertices);
	}
}


void MeshContainer::OptimizeOverdraw()
{
	if (!Loaded) return;

	for (int i = 0; i < MeshList.size(); i++)
	{
		Mesh& MeshObj = MeshList[i];
		aiMesh* MeshData = MeshObj.MeshData;

		aiFace* Faces = MeshData->mFaces;
		aiVector3D* Vertices = MeshData->mVertices;
		meshopt_optimizeOverdraw(Faces->mIndices, Faces->mIndices, Faces->mNumIndices, (float*)Vertices, MeshData->mNumVertices, sizeof(aiVector3D), 1.05f);
	}
}