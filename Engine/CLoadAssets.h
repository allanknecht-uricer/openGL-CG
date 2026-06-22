#pragma once
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <GL/glew.h>
#include <glm/glm.hpp>


#include "stb_image.h"

struct Vertex 
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
    glm::vec3 Color = glm::vec3(1.0f);
};

struct Texture 
{
    unsigned int id = 0;
    std::string type;
    std::string path;
};

class Mesh 
{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    glm::vec4 diffuseColor;
    bool usePolygonOffset;
    bool isMetallic;
    bool useVertexColor;
    bool isNgonPanel;
    float specularScale;
    unsigned int VAO;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
        std::vector<Texture> textures, glm::vec4 diffuseColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f),
        bool usePolygonOffset = false, bool isMetallic = false, bool useVertexColor = false,
        bool isNgonPanel = false, float specularScale = 1.0f);
    void Draw(unsigned int shaderProgram);

private:
    unsigned int VBO, EBO;
    void setupMesh();
};

class CLoadAssets 
{
public:
    CLoadAssets(const std::string& path, const std::vector<std::string>& skipMeshNames = {});
    ~CLoadAssets();

    void Draw(unsigned int shaderProgram);

    const std::vector<glm::vec3>& GetNgonLightPositions() const { return ngonLightPositions; }

private:
    std::vector<Mesh> meshes;
    std::vector<glm::vec3> ngonLightPositions;
    std::string directory;
    std::string texturesDirectory;
    std::vector<std::string> skipMeshNames;

    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    unsigned int TextureFromFile(const char* path, const std::string& directory, const std::string& fallbackDirectory = {});
};

