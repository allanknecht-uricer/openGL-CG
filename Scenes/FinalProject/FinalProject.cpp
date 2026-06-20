#include "FinalProject.h"

FinalProject::FinalProject()
	: pModel(nullptr)
	, pDetalheParede(nullptr)
{
	// Quarto exportado do 3ds Max em polegadas (~200 unidades) — camera afastada como Scene5
	pCamera = new CCamera(glm::vec3(80.0f, 100.0f, 120.0f));
	pCamera->MovementSpeed = 100.0f;

	pTimer = new CTimer();
	pTimer->Init();

	pText = new CText();
	pText->InitTextManager();

	pShader = new CShader();
	pShader->LoadShader("Grid", "Scenes/Common/grid.vert", "Scenes/Common/grid.frag");
	pShader->LoadShader("Axis", "Scenes/Common/axis.vert", "Scenes/Common/axis.frag");
	pShader->LoadShader("Text2D", "Scenes/Common/Text2D.vert", "Scenes/Common/Text2D.frag");
	pShader->LoadShader("Assets", "Scenes/FinalProject/Assets.vert", "Scenes/FinalProject/Assets.frag");

	pGridAxis = new CGridAxis(50.0f);

	pModel = new CLoadAssets("Scenes/FinalProject/project.obj", { "detalhe_parede" });
	pDetalheParede = new CLoadAssets("Scenes/FinalProject/detalhe_parede_fixed.obj");
}

FinalProject::~FinalProject()
{
	if (pDetalheParede)
	{
		delete pDetalheParede;
		pDetalheParede = NULL;
	}

	if (pModel)
	{
		delete pModel;
		pModel = NULL;
	}

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

void FinalProject::DrawScene()
{
	pTimer->Update();

	glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection = glm::perspective(glm::radians(pCamera->Zoom), ASPECT_RATIO, Z_NEAR, Z_FAR);
	glm::mat4 view = pCamera->GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);

	pShader->Use("Axis");
	pShader->SetMat4("uProj", projection);
	pShader->SetMat4("uView", view);
	pGridAxis->DrawAxis();

	if (pModel)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		pShader->Use("Assets");
		pShader->SetMat4("projection", projection);
		pShader->SetMat4("view", view);
		pShader->SetMat4("model", model);
		pModel->Draw(pShader->GetProgram("Assets"));

		if (pDetalheParede)
			pDetalheParede->Draw(pShader->GetProgram("Assets"));
	}

	glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));

	pShader->Use("Text2D");
	pShader->SetMat4("projection", projection);
	pShader->SetVec3("textColor", 1.0f, 1.0f, 1.0f);

	pText->RenderText("FinalProject — Quarto Gamer (tecla 0)", 5.0f, 120.0f, 0.35f);
	pText->RenderText("WASD + mouse | Shift = rapido | igual Scene5 (Assets + OBJ)", 5.0f, 100.0f, 0.28f);
	pText->RenderText(std::format("CamPosition\tx: {0:.2f} \t y: {1:.2f} \t z: {2:.2f}", pCamera->Position.x, pCamera->Position.y, pCamera->Position.z), 5.0f, 80.0f, 0.28f);
	pText->RenderText(std::format("FPS: {0:.0f}\tDeltatime: {1:.2f}ms", pTimer->GetFPS(), pTimer->GetDeltaTime()), 5.0f, 10.0f, 0.28f);
	glEnable(GL_DEPTH_TEST);
}

void FinalProject::ProcessSceneInput(GLFWwindow* window, float deltaTime)
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
		pCamera->MovementSpeed = 100.0f;

	glm::vec2 mouseDelta = input.GetMouseDelta();
	if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f)
		pCamera->ProcessMouseMovement(mouseDelta.x, mouseDelta.y);

	float scrollOffset = input.GetScrollOffset();
	if (scrollOffset != 0.0f)
		pCamera->ProcessMouseScroll(scrollOffset);
}
