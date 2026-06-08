#pragma once
#include "Engine/CSceneBase.h"
#include "Engine/CCamera.h"
#include "Engine/CGridAxis.h"
#include "Engine/CShader.h"
#include "Engine/CTimer.h"
#include "Engine/CText.h"
#include "Engine/CPrimitives.h"

class Scene2 : public CSceneBase
{
public:
	Scene2();
	~Scene2();

	virtual void DrawScene();
	virtual void ProcessSceneInput(GLFWwindow* window, float deltaTime);

	void CreateFloor();
	void DestroyFloor();
	void DrawFloor();

	void InitTree();
	void CleanupTree();
	void DrawTreeTrunk();
	void DrawTreeCone();

	void InitSnowman();
	void CleanupSnowman();
	void DrawSnowSphere();
	void DrawHatBrim();
	void DrawHatCrown();
	void DrawCarrotNose();

private:
	CCamera* pCamera;
	CGridAxis* pGridAxis;
	CShader* pShader;
	CTimer* pTimer;
	CText* pText;

	GLuint floorVAO;
	GLuint floorVBO;

	CPrimitives treeTrunk;
	CPrimitives treeCone;
	CPrimitives snowSphere;
	CPrimitives hatBrim;
	CPrimitives hatCrown;
	CPrimitives carrotNose;
};
