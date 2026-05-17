#include "Scene2.h"


Scene2::Scene2()
{
	// Cria a câmera
	pCamera = NULL;
	pCamera = new CCamera(glm::vec3(0.0f, 3.0f, 20.0f));
	pCamera->MovementSpeed = 100.0f;

	// Cria o Timer
	pTimer = NULL;
	pTimer = new CTimer();
	pTimer->Init();

	// Cria gerenciador de impressão de texto na tela
	pText = NULL;
	pText = new CText();
	pText->InitTextManager();

	// Cria os shaders
	pShader = NULL;
	pShader = new CShader();
	pShader->LoadShader("Grid", "Scenes/Common/grid.vert", "Scenes/Common/grid.frag");
	pShader->LoadShader("Axis", "Scenes/Common/axis.vert", "Scenes/Common/axis.frag");
	pShader->LoadShader("Text2D", "Scenes/Common/Text2D.vert", "Scenes/Common/Text2D.frag");
	pShader->LoadShader("Decal", "Scenes/Scene2/Decal.vert", "Scenes/Scene2/Decal.frag");
	pShader->LoadShader("River", "Scenes/Scene2/River.vert", "Scenes/Scene2/River.frag");
	pShader->LoadShader("MTMask", "Scenes/Scene2/MTMask.vert", "Scenes/Scene2/MTMask.frag");
	pShader->LoadShader("LightMap", "Scenes/Scene2/LightMap.vert", "Scenes/Scene2/LightMap.frag");


	// Cria o gerenciador de grid e axis
	pGridAxis = NULL;
	pGridAxis = new CGridAxis(50.0f);

	// Cria o gerenciador de texturas
	pTextures = NULL;
	pTextures = new CTextures();
	pTextures->LoadTextureAnisotropic(0, "Scenes/Scene2/B1.jpg");
	pTextures->LoadTextureAnisotropic(1, "Scenes/Scene2/Blood.bmp");
	pTextures->LoadTextureAnisotropic(2, "Scenes/Scene2/brick.bmp");
	pTextures->LoadTextureAnisotropic(3, "Scenes/Scene2/lightmap.bmp");
	pTextures->LoadTextureAnisotropic(4, "Scenes/Scene2/grass.bmp");
	pTextures->LoadTextureAnisotropic(5, "Scenes/Scene2/Mask.bmp");
	pTextures->LoadTextureAnisotropic(6, "Scenes/Scene2/mud.bmp");
	pTextures->LoadTextureAnisotropic(7, "Scenes/Scene2/water.bmp");

	// Define as repetições TILE para o plano
	tileS0 = 1.0f;
	tileT0 = 1.0f;
	tileS1 = 1.0f;
	tileT1 = 1.0f;

	// Cria o plano
	CreatePlane();
}

Scene2::~Scene2()
{
	// Destroi o plano
	DestroyPlane();

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

void Scene2::DrawScene()
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

	/*glm::mat4 m = glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	)*/


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
	//                               DESENHA OS OBJETOS DA CENA (INÍCIO)
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	pShader->Use("Decal");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
	pShader->SetMat4("model", model);

	pShader->SetVec4("objColor", 1.0f, 1.0f, 1.0f, 1.0f);

	glActiveTexture(GL_TEXTURE0);
	pTextures->ApplyTexture(0);
	pShader->SetInt("Texture0", 0);

	glActiveTexture(GL_TEXTURE1);
	pTextures->ApplyTexture(1);
	pShader->SetInt("Texture1", 1);
	DrawPlane();

	///////////////////////////////////////////////////////////////////

	pShader->Use("LightMap");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-10.0f, 5.0f, 0.0f));
	pShader->SetMat4("model", model);

	pShader->SetVec4("objColor", 1.0f, 1.0f, 1.0f, 1.0f);
	pShader->SetFloat("uTime", glfwGetTime());

	glActiveTexture(GL_TEXTURE0);
	pTextures->ApplyTexture(0);
	pShader->SetInt("Texture0", 0);

	glActiveTexture(GL_TEXTURE1);
	pTextures->ApplyTexture(3);
	pShader->SetInt("Texture1", 1);
	DrawPlane();

	///////////////////////////////////////////////////////////////////
	pShader->Use("River");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.1f, 5.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(3.0f, 1.0f, 1.0f));
	pShader->SetMat4("model", model);

	pShader->SetVec4("objColor", 1.0f, 1.0f, 1.0f, 1.0f);
	pShader->SetFloat("tileS0", 3.0f);
	pShader->SetFloat("tileT0", 2.0f);
	pShader->SetFloat("tileS1", 3.0f);
	pShader->SetFloat("tileT1", 2.0f);

	pShader->SetFloat("uTime0", glfwGetTime() * 0.1f);
	pShader->SetFloat("uTime1", glfwGetTime() * 0.05f);

	glActiveTexture(GL_TEXTURE0);
	pTextures->ApplyTexture(7);
	pShader->SetInt("Texture0", 0);

	glActiveTexture(GL_TEXTURE1);
	pTextures->ApplyTexture(7);
	pShader->SetInt("Texture1", 1);
	DrawPlane();
	/////////////////////////////////////////////////////////////////////////////

	pShader->Use("MTMask");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(10.0f, 5.0f, 0.0f));
	pShader->SetMat4("model", model);

	pShader->SetVec4("objColor", 1.0f, 1.0f, 1.0f, 1.0f);

	glActiveTexture(GL_TEXTURE0);
	pTextures->ApplyTexture(4);
	pShader->SetInt("Texture0", 0);

	glActiveTexture(GL_TEXTURE1);
	pTextures->ApplyTexture(5);
	pShader->SetInt("Texture1", 1);

	glActiveTexture(GL_TEXTURE2);
	pTextures->ApplyTexture(6);
	pShader->SetInt("Texture2", 2);
	DrawPlane();















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


void Scene2::ProcessSceneInput(GLFWwindow* window, float deltaTime)
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





void Scene2::CreatePlane()
{
	// Create a textured cube
	float v[] =
	{
		// positions         // TexCoords
		-5.0f, -5.0f,  0.0f, 0.0f, 0.0f,
		 5.0f, -5.0f,  0.0f, 1.0f, 0.0f,
		 5.0f,  5.0f,  0.0f, 1.0f, 1.0f,
		 5.0f,  5.0f,  0.0f, 1.0f, 1.0f,
		-5.0f,  5.0f,  0.0f, 0.0f, 1.0f,
		-5.0f, -5.0f,  0.0f, 0.0f, 0.0f
	};


	// Generate and bind VAO and VBO
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);

	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);

	// Stride
	int stride = 5 * sizeof(float);

	// Set position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	// Set texture coordinate attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO
}

void Scene2::DestroyPlane()
{
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);
}

void Scene2::DrawPlane()
{
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}
