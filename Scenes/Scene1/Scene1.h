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

		void CreateCube();
		void DestroyCube();
		void DrawCube();

		void CreatePyramid();
		void DestroyPyramid();
		void DrawPyramid();

		void ShowGUI();
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

	GLuint cubeVAO, cubeVBO;
	GLuint pyramidVAO, pyramidVBO;

	glm::vec3 objScale;

	float rotationAngle;
	glm::vec3 rotationAxis;
	int rotAxisMode;
	float rotSpeed;

	glm::vec4 objectColor;

	bool enableRotation;

	float moveX, moveY, moveZ;

	// Cursor state
	bool bCursorDisabled;


	int tileS, tileT;
	bool isTile;
};

