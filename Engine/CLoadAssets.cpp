#pragma warning(disable : 4267)
#define STB_IMAGE_IMPLEMENTATION 

#include "CLoadAssets.h"
#include <iostream>
#include <cstddef>
#include <algorithm>
#include <map>
#include <glm/gtc/type_ptr.hpp>

namespace
{
    std::map<std::string, unsigned int> gTextureCache;

    bool HasDiffuseMap(const std::vector<Texture>& textures)
    {
        for (const auto& texture : textures)
        {
            if (texture.type == "uDiffuseMap" && texture.id != 0)
                return true;
        }
        return false;
    }
}

// Mesh Implementation
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
    std::vector<Texture> textures, glm::vec4 diffuseColor, bool usePolygonOffset, bool isMetallic, bool useVertexColor,
    bool isNgonPanel, float specularScale)
    : vertices(std::move(vertices)), indices(std::move(indices)), textures(std::move(textures)),
      diffuseColor(diffuseColor), usePolygonOffset(usePolygonOffset), isMetallic(isMetallic),
      useVertexColor(useVertexColor), isNgonPanel(isNgonPanel), specularScale(specularScale), VAO(0)
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

    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));

    glBindVertexArray(0);
}

void Mesh::Draw(unsigned int shaderProgram) 
{
    glUniform4fv(glGetUniformLocation(shaderProgram, "objColor"), 1, glm::value_ptr(diffuseColor));

    bool hasDiffuseMap = false;
    bool hasEmissiveMap = false;
    unsigned int unit = 0;
    for (unsigned int i = 0; i < textures.size(); i++) 
    {
        if (textures[i].id == 0)
            continue;

        glActiveTexture(GL_TEXTURE0 + unit);
        const std::string& name = textures[i].type;
        glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), static_cast<int>(unit));
        if (name == "uDiffuseMap")
            hasDiffuseMap = true;
        else if (name == "uEmissiveMap")
            hasEmissiveMap = true;
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
        ++unit;
    }

    glUniform1i(glGetUniformLocation(shaderProgram, "useMap"), hasDiffuseMap ? 1 : 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "useEmissive"), hasEmissiveMap ? 1 : 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "useVertexColor"), useVertexColor ? 1 : 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "uNgonPanel"), isNgonPanel ? 1 : 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "uMetallic"), isMetallic ? 1 : 0);
    glUniform1f(glGetUniformLocation(shaderProgram, "uSpecScale"), specularScale);

    if (usePolygonOffset)
    {
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-1.0f, -2.0f);
    }

    if (isNgonPanel)
        glDisable(GL_CULL_FACE);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    if (isNgonPanel)
        glEnable(GL_CULL_FACE);

    if (usePolygonOffset)
        glDisable(GL_POLYGON_OFFSET_FILL);
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
    texturesDirectory = directory + "/textures";
    if (directory.size() >= 7 && directory.compare(directory.size() - 6, 6, "models") == 0)
        texturesDirectory = directory.substr(0, directory.size() - 7) + "/textures";
    processNode(scene->mRootNode, scene);

    std::cout << "Loaded " << meshes.size() << " meshes from " << path;
    if (!ngonLightPositions.empty())
        std::cout << " (" << ngonLightPositions.size() << " ngon lights)";
    std::cout << std::endl;
    if (meshes.empty())
        std::cerr << "WARNING: no meshes in " << path << std::endl;

    std::sort(ngonLightPositions.begin(), ngonLightPositions.end(),
        [](const glm::vec3& a, const glm::vec3& b)
        {
            if (a.y != b.y)
                return a.y > b.y;
            return a.z < b.z;
        });
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

        if (mesh->HasVertexColors(0))
        {
            vertex.Color = glm::vec3(
                mesh->mColors[0][i].r,
                mesh->mColors[0][i].g,
                mesh->mColors[0][i].b);
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
    std::string matName;
    if (mesh->mMaterialIndex >= 0) 
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        aiString materialName;
        if (material->Get(AI_MATKEY_NAME, materialName) == AI_SUCCESS)
            matName = materialName.C_Str();

        aiColor3D kd(0.8f, 0.8f, 0.8f);
        if (material->Get(AI_MATKEY_COLOR_DIFFUSE, kd) == AI_SUCCESS)
            diffuseColor = glm::vec4(kd.r, kd.g, kd.b, 1.0f);

        auto diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "uDiffuseMap");
        if (diffuseMaps.empty())
            diffuseMaps = loadMaterialTextures(material, aiTextureType_BASE_COLOR, "uDiffuseMap");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        auto emissiveMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, "uEmissiveMap");
        if (emissiveMaps.empty())
            emissiveMaps = loadMaterialTextures(material, aiTextureType_EMISSION_COLOR, "uEmissiveMap");
        textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());

        auto normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "uNormalMap");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        auto heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "uHeightMap");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    }

    const std::string meshName = mesh->mName.C_Str();
    bool usePolygonOffset = false;
    bool isMetallic = false;
    bool isNgonPanel = (matName == "losangulos_parede");
    bool useVertexColor = (meshName == "PivotDoor001" || meshName == "TV") && mesh->HasVertexColors(0);
    float specularScale = 1.0f;

    if (isNgonPanel)
    {
        textures.clear();
        diffuseColor = glm::vec4(0.0012f, 0.0012f, 0.0012f, 1.0f);
    }

    auto assignDiffuseTexture = [&](const char* filename)
    {
        textures.erase(
            std::remove_if(textures.begin(), textures.end(),
                [](const Texture& t) { return t.type == "uDiffuseMap"; }),
            textures.end());

        Texture tex;
        tex.id = TextureFromFile(filename, texturesDirectory, texturesDirectory);
        tex.type = "uDiffuseMap";
        tex.path = filename;
        if (tex.id != 0)
            textures.push_back(tex);
    };

    auto assignEmissiveTexture = [&](const char* filename)
    {
        textures.erase(
            std::remove_if(textures.begin(), textures.end(),
                [](const Texture& t) { return t.type == "uEmissiveMap"; }),
            textures.end());

        Texture tex;
        tex.id = TextureFromFile(filename, texturesDirectory, texturesDirectory);
        tex.type = "uEmissiveMap";
        tex.path = filename;
        if (tex.id != 0)
            textures.push_back(tex);
    };

    if (matName == "floor_wood" || matName == "wall_cement" || matName == "sofa_fabric" || matName == "wall_brick"
        || matName == "detalhe_parede_black")
    {
        if (matName == "floor_wood")
            assignDiffuseTexture("floor.png");
        else if (matName == "wall_cement")
            assignDiffuseTexture("wall_cement.jpg");
        else if (matName == "sofa_fabric")
            assignDiffuseTexture("plastic_dark.png");
        else if (matName == "wall_brick")
            assignDiffuseTexture("wall_brick.png");
        else if (matName == "detalhe_parede_black")
        {
            assignDiffuseTexture("wall_slats.png");
            if (!HasDiffuseMap(textures))
                assignDiffuseTexture("plastic_dark.png");
        }

        if (matName == "sofa_fabric")
            diffuseColor = glm::vec4(0.55f, 0.55f, 0.58f, 1.0f);
        else if (matName == "detalhe_parede_black")
            diffuseColor = HasDiffuseMap(textures)
                ? glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
                : glm::vec4(0.098f, 0.098f, 0.098f, 1.0f);
        else
            diffuseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else if (matName == "Material_001")
    {
        textures.clear();
        assignDiffuseTexture("Material.001_Base_color.png");
        assignEmissiveTexture("Material.001_Emissive.png");
        diffuseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }

    if (!HasDiffuseMap(textures))
    {
        if (meshName == "floor" || meshName == "mesa" || meshName == "estante")
        {
            assignDiffuseTexture("floor.png");
            diffuseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }
        else if (meshName == "Box001")
        {
            assignDiffuseTexture("wall_cement.jpg");
            diffuseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }
        else if (meshName == "laptop")
        {
            textures.clear();
            assignDiffuseTexture("Material.001_Base_color.png");
            assignEmissiveTexture("Material.001_Emissive.png");
            diffuseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }
        else if (meshName == "sofa" || meshName.rfind("almofada", 0) == 0)
        {
            assignDiffuseTexture("plastic_dark.png");
            diffuseColor = glm::vec4(0.55f, 0.55f, 0.58f, 1.0f);
        }
        else if (meshName.find("lugar_onde_vai_textura_de_tijolo") != std::string::npos)
            assignDiffuseTexture("wall_brick.png");
        else if (meshName == "detalhe_parede")
        {
            assignDiffuseTexture("wall_slats.png");
            if (!HasDiffuseMap(textures))
                assignDiffuseTexture("plastic_dark.png");
            diffuseColor = HasDiffuseMap(textures)
                ? glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
                : glm::vec4(0.098f, 0.098f, 0.098f, 1.0f);
        }
    }

    if (meshName == "sofa" || meshName.rfind("almofada", 0) == 0 || meshName == "PivotDoor001")
        specularScale = 0.12f;
    else if (meshName == "detalhe_parede")
        specularScale = 0.12f;

    if (meshName == "PivotDoor001")
    {
        textures.clear();
        diffuseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else if (meshName == "TV")
    {
        textures.clear();
        diffuseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else if (meshName.rfind("NgonParede", 0) == 0 && diffuseColor.r > 0.1f && !isNgonPanel)
    {
        textures.clear();
        diffuseColor = glm::vec4(0.78f, 0.80f, 0.83f, 1.0f);
        isMetallic = true;
    }
    else if (meshName.find("lugar_onde_vai_textura_de_tijolo") != std::string::npos)
        usePolygonOffset = true;

    if (isNgonPanel && !vertices.empty())
    {
        glm::vec3 center(0.0f);
        glm::vec3 avgNormal(0.0f);
        for (const auto& vertex : vertices)
        {
            center += vertex.Position;
            avgNormal += vertex.Normal;
        }
        const float invCount = 1.0f / static_cast<float>(vertices.size());
        center *= invCount;
        avgNormal *= invCount;
        if (glm::dot(avgNormal, avgNormal) > 0.000001f)
            avgNormal = glm::normalize(avgNormal);
        else
            avgNormal = glm::vec3(0.0f, 0.0f, 1.0f);

        // Emit from in front of the panel so the wall around it receives light.
        ngonLightPositions.push_back(center + avgNormal * 22.0f);
    }

    return Mesh(vertices, indices, textures, diffuseColor, usePolygonOffset, isMetallic, useVertexColor, isNgonPanel, specularScale);
}

std::vector<Texture> CLoadAssets::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) 
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) 
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        Texture texture;
        texture.id = TextureFromFile(str.C_Str(), directory, texturesDirectory);
        texture.type = typeName;
        texture.path = str.C_Str();
        if (texture.id != 0)
            textures.push_back(texture);
    }
    return textures;
}

