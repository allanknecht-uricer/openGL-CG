#pragma warning(disable : 4267)

#include "CLoadAssets.h"
#include <iostream>
#include <cstddef>
#include <utility>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>

namespace
{
    bool meshNeedsFlatShading(const std::string& meshName)
    {
        return meshName.find("detalhe_parede") != std::string::npos;
    }

    void rebuildFlatShaded(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
    {
        std::vector<Vertex> flatVertices;
        std::vector<unsigned int> flatIndices;
        flatVertices.reserve(indices.size());
        flatIndices.reserve(indices.size());

        for (size_t i = 0; i + 2 < indices.size(); i += 3)
        {
            Vertex v0 = vertices[indices[i]];
            Vertex v1 = vertices[indices[i + 1]];
            Vertex v2 = vertices[indices[i + 2]];

            const glm::vec3 edge1 = v1.Position - v0.Position;
            const glm::vec3 edge2 = v2.Position - v0.Position;
            glm::vec3 faceNormal = glm::cross(edge1, edge2);
            if (glm::dot(faceNormal, faceNormal) > 0.000001f)
                faceNormal = glm::normalize(faceNormal);
            else
                faceNormal = glm::vec3(0.0f, 1.0f, 0.0f);

            v0.Normal = faceNormal;
            v1.Normal = faceNormal;
            v2.Normal = faceNormal;

            const unsigned int base = static_cast<unsigned int>(flatVertices.size());
            flatVertices.push_back(v0);
            flatVertices.push_back(v1);
            flatVertices.push_back(v2);
            flatIndices.push_back(base);
            flatIndices.push_back(base + 1);
            flatIndices.push_back(base + 2);
        }

        vertices = std::move(flatVertices);
        indices = std::move(flatIndices);
    }
}

// Mesh Implementation
Mesh::Mesh(std::vector<Vertex> inVertices, std::vector<unsigned int> inIndices,
    std::vector<Texture> inTextures, glm::vec4 inDiffuseColor, bool inUsePolygonOffset)
    : vertices(std::move(inVertices)), indices(std::move(inIndices)), textures(std::move(inTextures)),
      diffuseColor(inDiffuseColor), usePolygonOffset(inUsePolygonOffset), VAO(0)
{
    setupMesh();
}

void Mesh::setupMesh() 
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    // Texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	// Tangents
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

	// Bitangents
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    

    glBindVertexArray(0);
}

void Mesh::Draw(unsigned int shaderProgram) 
{
    glUniform4fv(glGetUniformLocation(shaderProgram, "objColor"), 1, glm::value_ptr(diffuseColor));

    if (usePolygonOffset)
    {
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0f, 1.0f);
    }

    // Bind textures
    unsigned int diffuseNr = 1;
    unsigned int normalNr = 1;
    bool hasDiffuseMap = false;
    for (unsigned int i = 0; i < textures.size(); i++) 
    {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string name = textures[i].type;
        std::string number = (name == "uDiffuseMap") ? std::to_string(diffuseNr++) : std::to_string(normalNr++);
        (void)number;
        glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), i);
        if (name == "uDiffuseMap")
            hasDiffuseMap = true;
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glUniform1i(glGetUniformLocation(shaderProgram, "useMap"), hasDiffuseMap ? 1 : 0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    if (usePolygonOffset)
        glDisable(GL_POLYGON_OFFSET_FILL);
    
}

// CLoadAssets Implementation
CLoadAssets::CLoadAssets(const std::string& path, const std::vector<std::string>& skipMeshNames)
    : skipMeshNames(skipMeshNames)
{
    loadModel(path);
}

CLoadAssets::~CLoadAssets()
{
    // Cleanup textures
    for (auto& mesh : meshes) 
    {
        for (auto& texture : mesh.textures) 
        {
            glDeleteTextures(1, &texture.id);
        }
	}
}

void CLoadAssets::Draw(unsigned int shaderProgram) 
{
    for (auto& mesh : meshes)
        mesh.Draw(shaderProgram);
}

void CLoadAssets::loadModel(const std::string& path) 
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    const size_t lastSlash = path.find_last_of("/\\");
    directory = (lastSlash != std::string::npos) ? path.substr(0, lastSlash) : ".";
    processNode(scene->mRootNode, scene);
}

void CLoadAssets::processNode(aiNode* node, const aiScene* scene) 
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++) 
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        const std::string meshName = mesh->mName.C_Str();
        if (std::find(skipMeshNames.begin(), skipMeshNames.end(), meshName) != skipMeshNames.end())
            continue;
        meshes.push_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) 
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh CLoadAssets::processMesh(aiMesh* mesh, const aiScene* scene) 
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) 
    {
        Vertex vertex;
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        if (mesh->HasNormals())
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        else
            vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);

        if (mesh->mTextureCoords[0]) 
        {
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else 
        {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        if (mesh->HasTangentsAndBitangents())
        {
            vertex.Tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
            vertex.Bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
        }
        else
        {
            vertex.Tangent = glm::vec3(1.0f, 0.0f, 0.0f);
            vertex.Bitangent = glm::vec3(0.0f, 0.0f, 1.0f);
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) 
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    glm::vec4 diffuseColor(0.8f, 0.8f, 0.8f, 1.0f);
    if (mesh->mMaterialIndex >= 0) 
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        aiColor3D kd(0.8f, 0.8f, 0.8f);
        if (material->Get(AI_MATKEY_COLOR_DIFFUSE, kd) == AI_SUCCESS)
            diffuseColor = glm::vec4(kd.r, kd.g, kd.b, 1.0f);

        auto diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "uDiffuseMap");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        auto normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "uNormalMap");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        auto heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "uHeightMap");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    }

    const std::string meshName = mesh->mName.C_Str();
    const bool usePolygonOffset = meshNeedsFlatShading(meshName);
    if (usePolygonOffset)
        rebuildFlatShaded(vertices, indices);

    return Mesh(vertices, indices, textures, diffuseColor, usePolygonOffset);
}

std::vector<Texture> CLoadAssets::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) 
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) 
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        Texture texture;
        texture.id = TextureFromFile(str.C_Str(), directory);
        texture.type = typeName;
        texture.path = str.C_Str();
        textures.push_back(texture);
    }
    return textures;
}


unsigned int CLoadAssets::TextureFromFile(const char* path, const std::string& directory) 
{
    std::string filename = std::string(path);
    const bool isAbsolute = (filename.size() > 1 && filename[1] == ':') || (!filename.empty() && filename[0] == '/');
    if (!isAbsolute)
        filename = directory + '/' + filename;

    // Flip the image vertically during loading
    stbi_set_flip_vertically_on_load(false);

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data) 
    {
        GLenum format = GL_RGB;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Check if anisotropic filtering is supported and get the maximum anisotropy level
        GLfloat maxAnisotropy = 0.0f;
        if (glewIsSupported("GL_EXT_texture_filter_anisotropic"))
        {
            // Get the maximum anisotropy level supported by the GPU
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);

            // Set anisotropic filtering
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
        }

        stbi_image_free(data);
    }
    else 
    {
        std::cerr << "Texture failed to load at path: " << filename << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

