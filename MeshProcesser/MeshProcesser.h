#pragma once

#include <string>
#include <vector>

#include "Mesh.h"

class MeshProcesser
{
public:
	MeshProcesser():
		CurrentMeshPath("No mesh has loaded"),
		EnableVertexCacheOptimization(false)
	{}
	~MeshProcesser() {
	}
	MeshProcesser(const MeshProcesser&) = delete;

	void LoadMesh();


public:
	std::string CurrentMeshPath;
	bool EnableVertexCacheOptimization;

	MeshContainer CurrentMesh;



};

