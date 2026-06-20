#pragma warning(disable : 4267)
#define STB_IMAGE_IMPLEMENTATION 

#include "CLoadAssets.h"
#include <iostream>
#include <cstddef>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>

// Mesh Implementation
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
    std::vector<Texture> textures, glm::vec4 diffuseColor)
    : vertices(std::move(vertices)), indices(std::move(indices)), textures(std::move(textures)),
      diffuseColor(diffuseColor), VAO(0)
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

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

    glBindVertexArray(0);
}

void Mesh::Draw(unsigned int shaderProgram) 
{
    glUniform4fv(glGetUniformLocation(shaderProgram, "objColor"), 1, glm::value_ptr(diffuseColor));

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
}

CLoadAssets::CLoadAssets(const std::string& path, const std::vector<std::string>& skipMeshNames)
    : skipMeshNames(skipMeshNames)
{
    loadModel(path);
}

CLoadAssets::~CLoadAssets()
{
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
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        else 
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

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
    if (meshName == "floor")
    {
        textures.clear();
        Texture floorTex;
        floorTex.id = TextureFromFile("textures/floor.png", directory);
        floorTex.type = "uDiffuseMap";
        floorTex.path = "textures/floor.png";
        if (floorTex.id != 0)
            textures.push_back(floorTex);
    }

    return Mesh(vertices, indices, textures, diffuseColor);
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
    const bool isAbsolute = (filename.size() > 1 && filename[1] == ':')
        || (!filename.empty() && filename[0] == '/');
    if (!isAbsolute)
        filename = directory + '/' + filename;

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

        GLfloat maxAnisotropy = 0.0f;
        if (glewIsSupported("GL_EXT_texture_filter_anisotropic"))
        {
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
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