unsigned int CLoadAssets::TextureFromFile(const char* path, const std::string& directory, const std::string& fallbackDirectory)
{
    std::string filename = std::string(path);
    const bool isAbsolute = (filename.size() > 1 && filename[1] == ':')
        || (!filename.empty() && filename[0] == '/');

    std::vector<std::string> candidates;
    if (isAbsolute)
        candidates.push_back(filename);
    else
    {
        candidates.push_back(directory + '/' + filename);
        if (!fallbackDirectory.empty() && fallbackDirectory != directory)
            candidates.push_back(fallbackDirectory + '/' + filename);

        const size_t slash = filename.find_last_of("/\\");
        if (slash != std::string::npos)
        {
            const std::string basename = filename.substr(slash + 1);
            if (!fallbackDirectory.empty())
                candidates.push_back(fallbackDirectory + '/' + basename);
            candidates.push_back(directory + '/' + basename);
        }
    }

    for (const auto& candidate : candidates)
    {
        const auto cached = gTextureCache.find(candidate);
        if (cached != gTextureCache.end())
            return cached->second;
    }

    stbi_set_flip_vertically_on_load(false);

    unsigned char* data = nullptr;
    int width = 0, height = 0, nrComponents = 0;
    std::string loadedFrom;
    for (const auto& candidate : candidates)
    {
        data = stbi_load(candidate.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            loadedFrom = candidate;
            break;
        }
    }

    if (!data)
    {
        std::cerr << "Texture failed to load at paths:";
        for (const auto& candidate : candidates)
            std::cerr << " " << candidate;
        std::cerr << std::endl;
        return 0;
    }

    unsigned int textureID;
    glGenTextures(1, &textureID);

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
    gTextureCache[loadedFrom] = textureID;
    return textureID;
}
