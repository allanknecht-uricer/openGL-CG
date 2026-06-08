#include "Scene3.h"

const float Scene3::TAMANHO_CUBO = 1.0f;
const float Scene3::INTERVALO_SEG = 1.0f;

Scene3::Scene3()
	: ultimoTempo(0.0f)
{
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	pCamera = new CCamera(glm::vec3(0.0f, 9.0f, 14.0f));
	pCamera->MovementSpeed = 100.0f;

	pTimer = new CTimer();
	pTimer->Init();

	pText = new CText();
	pText->InitTextManager();

	pShader = new CShader();
	pShader->LoadShader("Grid", "Scenes/Common/grid.vert", "Scenes/Common/grid.frag");
	pShader->LoadShader("Axis", "Scenes/Common/axis.vert", "Scenes/Common/axis.frag");
	pShader->LoadShader("Text2D", "Scenes/Common/Text2D.vert", "Scenes/Common/Text2D.frag");
	pShader->LoadShader("Scene3", "Scenes/tde/Scene3/Scene3.vert", "Scenes/tde/Scene3/Scene3.frag");

	pGridAxis = new CGridAxis(50.0f);

	InitCube();
}

Scene3::~Scene3()
{
	CleanupCube();

	if (pCamera)
	{
		delete pCamera;
		pCamera = NULL;
	}
	if (pShader)
	{
		delete pShader;
		pShader = NULL;
	}
	if (pGridAxis)
	{
		delete pGridAxis;
		pGridAxis = NULL;
	}
	if (pTimer)
	{
		delete pTimer;
		pTimer = NULL;
	}
	if (pText)
	{
		delete pText;
		pText = NULL;
	}
}

void Scene3::DrawScene()
{
	pTimer->Update();

	const float tempoAtual = pTimer->GetTime() / 1000.0f;
	UpdateSimulation(tempoAtual);

	glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection = glm::perspective(glm::radians(pCamera->Zoom), ASPECT_RATIO, Z_NEAR, Z_FAR);
	glm::mat4 view = pCamera->GetViewMatrix();

	pShader->Use("Grid");
	pShader->SetMat4("uProj", projection);
	pShader->SetMat4("uView", view);
	pShader->SetFloat("uGridSpacing", 1.0f);
	pShader->SetFloat("uFadeStart", 18.f);
	pShader->SetFloat("uFadeEnd", 100.f);
	pShader->SetVec3("uCamPos", pCamera->Position);
	pGridAxis->DrawGrid();

	pShader->Use("Axis");
	pShader->SetMat4("uProj", projection);
	pShader->SetMat4("uView", view);
	pGridAxis->DrawAxis();

	DrawCubes(projection, view);

	glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));

	pShader->Use("Text2D");
	pShader->SetMat4("projection", projection);
	pShader->SetVec3("textColor", 1.0f, 1.0f, 1.0f);

	int totalCubos = static_cast<int>(cubos.size());
	pText->RenderText("Scene 5 — Empilhamento de cubos (tecla 5)", 5.0f, 140.0f, 0.35f);
	pText->RenderText("Teclas 0-5 trocam de cena | 1 cubo/s | 5 pilhas x 10 cubos", 5.0f, 120.0f, 0.3f);
	pText->RenderText(std::format("Cubos: {0}/50", totalCubos), 5.0f, 100.0f, 0.3f);
	pText->RenderText(std::format("CamPosition\tx: {0:.2f} \t y: {1:.2f} \t z: {2:.2f}", pCamera->Position.x, pCamera->Position.y, pCamera->Position.z), 5.0f, 80.0f, 0.3f);
	pText->RenderText(std::format("FPS: {0:.0f}\tDeltatime: {1:.2f}ms\tTimer: {2:.2f}s", pTimer->GetFPS(), pTimer->GetDeltaTime(), tempoAtual), 5.0f, 10.0f, 0.3f);
	glEnable(GL_DEPTH_TEST);
}

