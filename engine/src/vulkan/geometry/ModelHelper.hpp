#pragma once

#include <pch.hpp>
#include <vulkan/geometry/Vertex.hpp>

// Helper class for loading model files
class cModelHelper
{
public:
    // Load the vertices from a model file and optimize it into vertices and indices
    static void LoadModel(const char* sFilePath, std::vector<Vertex>& atVertices, std::vector<uint>& auiIndices);
};

void cModelHelper::LoadModel(const char* sFilePath, std::vector<Vertex>& atVertices, std::vector<uint>& auiIndices)
{
    assert(sFilePath != nullptr);   // path should not be null
    assert(atVertices.empty());     // vertices should be empty
    assert(auiIndices.empty());     // indices should be empty

    // Struct for storing the vertex attributes
    tinyobj::attrib_t tAttrib;

    // Structs for storing the shapes
    std::vector<tinyobj::shape_t> atShapes;

    // Structs for storing the materials
    std::vector<tinyobj::material_t> atMaterials;

    // String for printing error messages
    std::string sError;

    // Load the file using tinyobj
    if (!tinyobj::LoadObj(&tAttrib, &atShapes, &atMaterials, &sError, sFilePath, 0, true))
    {
        throw std::runtime_error(sError);
    }

    assert(tAttrib.vertices.size() > 0);    // the model must contain vertices
    assert(atShapes.size() > 0);            // the model most contain at least one shape

    // Map for all unique vertices, so we can use vertices in more than one triangle
    std::unordered_map<Vertex, uint> mUniqueVertices = {};

    for (const auto& tShape : atShapes)
    {
        for (const auto& index : tShape.mesh.indices)
        {
            Vertex tVertex = {};

            // Load the vertex position
            tVertex.pos = {
                    tAttrib.vertices[3 * index.vertex_index + 0],
                    tAttrib.vertices[3 * index.vertex_index + 1],
                    tAttrib.vertices[3 * index.vertex_index + 2]
            };

            // Load the vertex normal if it exists in the model
            if (index.normal_index >= 0)
            {
                tVertex.normal = {
                        tAttrib.normals[3 * index.normal_index + 0],
                        tAttrib.normals[3 * index.normal_index + 1],
                        tAttrib.normals[3 * index.normal_index + 2]
                };
            }
            else
            {
                tVertex.normal = {0, 0, 0};
                ENGINE_WARN("Model file " << sFilePath << " is missing a vertex normal!");
            }

            // Load the texture coordinates
            tVertex.texCoord = {
                    tAttrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - tAttrib.texcoords[2 * index.texcoord_index + 1]
            };

            // Load the vertex color
            tVertex.color = {1.0f, 1.0f, 1.0f};

            // Check if this vertex has not been added yet
            if (mUniqueVertices.count(tVertex) == 0)
            {
                size_t uiVertexCount = atVertices.size();
                assert(uiVertexCount <= UINT32_MAX); // we cannot have more than UINT32_MAX vertices

                // Add the vertex
                mUniqueVertices[tVertex] = (uint)uiVertexCount;
                atVertices.push_back(tVertex);

            }

            // Find and add the index of the vertex
            auiIndices.push_back(mUniqueVertices[tVertex]);
        }
    }
}
