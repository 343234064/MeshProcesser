#include "Mesh.h"

#include <assimp\postprocess.h>
#include "meshoptimizer\meshoptimizer.h"



bool MeshContainer::LoadMesh(std::string filename)
{
	const aiScene* pScene = Importer.ReadFile(filename,
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded);

	if (pScene == NULL)
		return false;

	MeshScene = pScene;

	Node* Root = GatherNodeList(MeshScene->mRootNode);
	RootNode.reset(Root);


	for (int i = 0; i < MeshScene->mNumMeshes; i++)
	{
		aiMesh* MeshData = MeshScene->mMeshes[i];

		int NumMeshVertices = MeshData->mNumVertices;
		int NumMeshFaces = MeshData->mNumFaces;
		std::string MeshName = MeshData->mName.C_Str();

		MeshList.push_back(Mesh(MeshData, NumMeshVertices, NumMeshFaces, MeshName));

		NumTotalMeshes++;
		NumTotalVertices += NumMeshVertices;
		NumTotalFaces += NumMeshFaces;
	}
	
	Loaded = true;

	return true;
}


Node* MeshContainer::GatherNodeList(aiNode* node)
{
	Node* NewNode = new Node();
	NewNode->Name = node->mName.C_Str();

	for (int i = 0; i < node->mNumMeshes; i++)
	{
		NewNode->MeshIdx.push_back(node->mMeshes[i]);
	}

	for (int i = 0; i < node->mNumChildren; i++)
	{
		Node* ChildNode = GatherNodeList(node->mChildren[i]);
		NewNode->Childs.push_back(std::unique_ptr<Node>(ChildNode));
	}

	return NewNode;
}


void MeshContainer::Clear()
{
	/*
	Objects of this class are generally maintained and owned by Assimp, not by the caller.
	You shouldn't want to instance it, nor should you ever try to delete a given scene on your own.
	*/
	MeshScene = nullptr;

	Importer.FreeScene();
	Exporter.FreeScene();

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