void Scene3::ProcessSceneInput(GLFWwindow* window, float deltaTime)
{
	(void)window;
	InputManager& input = InputManager::GetInstance();

	if (input.IsKeyPressed(GLFW_KEY_W))
		pCamera->ProcessKeyboard(FORWARD, deltaTime);
	if (input.IsKeyPressed(GLFW_KEY_S))
		pCamera->ProcessKeyboard(BACKWARD, deltaTime);
	if (input.IsKeyPressed(GLFW_KEY_A))
		pCamera->ProcessKeyboard(LEFT, deltaTime);
	if (input.IsKeyPressed(GLFW_KEY_D))
		pCamera->ProcessKeyboard(RIGHT, deltaTime);
	if (input.IsKeyPressed(GLFW_KEY_E))
		pCamera->ProcessKeyboard(UP, deltaTime);
	if (input.IsKeyPressed(GLFW_KEY_Q))
		pCamera->ProcessKeyboard(DOWN, deltaTime);

	if (input.IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
		pCamera->MovementSpeed = SPEED_MULTIPLIER;
	else
		pCamera->MovementSpeed = SPEED;

	glm::vec2 mouseDelta = input.GetMouseDelta();
	if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f)
		pCamera->ProcessMouseMovement(mouseDelta.x, mouseDelta.y);

	float scrollOffset = input.GetScrollOffset();
	if (scrollOffset != 0.0f)
		pCamera->ProcessMouseScroll(scrollOffset);
}

void Scene3::UpdateSimulation(float tempoAtual)
{
	if (tempoAtual - ultimoTempo < INTERVALO_SEG)
		return;

	ultimoTempo = tempoAtual;
	InserirCuboAleatorio();
}

void Scene3::InserirCuboAleatorio()
{
	if (TodasPilhasCheias())
	{
		ReiniciarSimulacao();
		return;
	}

	const int pilha = EscolherPilhaDisponivel();

	CuboEmpilhado novo;
	novo.pilha = pilha;
	novo.cor = CorAleatoria();
	cubos.push_back(novo);
}

int Scene3::ContarCubosNaPilha(int pilha) const
{
	int count = 0;
	for (const CuboEmpilhado& cubo : cubos)
	{
		if (cubo.pilha == pilha)
			++count;
	}
	return count;
}

bool Scene3::TodasPilhasCheias() const
{
	for (int i = 0; i < NUM_PILHAS; ++i)
	{
		if (ContarCubosNaPilha(i) < MAX_CUBOS_POR_PILHA)
			return false;
	}
	return true;
}

int Scene3::EscolherPilhaDisponivel()
{
	std::vector<int> disponiveis;

	for (int i = 0; i < NUM_PILHAS; ++i)
	{
		if (ContarCubosNaPilha(i) < MAX_CUBOS_POR_PILHA)
			disponiveis.push_back(i);
	}

	return disponiveis[std::rand() % disponiveis.size()];
}

void Scene3::ReiniciarSimulacao()
{
	cubos.clear();
}

glm::vec3 Scene3::CorAleatoria()
{
	int sorteioR = std::rand() % 256;
	int sorteioG = std::rand() % 256;
	int sorteioB = std::rand() % 256;

	float r = sorteioR / 255.0f;
	float g = sorteioG / 255.0f;
	float b = sorteioB / 255.0f;

	// return já impede que a cor seja escura demais
	return glm::vec3(0.2f + 0.8f * r, 0.2f + 0.8f * g, 0.2f + 0.8f * b);
}

void Scene3::DrawCubes(const glm::mat4& projection, const glm::mat4& view)
{
	pShader->Use("Scene3");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);

	for (size_t i = 0; i < cubos.size(); ++i)
	{
		const CuboEmpilhado& cubo = cubos[i];

		int nivel = 0;
		for (size_t j = 0; j < i; ++j)
		{
			if (cubos[j].pilha == cubo.pilha)
				++nivel;
		}

		const float x = static_cast<float>(cubo.pilha - 2) * TAMANHO_CUBO;
		const float y = static_cast<float>(nivel) * TAMANHO_CUBO + TAMANHO_CUBO * 0.5f;

		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));

		pShader->SetMat4("model", model);
		pShader->SetVec3("objColor", cubo.cor.r, cubo.cor.g, cubo.cor.b);
		cube.render();
	}
}

void Scene3::InitCube()
{
	cube.generateCube(1.0f);
	cube.setupBuffers();
}

void Scene3::CleanupCube()
{
	cube.cleanup();
}
