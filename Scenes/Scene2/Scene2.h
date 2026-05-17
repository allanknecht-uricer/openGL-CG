#pragma once
#include "Engine/CSceneBase.h"
#include "Engine/CCamera.h"
#include "Engine/CGridAxis.h"
#include "Engine/CShader.h"
#include "Engine/CTextures.h"
#include "Engine/CTimer.h"
#include "Engine/CText.h"



class Scene2 : public CSceneBase
{
public:
		Scene2();
		~Scene2();

		virtual void DrawScene();
        virtual void ProcessSceneInput(GLFWwindow* window, float deltaTime);

		void CreatePlane();
		void DestroyPlane();
		void DrawPlane();

private:

	// Camera
	CCamera* pCamera;

	// Axis
	CGridAxis* pGridAxis;

	// Shader
	CShader* pShader;

	// Textures
	CTextures* pTextures;

	// Timer
	CTimer* pTimer;			

	// Texto
	CText* pText;			

	// Plane
	GLuint planeVAO, planeVBO;

	GLuint tileS0, tileT0;
	GLuint tileS1, tileT1;
};

