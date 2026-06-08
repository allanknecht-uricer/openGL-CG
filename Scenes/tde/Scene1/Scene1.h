#pragma once
#include "Engine/CSceneBase.h"
#include "Engine/CCamera.h"
#include "Engine/CGridAxis.h"
#include "Engine/CShader.h"
#include "Engine/CTextures.h"
#include "Engine/CTimer.h"
#include "Engine/CText.h"



class Scene1 : public CSceneBase
{
public:
		Scene1();
		~Scene1();

		virtual void DrawScene();
        virtual void ProcessSceneInput(GLFWwindow* window, float deltaTime);

		void CreateFloor();
		void DestroyFloor();
		void DrawFloor();

		void CreateCrate();
		void DestroyCrate();
		void DrawCrate();

		void CreateHouse();
		void DestroyHouse();
		void DrawHouse();

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

	GLuint floorVAO, floorVBO;
	GLuint crateVAO, crateVBO;
	GLuint houseVAO, houseVBO;
	GLsizei houseVertexCount;
};
