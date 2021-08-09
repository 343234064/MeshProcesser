#pragma once

#include <string>
#include <vector>

#include "Mesh.h"

class MeshProcesser
{
public:
	MeshProcesser():
		CurrentMeshPath("No mesh has loaded"),
		EnableVertexCacheOptimization(true),
		EnableOverdrawOptimization(true)
	{}
	~MeshProcesser() {
	}
	MeshProcesser(const MeshProcesser&) = delete;

	void LoadMesh();
	void SaveMesh();
	void OptimizeMesh();
	void AdjacencyData();

public:
	std::string CurrentMeshPath;
	bool EnableVertexCacheOptimization;
	bool EnableOverdrawOptimization;

	MeshContainer CurrentMesh;



};

