#include <iostream>
#include <stdexcept> // for std::runtime_error

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "vloader.hpp"

namespace vload {

vloader::vloader(std::string_view npath, bool nindex, bool nnormals, bool ntangents)
	: path(npath), index(nindex), normals(nnormals), tangents(ntangents), meshList() {}

void vloader::load() {
	Assimp::Importer imp;

	// make everything triangles, generate normals if they aren't there, join identical vertices together, remove bad model data
	unsigned int flags = aiProcess_Triangulate 
						| aiProcess_OptimizeMeshes
						| aiProcess_OptimizeGraph
						| aiProcess_FindInvalidData;
	
	if (index) {
		// if JoinIdenticalVertices isn't specified, an index buffer isn't required.
		flags |= aiProcess_JoinIdenticalVertices;
	}

	if (normals) {
		// create normals for all model vertices (joined or not) and fix inverted normals
		flags |= aiProcess_GenSmoothNormals;
	}

	if (tangents) {
		flags |= aiProcess_CalcTangentSpace;
	}
	
	const aiScene* scene = imp.ReadFile(path.data(), flags);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::string err = imp.GetErrorString();
		throw std::runtime_error(err);
	}

	processNode(scene->mRootNode, scene);
}

void vloader::dispatch() {
	t = std::thread([&]() -> void { load(); });
}

void vloader::join() {
    if (!t.joinable()) {
        throw std::runtime_error("cannot join non-joinable thread!");
    }

    t.join();
}

// recursively visit nodes
void vloader::processNode(aiNode* node, const aiScene* scene) {
	for (size_t i = 0; i < node->mNumMeshes; i++) {
		aiMesh* temp = scene->mMeshes[node->mMeshes[i]];
		meshList.push_back(processMesh(temp));
	}

	for (size_t i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene); // process children
	}
}

mesh vloader::processMesh(aiMesh* inMesh) {
	std::vector<vformat::vertex> vList;
	std::vector<uint32_t> indices;

	vList.resize(inMesh->mNumVertices);
	
	for (size_t i = 0; i < inMesh->mNumVertices; i++) { // extract position information
		auto& v = vList[i];
		v.pos.x = inMesh->mVertices[i].x;
		v.pos.y = inMesh->mVertices[i].y;
		v.pos.z = inMesh->mVertices[i].z;

		v.normal.x = inMesh->mNormals[i].x;
		v.normal.y = inMesh->mNormals[i].y;
		v.normal.z = inMesh->mNormals[i].z;
	}

	// can't call resize here - might want to import point clouds with 1 vertex per face
	for (size_t i = 0; i < inMesh->mNumFaces; i++) { // extract element information
		aiFace f = inMesh->mFaces[i];
		for (size_t j = 0; j < f.mNumIndices; j++) {
			indices.push_back(f.mIndices[j]);
		}
	}

	if (inMesh->mTextureCoords[0]) {
		for (size_t i = 0; i < inMesh->mNumVertices; i++) {
			glm::vec2 coord;
			coord.s = inMesh->mTextureCoords[0][i].x;
			coord.t = inMesh->mTextureCoords[0][i].y;
			vList[i].uv = coord;
		}
	} else {
		std::cout << "mesh has no texture coordinates\n";
	}
	
	// not using tangents right now
	if (inMesh->HasTangentsAndBitangents()) {
		if (inMesh->mTextureCoords[0]) {
			for (size_t i = 0; i < inMesh->mNumVertices; i++) {
				glm::vec3 tangent;	
				tangent.x = inMesh->mTangents[i].x;
				tangent.y = inMesh->mTangents[i].y;
				tangent.z = inMesh->mTangents[i].z;
				vList[i].tangent = tangent;
			}
		} else {
			std::cout << "skipping tangent calculation\n";
		}
	}
	
	return mesh(vList, indices);
}

}
