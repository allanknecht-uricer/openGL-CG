#pragma once
#include "Engine/CSceneBase.h"
#include "Engine/CCamera.h"
#include "Engine/CGridAxis.h"
#include "Engine/CShader.h"
#include "Engine/CTextures.h"
#include "Engine/CTimer.h"
#include "Engine/CText.h"



class Scene0 : public CSceneBase
{
public:
		Scene0();
		~Scene0();

		virtual void DrawScene();
        virtual void ProcessSceneInput(GLFWwindow* window, float deltaTime);

		void CreatePyramid();
		void DestroyPyramid();
		void DrawPyramid();

		void CreateCube();
		void DestroyCube();
		void DrawCube();

		void CreateTriangle();
		void DestroyTriangle();
		void DrawTriangle();



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

	GLuint pyramidVAO, pyramidVBO;
	GLuint cubeVAO, cubeVBO;
	GLuint triangleVAO, trianglePositionVBO, triangleColorVBO;

	float speed;
};

