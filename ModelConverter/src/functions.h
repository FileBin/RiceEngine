#pragma once

#include <iostream>
#include <fbxsdk.h>
#include <string>

using namespace fbxsdk;

#define Print(lvl) std::cout << Space(lvl)

FbxNode* getRootNode(std::string file) {
	FbxManager* manager = FbxManager::Create();

	FbxIOSettings* ioSettings = FbxIOSettings::Create(manager, IOSROOT);
	manager->SetIOSettings(ioSettings);

	FbxImporter* importer = FbxImporter::Create(manager, "");
	importer->Initialize(file.c_str(), -1, manager->GetIOSettings());

	FbxScene* scene = FbxScene::Create(manager, "tempName");

	importer->Import(scene);
	importer->Destroy();

	return scene->GetRootNode();
}

const char* Space(size_t n = 0) {
	n *= 2;
	char* out = new char[n+1];
	for (size_t i = 0; i < n; i++) {
		out[i] = ' ';
	}
	out[n] = '\0';
	return out;
}

void PrintNode(FbxNode* node, int lvl = 0) {
	auto n = node->GetChildCount();
	Print(lvl) << node->GetName() << "\n";
	void* p;
	lvl++;
	if ((p = node->GetMesh()) != nullptr) {
		Print(lvl) << "Mesh:\n";
		lvl++;
		Print(lvl) << "Vertices:\n";
		auto mesh = (FbxMesh*)p;
		auto n = mesh->GetControlPointsCount();
		lvl++;
		for (int i = 0; i < n; i++) {
			auto data = (double*)mesh->GetControlPointAt(i).mData;
			Print(lvl) << "(" << data[0] << "; " << data[1] << "; " << data[2] << "),\n";
		}
		lvl--;

		Print(lvl) << "Indexes:\n";
		lvl++;
		n = mesh->GetPolygonCount();
		for (int i = 0; i < n; i++) {
			auto m = mesh->GetPolygonSize(i);
			Print(lvl) << "{";
			lvl++;
			for (int j = 0; j < m; j++) {
				std::cout << " " << mesh->GetPolygonVertex(i, j) << ",";
			}
			lvl--;
			std::cout << " },\n";
		}
		lvl--;

		Print(lvl) << "Normals:\n";
		lvl++;
		for (int i = 0; i < n; i++) {
			auto m = mesh->GetPolygonSize(i);
			lvl++;
			for (int j = 0; j < m; j++) {
				FbxVector4 norm;
				mesh->GetPolygonVertexNormal(i, j, norm);
				auto data = (double*)norm.mData;
				Print(lvl) << "(" << data[0] << "; " << data[1] << "; " << data[2] << "),\n";
			}
			lvl--;
		}
		lvl--;

		lvl--;
	}
	if (n > 0) {
		Print(lvl) << "Children:\n";
		lvl++;
		for (int i = 0; i < n; i++) {
			auto child = node->GetChild(i);
			PrintNode(child, lvl);
		}
	}
}