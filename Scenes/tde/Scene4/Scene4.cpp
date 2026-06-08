#include "Scene4.h"
#include <cstdio>
#include <fstream>

namespace
{
	bool IsBlankLine(const std::string& line)
	{
		for (char c : line)
		{
			if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
				return false;
		}
		return true;
	}
}

Scene4::Scene4()
	: houseVAO(0)
	, houseVBO(0)
	, houseVertexCount(0)
	, floorVAO(0)
	, floorVBO(0)
{
	pCamera = new CCamera(
		glm::vec3(-17.7f, 8.1f, 21.6f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		-48.7f,
		-6.9f);
	pCamera->MovementSpeed = 100.0f;

	pTimer = new CTimer();
	pTimer->Init();

	pText = new CText();
	pText->InitTextManager();

	pShader = new CShader();
	pShader->LoadShader("Grid", "Scenes/Common/grid.vert", "Scenes/Common/grid.frag");
	pShader->LoadShader("Axis", "Scenes/Common/axis.vert", "Scenes/Common/axis.frag");
	pShader->LoadShader("Text2D", "Scenes/Common/Text2D.vert", "Scenes/Common/Text2D.frag");
	pShader->LoadShader("Textured", "Scenes/tde/Scene1/Textured.vert", "Scenes/tde/Scene1/Textured.frag");

	pGridAxis = new CGridAxis(50.0f);

	pTextures = new CTextures();
	pTextures->LoadTextureAnisotropic(0, "Scenes/tde/Scene4/grama.bmp");
	pTextures->LoadTextureClamp(1, "Scenes/tde/Scene4/CasaTex.jpg");

	if (!LoadHouseFromFile("Scenes/tde/Scene4/Casa.txt"))
		std::cerr << "Scene4: falha ao carregar Scenes/tde/Scene4/Casa.txt" << std::endl;

	CreateHouseBuffers();
	CreateFloor();
}

Scene4::~Scene4()
{
	DestroyHouse();
	DestroyFloor();

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
	if (pTextures)
	{
		delete pTextures;
		pTextures = NULL;
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

bool Scene4::LoadHouseFromFile(const char* filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
		return false;

	houseVertices.clear();

	std::string line;
	float position[3] = {};
	bool expectingUv = false;

	while (std::getline(file, line))
	{
		if (IsBlankLine(line))
			continue;

		if (!expectingUv)
		{
			if (std::sscanf(line.c_str(), "%f , %f , %f", &position[0], &position[1], &position[2]) != 3)
				return false;
			expectingUv = true;
		}
		else
		{
			float uv[2] = {};
			if (std::sscanf(line.c_str(), "%f , %f", &uv[0], &uv[1]) != 2)
				return false;

			houseVertices.push_back(position[0]);
			houseVertices.push_back(position[1]);
			houseVertices.push_back(position[2]);
			houseVertices.push_back(uv[0]);
			houseVertices.push_back(uv[1]);

			expectingUv = false;
		}
	}

	return !houseVertices.empty() && !expectingUv;
}

void Scene4::CreateHouseBuffers()
{
	if (houseVertices.empty())
		return;

	houseVertexCount = static_cast<GLsizei>(houseVertices.size() / 5);

	glGenVertexArrays(1, &houseVAO);
	glGenBuffers(1, &houseVBO);
	glBindVertexArray(houseVAO);
	glBindBuffer(GL_ARRAY_BUFFER, houseVBO);
	glBufferData(
		GL_ARRAY_BUFFER,
		static_cast<GLsizeiptr>(houseVertices.size() * sizeof(float)),
		houseVertices.data(),
		GL_STATIC_DRAW);

	const GLsizei stride = static_cast<GLsizei>(5 * sizeof(float));
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}

void Scene4::DestroyHouse()
{
	if (houseVAO != 0)
	{
		glDeleteVertexArrays(1, &houseVAO);
		houseVAO = 0;
	}
	if (houseVBO != 0)
	{
		glDeleteBuffers(1, &houseVBO);
		houseVBO = 0;
	}
}

void Scene4::DrawHouse()
{
	if (houseVAO == 0)
		return;

	glBindVertexArray(houseVAO);
	glDrawArrays(GL_TRIANGLES, 0, houseVertexCount);
	glBindVertexArray(0);
}

void Scene4::CreateFloor()
{
	const float halfSize = 30.0f;
	float v[] = {
		-halfSize, 0.0f,  halfSize, 0.0f, 0.0f,
		 halfSize, 0.0f,  halfSize, 1.0f, 0.0f,
		 halfSize, 0.0f, -halfSize, 1.0f, 1.0f,

		 halfSize, 0.0f, -halfSize, 1.0f, 1.0f,
		-halfSize, 0.0f, -halfSize, 0.0f, 1.0f,
		-halfSize, 0.0f,  halfSize, 0.0f, 0.0f,
	};

	glGenVertexArrays(1, &floorVAO);
	glGenBuffers(1, &floorVBO);
	glBindVertexArray(floorVAO);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);

	const GLsizei stride = static_cast<GLsizei>(5 * sizeof(float));
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}

void Scene4::DestroyFloor()
{
	glDeleteVertexArrays(1, &floorVAO);
	glDeleteBuffers(1, &floorVBO);
}

void Scene4::DrawFloor()
{
	glBindVertexArray(floorVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Scene4::DrawScene()
{
	pTimer->Update();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection = glm::perspective(glm::radians(pCamera->Zoom), ASPECT_RATIO, Z_NEAR, Z_FAR);
	glm::mat4 view = pCamera->GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);

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

	pShader->Use("Textured");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);
	pShader->SetMat4("model", model);
	pShader->SetVec4("objColor", 1.0f, 1.0f, 1.0f, 1.0f);
	pShader->SetFloat("tileS", 8.0f);
	pShader->SetFloat("tileT", 8.0f);
	pShader->SetBool("isTile", true);
	pTextures->ApplyTexture(0);
	glActiveTexture(GL_TEXTURE0);
	pShader->SetInt("Texture0", 0);
	DrawFloor();

	pShader->Use("Textured");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);
	pShader->SetMat4("model", model);
	pShader->SetVec4("objColor", 1.0f, 1.0f, 1.0f, 1.0f);
	pShader->SetFloat("tileS", 1.0f);
	pShader->SetFloat("tileT", 1.0f);
	pShader->SetBool("isTile", false);
	pTextures->ApplyTexture(1);
	glActiveTexture(GL_TEXTURE0);
	pShader->SetInt("Texture0", 0);
	DrawHouse();

	glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));

	pShader->Use("Text2D");
	pShader->SetMat4("projection", projection);
	pShader->SetVec3("textColor", 1.0f, 1.0f, 0.0f);

	pText->RenderText("Scene 6 — Casa (Exer4) | tecla 6", 5.0f, 140.0f, 0.35f);
	pText->RenderText("Teclas 0-6 trocam de cena | TAB = wireframe", 5.0f, 120.0f, 0.3f);
	pText->RenderText(std::format("Vertices: {0}", houseVertexCount), 5.0f, 100.0f, 0.3f);
	pText->RenderText(std::format("CamPosition\tx: {0:.2f} \t y: {1:.2f} \t z: {2:.2f}", pCamera->Position.x, pCamera->Position.y, pCamera->Position.z), 5.0f, 80.0f, 0.3f);
	pText->RenderText(std::format("CamForward\tx: {0:.2f} \t y: {1:.2f} \t z: {2:.2f}", pCamera->Front.x, pCamera->Front.y, pCamera->Front.z), 5.0f, 60.0f, 0.3f);
	pText->RenderText(std::format("FPS: {0:.0f}\tDeltatime: {1:.2f}ms\tTimer: {2:.2f}s", pTimer->GetFPS(), pTimer->GetDeltaTime(), pTimer->GetTime() / 1000), 5.0f, 10.0f, 0.3f);
	glEnable(GL_DEPTH_TEST);
}

void Scene4::ProcessSceneInput(GLFWwindow* window, float deltaTime)
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
