#pragma once
#include "Engine/CSceneBase.h"
#include "Engine/CCamera.h"
#include "Engine/CGridAxis.h"
#include "Engine/CShader.h"
#include "Engine/CTimer.h"
#include "Engine/CText.h"
#include "Engine/CPrimitives.h"
#include <vector>
#include <cstdlib>
#include <ctime>

class Scene3 : public CSceneBase
{
public:
	Scene3();
	~Scene3();

	virtual void DrawScene();
	virtual void ProcessSceneInput(GLFWwindow* window, float deltaTime);

private:
	struct CuboEmpilhado
	{
		int pilha;
		glm::vec3 cor;
	};

	static const int NUM_PILHAS = 5;
	static const int MAX_CUBOS_POR_PILHA = 10;
	static const float TAMANHO_CUBO;
	static const float INTERVALO_SEG;

	void InitCube();
	void CleanupCube();

	void UpdateSimulation(float tempoAtual);
	void InserirCuboAleatorio();
	int ContarCubosNaPilha(int pilha) const;
	bool TodasPilhasCheias() const;
	int EscolherPilhaDisponivel();
	void ReiniciarSimulacao();
	glm::vec3 CorAleatoria();
	void DrawCubes(const glm::mat4& projection, const glm::mat4& view);

	CCamera* pCamera;
	CGridAxis* pGridAxis;
	CShader* pShader;
	CTimer* pTimer;
	CText* pText;

	CPrimitives cube;

	std::vector<CuboEmpilhado> cubos;
	float ultimoTempo;
};
