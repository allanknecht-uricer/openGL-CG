#include "Scene3.h"

Scene3::Scene3()
{
	// Cria a c�mera
	pCamera = NULL;
	pCamera = new CCamera(glm::vec3(0.0f, 3.0f, 20.0f));
	pCamera->MovementSpeed = 100.0f;

	// Cria o Timer
	pTimer = NULL;
	pTimer = new CTimer();
	pTimer->Init();

	// Cria gerenciador de impress�o de texto na tela
	pText = NULL;
	pText = new CText();
	pText->InitTextManager();

	// Cria os shaders
	pShader = NULL;
	pShader = new CShader();
	pShader->LoadShader("Grid", "Scenes/Common/grid.vert", "Scenes/Common/grid.frag");
	pShader->LoadShader("Axis", "Scenes/Common/axis.vert", "Scenes/Common/axis.frag");
	pShader->LoadShader("Text2D", "Scenes/Common/Text2D.vert", "Scenes/Common/Text2D.frag");
	pShader->LoadShader("Textured", "Scenes/Scene3/Textured.vert", "Scenes/Scene3/Textured.frag");

	// Cria o gerenciador de grid e axis
	pGridAxis = NULL;
	pGridAxis = new CGridAxis(50.0f);

	pTextures = new CTextures();
	pTextures->LoadTextureAnisotropic(0, "Scenes/Scene3/Exer3/grama.bmp");
	pTextures->LoadTextureAnisotropic(1, "Scenes/Scene3/Exer3/CRATE.BMP");
	pTextures->LoadTextureClamp(2, "Scenes/Scene3/Exer3/House.jpg");

	CreateFloor();
	CreateCrate();
	CreateHouse();
}

Scene3::~Scene3()
{

	DestroyCrate();
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

void Scene3::DrawScene()
{
	// Update timer (must be called at start of frame)
	pTimer->Update();

	// Clear the color and depth buffers
	glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Create transformations 
	// Note: In a real application, you would typically calculate the projection and view matrices 
	// once per frame and pass them to your shaders
	glm::mat4 projection = glm::perspective(glm::radians(pCamera->Zoom), ASPECT_RATIO, Z_NEAR, Z_FAR);
	glm::mat4 view = pCamera->GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Draw the 3D grid
	pShader->Use("Grid");
	pShader->SetMat4("uProj", projection);
	pShader->SetMat4("uView", view);
	pShader->SetFloat("uGridSpacing", 1.0f);
	pShader->SetFloat("uFadeStart", 18.f);
	pShader->SetFloat("uFadeEnd", 100.f);
	pShader->SetVec3("uCamPos", pCamera->Position);
	pGridAxis->DrawGrid();
	// Draw the 3D axis
	pShader->Use("Axis");
	pShader->SetMat4("uProj", projection);
	pShader->SetMat4("uView", view);
	pGridAxis->DrawAxis();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                               DESENHA OS OBJETOS DA CENA (INICIO)
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	// Floor (tiles iguais ao default do Textured.vert para nao herdar estado da caixa)
	pShader->Use("Textured");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);
	model = glm::mat4(1.0f);
	pShader->SetMat4("model", model);
	pShader->SetVec4("objColor", 1.0f, 1.0f, 1.0f, 1.0f);
	pShader->SetFloat("tileS", 6.0f);
	pShader->SetFloat("tileT", 6.0f);
	pShader->SetBool("isTile", true);
	pTextures->ApplyTexture(0);
	glActiveTexture(GL_TEXTURE0);
	pShader->SetInt("Texture0", 0);
	DrawFloor();

	// Caixa 1
	pShader->Use("Textured");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.5f));
	model = glm::scale(model, glm::vec3(1.0f));
	pShader->SetMat4("model", model);
	pShader->SetVec4("objColor", 1.0f, 1.0f, 1.0f, 1.0f);
	pShader->SetFloat("tileS", 1.0f);
	pShader->SetFloat("tileT", 1.0f);
	pShader->SetBool("isTile", false);
	pTextures->ApplyTexture(1);
	glActiveTexture(GL_TEXTURE0);
	pShader->SetInt("Texture0", 0);
	DrawCrate();

	// caixa 2
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 1.0f, 1.5f));
	model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.7f, 0.75f, 0.7f));
	pShader->SetMat4("model", model);
	DrawCrate();

	// caixa 3
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(3.25f, 0.0f, 1.5f));
	model = glm::rotate(model, glm::radians(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.5f, 2.5f, 2.0f));
	pShader->SetMat4("model", model);
	DrawCrate();

	// casa
	pShader->Use("Textured");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-2.2f, 0.0f, -3.8f));
	pShader->SetMat4("model", model);
	pShader->SetVec4("objColor", 1.0f, 1.0f, 1.0f, 1.0f);
	pShader->SetFloat("tileS", 1.0f);
	pShader->SetFloat("tileT", 1.0f);
	pShader->SetBool("isTile", false);
	pTextures->ApplyTexture(2);
	glActiveTexture(GL_TEXTURE0);
	pShader->SetInt("Texture0", 0);
	DrawHouse();










	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                               DESENHA OS OBJETOS DA CENA (FIM)
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Draw text on the screen (disable depth test and use orthographic projection)
	glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));

	pShader->Use("Text2D");
	pShader->SetMat4("projection", projection);
	pShader->SetVec3("textColor", 1.0f, 1.0f, 1.0f);

	pText->RenderText(std::format("CamPosition\tx: {0:.2f} \t y: {1:.2f} \t z: {2:.2f}", pCamera->Position.x, pCamera->Position.y, pCamera->Position.z), 5.0f, 100.0f, 0.3f);
	pText->RenderText(std::format("CamFoward\tx: {0:.2f} \t y: {1:.2f} \t z: {2:.2f}", pCamera->Front.x, pCamera->Front.y, pCamera->Front.z), 5.0f, 80.0f, 0.3f);

	pText->RenderText(std::format("FPS: {0:.0f}\tDeltatime: {1:.2f}ms\tTimer: {2:.2f}s", pTimer->GetFPS(), pTimer->GetDeltaTime(), pTimer->GetTime() / 1000), 5.0f, 10.0f, 0.3f);
	glEnable(GL_DEPTH_TEST);
}


