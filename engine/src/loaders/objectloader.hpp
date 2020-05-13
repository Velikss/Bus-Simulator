#pragma once
#include <pch.hpp>
#include <vendor/tiny_obj_loader.h>

// Very, VERY simple OBJ loader.
// Here is a short list of features a real function would provide : 
// - Binary files. Reading a model should be just a few memcpy's away, not parsing a file at runtime. In short : OBJ is not very great.
// - Animations & bones (includes bones weights)
// - Multiple UVs
// - All attributes should be optional, not "forced"
// - More stable. Change a line in the OBJ file and it crashes.
// - More secure. Change another line and you can inject code.
// - Loading from memory, stream, etc

bool loadOBJ(
    const char* path,
    std::vector<glm::vec3>& vertices,
    std::vector<glm::vec2>& uvs,
    std::vector<glm::vec3>& normals
    ) 
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path)) {
        throw std::runtime_error(err);
        return false;
    }

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            glm::vec3 normal(attrib.normals[3 * index.normal_index + 0],
                                attrib.normals[3 * index.normal_index + 1],
                                attrib.normals[3 * index.normal_index + 2]);
            glm::vec3 vertice(attrib.vertices[3 * index.vertex_index + 0], 
                                attrib.vertices[3 * index.vertex_index + 1],
                                attrib.vertices[3 * index.vertex_index + 2]);
            glm::vec2 uv;
            if (index.texcoord_index < 0)
                uv = glm::vec2(0.0, 0.0);
            else
                uv = glm::vec2(attrib.texcoords[2 * index.texcoord_index + 0], 
                                attrib.texcoords[2 * index.texcoord_index + 1]);

            vertices.push_back(vertice);
            normals.push_back(normal);
            uvs.push_back(uv);
        }
    }

    return true;
}

#ifdef USE_ASSIMP // don't use this #define, it's only for me (it AssImp fails to compile on your machine, at least all the other tutorials still work)

// Include AssImp
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

bool loadAssImp(
    const char* path,
    std::vector<unsigned short>& indices,
    std::vector<glm::vec3>& vertices,
    std::vector<glm::vec2>& uvs,
    std::vector<glm::vec3>& normals
) {

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path, 0/*aiProcess_JoinIdenticalVertices | aiProcess_SortByPType*/);
    if (!scene) {
        fprintf(stderr, importer.GetErrorString());
        getchar();
        return false;
    }
    const aiMesh* mesh = scene->mMeshes[0]; // In this simple example code we always use the 1rst mesh (in OBJ files there is often only one anyway)

    // Fill vertices positions
    vertices.reserve(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        aiVector3D pPosition = mesh->mVertices[i];
        vertices.push_back(glm::vec3(pPosition.x, pPosition.y, pPosition.z));
    }

    // Fill vertices texture coordinates
    uvs.reserve(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        aiVector3D UVW = mesh->mTextureCoords[0][i]; // Assume only 1 set of UV coords; AssImp supports 8 UV sets.
        uvs.push_back(glm::vec2(UVW.x, UVW.y));
    }

    // Fill vertices normals
    normals.reserve(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        aiVector3D n = mesh->mNormals[i];
        normals.push_back(glm::vec3(n.x, n.y, n.z));
    }


    // Fill face indices
    indices.reserve(3 * mesh->mNumFaces);
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        // Assume the model has only triangles.
        indices.push_back(mesh->mFaces[i].mIndices[0]);
        indices.push_back(mesh->mFaces[i].mIndices[1]);
        indices.push_back(mesh->mFaces[i].mIndices[2]);
    }

    // The "scene" pointer will be deleted automatically by "importer"

}

#endif