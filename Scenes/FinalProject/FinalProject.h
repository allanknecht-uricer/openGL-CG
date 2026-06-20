#pragma once
#include "Engine/CSceneBase.h"
#include "Engine/CCamera.h"
#include "Engine/CGridAxis.h"
#include "Engine/CShader.h"
#include "Engine/CTimer.h"
#include "Engine/CText.h"
#include "Engine/CTextures.h"
#include "Engine/CLoadAssets.h"

class FinalProject : public CSceneBase
{
public:
	FinalProject();
	~FinalProject();

	virtual void DrawScene();
	virtual void ProcessSceneInput(GLFWwindow* window, float deltaTime);

private:
	CCamera* pCamera;
	CGridAxis* pGridAxis;
	CShader* pShader;
	CTimer* pTimer;
	CText* pText;
	CTextures* pTextures;
	CLoadAssets* pModel;
	CLoadAssets* pDetalheParede;
};