void Scene3::ProcessSceneInput(GLFWwindow* window, float deltaTime)
{
	InputManager& input = InputManager::GetInstance();

	// Camera movement
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

	// Increase/decrease movement speed
	if (input.IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
		pCamera->MovementSpeed = SPEED_MULTIPLIER;
	else
		pCamera->MovementSpeed = SPEED;

	// Process mouse movement
	glm::vec2 mouseDelta = input.GetMouseDelta();
	if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f)
	{
		pCamera->ProcessMouseMovement(mouseDelta.x, mouseDelta.y);
	}

	// Process scroll
	float scrollOffset = input.GetScrollOffset();
	if (scrollOffset != 0.0f)
	{
		pCamera->ProcessMouseScroll(scrollOffset);
	}
}


void Scene3::CreateFloor()
{
	// Define vertices
	float v[] = {


	   -10.0f, 0.0f,  10.0f,	0.0f, 0.0f,
		10.0f, 0.0f,  10.0f,	1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	1.0f, 1.0f,

		10.0f, 0.0f, -10.0f,	1.0f, 1.0f,
	   -10.0f, 0.0f, -10.0f,	0.0f, 1.0f,
	   -10.0f, 0.0f,  10.0f,	0.0f, 0.0f,



	};

	// Generate and bind VAO and VBO
	glGenVertexArrays(1, &floorVAO);
	glGenBuffers(1, &floorVBO);

	glBindVertexArray(floorVAO);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);

	// Atributo de Posi��es
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Atributo de Cores
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	glBindVertexArray(0); // Unbind VAO

}

void Scene3::DestroyFloor()
{
	glDeleteVertexArrays(1, &floorVAO);
	glDeleteBuffers(1, &floorVBO);
}

