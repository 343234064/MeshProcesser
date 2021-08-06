#include "Mesh.h"

#include <assimp\postprocess.h>




bool MeshContainer::LoadMesh(std::string filename)
{
	const aiScene* pScene = Importer.ReadFile(filename,
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded);

	if (pScene == NULL)
		return false;

	MeshScene = pScene;

	GatherMeshesList(MeshScene->mRootNode, MeshScene);

	return true;
}


void MeshContainer::GatherMeshesList(aiNode* node, const aiScene* scene)
{
	Node NewNode;
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* MeshData = scene->mMeshes[node->mMeshes[i]];
		
		int NumMeshVertices = MeshData->mNumVertices;
		int NumMeshFaces = MeshData->mNumFaces;
		std::string MeshName = MeshData->mName.C_Str();

		NewNode.Meshes.push_back(Mesh(MeshData, NumMeshVertices, NumMeshFaces, MeshName));
		NumTotalMeshes++;
		NumTotalVertices += NumMeshVertices;
		NumTotalFaces += NumMeshFaces;
	}
	MeshList.push_back(NewNode);

	for (int i = 0; i < node->mNumChildren; i++)
	{
		GatherMeshesList(node->mChildren[i], scene);
	}
}


void MeshContainer::Clear()
{
	MeshScene = nullptr;

	Importer.FreeScene();
	Exporter.FreeScene();

	MeshList.clear();
	NumTotalMeshes = 0;
	NumTotalVertices = 0;
	NumTotalFaces = 0;
}