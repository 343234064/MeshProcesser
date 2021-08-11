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
	{
		ErrorString[0] = '\0';
		LogString[0] = '\0';
	}
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
	char ErrorString[1024];
	char LogString[8096];
	std::string Log;

	MeshContainer CurrentMesh;



};