void Scene3::DrawFloor()
{
	glBindVertexArray(floorVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Scene3::CreateCrate()
{
	float v[] = {
		 0.5f, 0.0f, -0.5f,  0.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,  1.0f, 0.0f,
		-0.5f, 1.0f, -0.5f,  1.0f, 1.0f,
		-0.5f, 1.0f, -0.5f,  1.0f, 1.0f,
		 0.5f, 1.0f, -0.5f,  0.0f, 1.0f,
		 0.5f, 0.0f, -0.5f,  0.0f, 0.0f,

		-0.5f, 0.0f,  0.5f, 0.0f, 0.0f,
		 0.5f, 0.0f,  0.5f, 1.0f, 0.0f,
		 0.5f, 1.0f,  0.5f, 1.0f, 1.0f,
		 0.5f, 1.0f,  0.5f, 1.0f, 1.0f,
		-0.5f, 1.0f,  0.5f, 0.0f, 1.0f,
		-0.5f, 0.0f,  0.5f, 0.0f, 0.0f,

		-0.5f, 1.0f,  0.5f,  1.0f, 1.0f,
		-0.5f, 1.0f, -0.5f,  0.0f, 1.0f,
		-0.5f, 0.0f, -0.5f,  0.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,  0.0f, 0.0f,
		-0.5f, 0.0f,  0.5f,  1.0f, 0.0f,
		-0.5f, 1.0f,  0.5f,  1.0f, 1.0f,

		 0.5f, 1.0f, -0.5f,  1.0f, 1.0f,
		 0.5f, 1.0f,  0.5f,  0.0f, 1.0f,
		 0.5f, 0.0f,  0.5f,  0.0f, 0.0f,
		 0.5f, 0.0f,  0.5f,  0.0f, 0.0f,
		 0.5f, 0.0f, -0.5f,  1.0f, 0.0f,
		 0.5f, 1.0f, -0.5f,  1.0f, 1.0f,

		-0.5f, 0.0f, -0.5f,  0.0f, 0.0f,
		 0.5f, 0.0f, -0.5f,  1.0f, 0.0f,
		 0.5f, 0.0f,  0.5f,  1.0f, 1.0f,
		 0.5f, 0.0f,  0.5f,  1.0f, 1.0f,
		-0.5f, 0.0f,  0.5f,  0.0f, 1.0f,
		-0.5f, 0.0f, -0.5f,  0.0f, 0.0f,

		-0.5f, 1.0f,  0.5f,  0.0f, 0.0f,
		 0.5f, 1.0f,  0.5f,  1.0f, 0.0f,
		 0.5f, 1.0f, -0.5f,  1.0f, 1.0f,
		 0.5f, 1.0f, -0.5f,  1.0f, 1.0f,
		-0.5f, 1.0f, -0.5f,  0.0f, 1.0f,
		-0.5f, 1.0f,  0.5f,  0.0f, 0.0f
	};

	glGenVertexArrays(1, &crateVAO);
	glGenBuffers(1, &crateVBO);

	glBindVertexArray(crateVAO);
	glBindBuffer(GL_ARRAY_BUFFER, crateVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);

	const GLsizei stride = static_cast<GLsizei>(5 * sizeof(float));
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void Scene3::DestroyCrate()
{
	glDeleteVertexArrays(1, &crateVAO);
	glDeleteBuffers(1, &crateVBO);
}

void Scene3::DrawCrate()
{
	glBindVertexArray(crateVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void Scene3::CreateHouse()
{
	float v[] = {
		// Frente bloco janela
		-3.0f, 0.0f, 1.0f,  0.0f, 0.0f,
		-1.0f, 0.0f, 1.0f,  0.5f, 0.0f,
		-1.0f, 2.0f, 1.0f,  0.5f, 0.5f,
		-3.0f, 0.0f, 1.0f,  0.0f, 0.0f,
		-1.0f, 2.0f, 1.0f,  0.5f, 0.5f,
		-3.0f, 2.0f, 1.0f,  0.0f, 0.5f,
		// Frente bloco 2 porta
		-1.0f, 0.0f, 1.0f,  0.5f, 0.5f,
		 1.0f, 0.0f, 1.0f,  1.0f, 0.5f,
		 1.0f, 2.0f, 1.0f,  1.0f, 1.0f,
		-1.0f, 0.0f, 1.0f,  0.5f, 0.5f,
		 1.0f, 2.0f, 1.0f,  1.0f, 1.0f,
		-1.0f, 2.0f, 1.0f,  0.5f, 1.0f,
		// Frente bloco parede
		 1.0f, 0.0f, 1.0f,  0.0f, 0.5f,
		 3.0f, 0.0f, 1.0f,  0.5f, 0.5f,
		 3.0f, 2.0f, 1.0f,  0.5f, 1.0f,
		 1.0f, 0.0f, 1.0f,  0.0f, 0.5f,
		 3.0f, 2.0f, 1.0f,  0.5f, 1.0f,
		 1.0f, 2.0f, 1.0f,  0.0f, 1.0f,

		// Fundo
		-3.0f, 0.0f, -1.0f,  0.0f, 0.5f,
		 3.0f, 0.0f, -1.0f,  0.5f, 0.5f,
		 3.0f, 2.0f, -1.0f,  0.5f, 1.0f,
		-3.0f, 0.0f, -1.0f,  0.0f, 0.5f,
		 3.0f, 2.0f, -1.0f,  0.5f, 1.0f,
		-3.0f, 2.0f, -1.0f,  0.0f, 1.0f,

		// Lateral esquerda
		-3.0f, 0.0f, -1.0f,  0.0f, 0.0f,
		-3.0f, 0.0f,  1.0f,  0.5f, 0.0f,
		-3.0f, 2.0f,  1.0f,  0.5f, 0.5f,
		-3.0f, 0.0f, -1.0f,  0.0f, 0.0f,
		-3.0f, 2.0f,  1.0f,  0.5f, 0.5f,
		-3.0f, 2.0f, -1.0f,  0.0f, 0.5f,

		// Lateral direita
		 3.0f, 0.0f,  1.0f,  0.0f, 0.0f,
		 3.0f, 0.0f, -1.0f,  0.5f, 0.0f,
		 3.0f, 2.0f, -1.0f,  0.5f, 0.5f,
		 3.0f, 0.0f,  1.0f,  0.0f, 0.0f,
		 3.0f, 2.0f, -1.0f,  0.5f, 0.5f,
		 3.0f, 2.0f,  1.0f,  0.0f, 0.5f,

		// Empenas
		-3.0f, 2.0f, -1.0f,  0.0f, 0.5f,
		-3.0f, 2.0f,  1.0f,  0.5f, 0.5f,
		-3.0f, 3.0f,  0.0f,  0.25f, 1.0f,
		 3.0f, 2.0f,  1.0f,  0.0f, 0.5f,
		 3.0f, 2.0f, -1.0f,  0.5f, 0.5f,
		 3.0f, 3.0f,  0.0f,  0.25f, 1.0f,

		// Telhado frente
		-3.12f, 1.8f,  1.25f,  0.5f, 0.0f,
		 3.12f, 1.8f,  1.25f,  1.0f, 0.0f,
		 3.12f, 3.0f,  0.0f,  1.0f, 0.5f,
		-3.12f, 1.8f,  1.25f,  0.5f, 0.0f,
		 3.12f, 3.0f,  0.0f,  1.0f, 0.5f,
		-3.12f, 3.0f,  0.0f,  0.5f, 0.5f,
		// Telhado trás
		 3.12f, 1.8f, -1.25f,  0.5f, 0.0f,
		-3.12f, 1.8f, -1.25f,  1.0f, 0.0f,
		-3.12f, 3.0f,  0.0f,  1.0f, 0.5f,
		 3.12f, 1.8f, -1.25f,  0.5f, 0.0f,
		-3.12f, 3.0f,  0.0f,  1.0f, 0.5f,
		 3.12f, 3.0f,  0.0f,  0.5f, 0.5f,
	};

	houseVertexCount = static_cast<GLsizei>(sizeof(v) / (5 * sizeof(float)));

	glGenVertexArrays(1, &houseVAO);
	glGenBuffers(1, &houseVBO);
	glBindVertexArray(houseVAO);
	glBindBuffer(GL_ARRAY_BUFFER, houseVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
	const GLsizei stride = static_cast<GLsizei>(5 * sizeof(float));
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}

void Scene3::DestroyHouse()
{
	glDeleteVertexArrays(1, &houseVAO);
	glDeleteBuffers(1, &houseVBO);
}

void Scene3::DrawHouse()
{
	glBindVertexArray(houseVAO);
	glDrawArrays(GL_TRIANGLES, 0, houseVertexCount);
	glBindVertexArray(0);
}