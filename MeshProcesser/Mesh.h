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

	bool LoadMesh(std::string Filename, char* ErrorString = nullptr, int StringLength = 0);
	bool SaveMesh(std::string Filename, MeshFileType FileType, char* ErrorString = nullptr, int StringLength = 0);

	void Clear();

	int GetTotalMeshesNum() { return NumTotalMeshes; }
	int GetTotalVerticesNum() { return NumTotalVertices; }
	int GetTotalFacesNum() { return NumTotalFaces; }

	bool IsLoaded() const { return Loaded; }

	std::unique_ptr<Node>& GetNodeList() { return RootNode; }
	Mesh* GetMesh(int Idx) {
		return &(MeshList[Idx]);
	}
	

	void OptimizeVertexCache();
	void OptimizeOverdraw();



protected:
	Node* GatherNodeList(aiNode* node, std::map<std::string, int>& NameMap);

protected:
	const aiScene* MeshScene;
	Assimp::Importer Importer;
	Assimp::Exporter Exporter;

	std::unique_ptr<Node> RootNode;
	std::vector<Mesh> MeshList;

	int NumTotalMeshes;
	int NumTotalVertices;
	int NumTotalFaces;

	bool Loaded;
};