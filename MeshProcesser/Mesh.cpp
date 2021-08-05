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
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		NewNode.Meshes.push_back(mesh);
		NumTotalMeshes++;
		NumTotalVertices += mesh->mNumVertices;
		NumTotalFaces += mesh->mNumFaces;
	}

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