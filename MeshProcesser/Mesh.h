#pragma once

#include <string>
#include <vector>
#include <memory>
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
	std::string Name;
	std::vector<int> MeshIdx;
	std::vector<std::unique_ptr<Node>> Childs;
};

class MeshContainer
{
public:
	MeshContainer(const aiScene* InitScene = nullptr) :
		MeshScene(InitScene),
		RootNode(nullptr),
		NumTotalMeshes(0),
		NumTotalVertices(0),
		NumTotalFaces(0),
		Loaded(false)
	{}
	~MeshContainer() {
		Clear();
	}
	MeshContainer(const MeshContainer&) = delete;

	bool LoadMesh(std::string filename);
	bool SaveMesh(std::string filename, MeshFileType FileType);

	void Clear();

	int GetTotalMeshesNum() { return NumTotalMeshes; }
	int GetTotalVerticesNum() { return NumTotalVertices; }
	int GetTotalFacesNum() { return NumTotalFaces; }

	std::unique_ptr<Node>& GetNodeList() { return RootNode; }
	Mesh* GetMesh(int Idx) {
		return &(MeshList[Idx]);
	}
	
	void OptimizeVertexCache();
	void OptimizeOverdraw();

protected:
	Node* GatherNodeList(aiNode* node);

protected:
	const aiScene* MeshScene;
	Assimp::Importer Importer;
	Assimp::Importer Exporter;

	std::unique_ptr<Node> RootNode;
	std::vector<Mesh> MeshList;

	int NumTotalMeshes;
	int NumTotalVertices;
	int NumTotalFaces;

	bool Loaded;
};