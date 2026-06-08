#pragma once
#include "CSceneBase.h"
#include "Scenes/FinalProject/FinalProject.h"
#include "Scenes/tde/Scene1/Scene1.h"
#include "Scenes/tde/Scene2/Scene2.h"
#include "Scenes/tde/Scene3/Scene3.h"
#include "Scenes/tde/Scene4/Scene4.h"

class CSceneManager : public CSceneBase
{
public:
	CSceneManager();
	~CSceneManager(void);
	void ChangeScene(unsigned int _uiCurrentScene);

	virtual void DrawScene();
	virtual void ProcessSceneInput(class GLFWwindow* window, float deltaTime);

private:
	// Ponteiro para cenas do jogo
	CSceneBase* Scene;

	// ID da cena
	unsigned int uiCurrentScene;

	bool bWireframeMode;
	bool bVsyncEnabled;
};
