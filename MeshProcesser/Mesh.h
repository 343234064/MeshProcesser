#pragma once

#include <string>
#include <vector>
#include <assimp\scene.h>
#include <assimp\Importer.hpp>
#include <assimp\Exporter.hpp>


enum class MeshFileType
{
	Obj,
	Fbx
};

struct Mesh
{
	Mesh(aiMesh* Data, int NumV, int NumF, std::string& Nme):
		MeshData(Data),
		NumVertices(NumV),
		NumFaces(NumF),
		Name(Nme)
	{}

	aiMesh* MeshData;
	int NumVertices;
	int NumFaces;
	std::string Name;
};

struct Node
{
	std::vector<Mesh> Meshes;
};

class MeshContainer
{
public:
	MeshContainer(const aiScene* InitScene = nullptr) :
		MeshScene(InitScene),
		NumTotalMeshes(0),
		NumTotalVertices(0),
		NumTotalFaces(0)
	{}
	~MeshContainer() {
		/*
			Objects of this class are generally maintained and owned by Assimp, not by the caller. 
			You shouldn't want to instance it, nor should you ever try to delete a given scene on your own.
		*/
		MeshScene = nullptr;
		MeshList.clear();
	}
	MeshContainer(const MeshContainer&) = delete;

	bool LoadMesh(std::string filename);
	bool SaveMesh(std::string filename, MeshFileType FileType);

	void Clear();

	int GetTotalMeshesNum() { return NumTotalMeshes; }
	int GetTotalVerticesNum() { return NumTotalVertices; }
	int GetTotalFacesNum() { return NumTotalFaces; }

	std::vector<Node>& GetMeshList() { return MeshList; }

protected:
	void GatherMeshesList(aiNode* node, const aiScene* scene);

protected:
	const aiScene* MeshScene;
	Assimp::Importer Importer;
	Assimp::Importer Exporter;

	std::vector<Node> MeshList;

	int NumTotalMeshes;
	int NumTotalVertices;
	int NumTotalFaces;
};