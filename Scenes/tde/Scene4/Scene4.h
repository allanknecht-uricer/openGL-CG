#pragma once
#include "Engine/CSceneBase.h"
#include "Engine/CCamera.h"
#include "Engine/CGridAxis.h"
#include "Engine/CShader.h"
#include "Engine/CTextures.h"
#include "Engine/CTimer.h"
#include "Engine/CText.h"
#include <vector>
#include <string>

class Scene4 : public CSceneBase
{
public:
	Scene4();
	~Scene4();

	virtual void DrawScene();
	virtual void ProcessSceneInput(GLFWwindow* window, float deltaTime);

private:
	bool LoadHouseFromFile(const char* filename);
	void CreateHouseBuffers();
	void DestroyHouse();
	void DrawHouse();

	void CreateFloor();
	void DestroyFloor();
	void DrawFloor();

	CCamera* pCamera;
	CGridAxis* pGridAxis;
	CShader* pShader;
	CTextures* pTextures;
	CTimer* pTimer;
	CText* pText;

	std::vector<float> houseVertices;
	GLuint houseVAO;
	GLuint houseVBO;
	GLsizei houseVertexCount;

	GLuint floorVAO;
	GLuint floorVBO;
};
