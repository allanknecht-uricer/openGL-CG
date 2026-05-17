#include "Scene0.h"


Scene0::Scene0()
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
	pShader->LoadShader("Solid", "Scenes/Scene0/Solid.vert", "Scenes/Scene0/Solid.frag");

	// Cria o gerenciador de grid e axis
	pGridAxis = NULL;
	pGridAxis = new CGridAxis(50.0f);

	// Cria o gerenciador de texturas
	pTextures = NULL;
	//pTextures = new CTextures();

	// Cria a pirâmide
	CreatePyramid();

	// Cria o cubo
	CreateCube();

	// Cria o triângulo
	CreateTriangle();

	speed = 0.05f;
}

Scene0::~Scene0()
{
	// Destroi a pirâmide
	DestroyPyramid();

	// Destroi o cubo
	DestroyCube();

	// Destroi o triângulo
	DestroyTriangle();

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

void Scene0::DrawScene()
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
	//                               DESENHA OS OBJETOS DA CENA (INÍCIO)
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Draw a simple pyramid
	pShader->Use("Solid");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::degrees((float)glfwGetTime() * speed), 
								glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f, 2.0f, 1.0f));
	pShader->SetMat4("model", model);
	DrawPyramid();


	// Draw a simple cube
	pShader->Use("Solid");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-2.0f, 0.5f, 0.0f));
	pShader->SetMat4("model", model);
	DrawCube();


	// Draw a simple triangle
	pShader->Use("Solid");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
	pShader->SetMat4("model", model);
	DrawTriangle();
















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
	pText->RenderText(std::format("Speed\tx: {0:.2f}", speed), 5.0f, 60.0f, 0.3f);


	pText->RenderText(std::format("FPS: {0:.0f}\tDeltatime: {1:.2f}ms\tTimer: {2:.2f}s", pTimer->GetFPS(), pTimer->GetDeltaTime(), pTimer->GetTime() / 1000), 5.0f, 10.0f, 0.3f);
	glEnable(GL_DEPTH_TEST);
}


void Scene0::ProcessSceneInput(GLFWwindow* window, float deltaTime)
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

	if (input.IsKeyPressed(GLFW_KEY_KP_ADD))
		speed += 0.01f;
	if (input.IsKeyPressed(GLFW_KEY_KP_SUBTRACT))
	{
		speed -= 0.01f;
		if (speed < 0.01f)
			speed = 0.01f;
	}


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


void Scene0::CreatePyramid()
{
	// Define vertices for a simple pyramid (3D triangle)
	float v[] = {
		// Vertices			// Cores
		// Front face
	   -0.5f, 0.0f, 0.5f,   1.0f, 0.0f, 0.0f,
		0.5f, 0.0f, 0.5f,	1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,	1.0f, 0.0f, 0.0f,

		// Right face
		0.5f, 0.0f,  0.5f,  0.0f, 1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f,
		0.0f, 1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

		// Back face
		0.5f, 0.0f, -0.5f,  0.0f, 0.0f, 1.0f,
	   -0.5f, 0.0f, -0.5f,	0.0f, 0.0f, 1.0f,
		0.0f, 1.0f,  0.0f,	0.0f, 0.0f, 1.0f,

		// Left face
	   -0.5f, 0.0f, -0.5f,  1.0f, 0.0f, 1.0f,
	   -0.5f, 0.0f,  0.5f,	1.0f, 0.0f, 1.0f,
		0.0f, 1.0f,  0.0f,	1.0f, 0.0f, 1.0f,

		// Base (two triangles)
	   -0.5f, 0.0f,  -0.5f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.0f,  -0.5f,	1.0f, 1.0f, 1.0f,
		0.5f, 0.0f,   0.5f,	1.0f, 1.0f, 1.0f,

	    0.5f, 0.0f,  0.5f,  1.0f, 1.0f, 1.0f,
	   -0.5f, 0.0f,  0.5f,	1.0f, 1.0f, 1.0f,
	   -0.5f, 0.0f, -0.5f,	1.0f, 1.0f, 1.0f
	};

	// Generate and bind VAO and VBO
	glGenVertexArrays(1, &pyramidVAO);
	glGenBuffers(1, &pyramidVBO);

	glBindVertexArray(pyramidVAO);
	glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);

	// Atributo de Posições
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Atributo de Cores
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	glBindVertexArray(0); // Unbind VAO
}

void Scene0::DestroyPyramid()
{
	glDeleteVertexArrays(1, &pyramidVAO);
	glDeleteBuffers(1, &pyramidVBO);
}

void Scene0::DrawPyramid()
{
	glBindVertexArray(pyramidVAO);
		glDrawArrays(GL_TRIANGLES, 0, 18);
	glBindVertexArray(0);
}


void Scene0::CreateCube()
{
	// Define vertices for a cube (positions)
	float v[] = {
		// Front face
		-0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,

		// Back face
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,

		// Left face
	   -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
	   -0.5f, -0.5f,  0.5f,	 0.0f, 0.0f, 1.0f,
	   -0.5f,  0.5f,  0.5f,	 0.0f, 0.0f, 1.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
	   -0.5f,  0.5f, -0.5f,	 0.0f, 0.0f, 1.0f,
	   -0.5f, -0.5f, -0.5f,	 0.0f, 0.0f, 1.0f,

	   // Right face
		0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,	 1.0f, 1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,	 1.0f, 1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,	 1.0f, 1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,	 1.0f, 1.0f, 1.0f,

		// Top face
	   -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,	 1.0f, 0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,	 1.0f, 0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
	   -0.5f,  0.5f, -0.5f,	 1.0f, 0.0f, 1.0f,
	   -0.5f,  0.5f,  0.5f,	 1.0f, 0.0f, 1.0f,

	   // Bottom face
	  -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
	   0.5f, -0.5f, -0.5f,	 1.0f, 0.0f, 0.0f,
	   0.5f, -0.5f,  0.5f,	 1.0f, 0.0f, 0.0f,
	   0.5f, -0.5f,  0.5f,	 1.0f, 0.0f, 0.0f,
	  -0.5f, -0.5f,  0.5f,	 1.0f, 0.0f, 0.0f,
	  -0.5f, -0.5f, -0.5f,	 1.0f, 0.0f, 0.0f,
	};

	// Generate and bind VAO and VBO
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);

	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);

	// Set position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO
}

void Scene0::DestroyCube()
{
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);
}

void Scene0::DrawCube()
{
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}



void Scene0::CreateTriangle()
{
	float positions[] = 
	{
		// x     y     z
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};

	float colors[] =
	{
		// r     g     b
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};

	// Generate and bind VAO and VBOs
	glGenVertexArrays(1, &triangleVAO);
	glBindVertexArray(triangleVAO);

	// Position VBO
	glGenBuffers(1, &trianglePositionVBO);
	glBindBuffer(GL_ARRAY_BUFFER, trianglePositionVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	// Color VBO
	glGenBuffers(1, &triangleColorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, triangleColorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO
}



void Scene0::DestroyTriangle()
{
	glDeleteVertexArrays(1, &triangleVAO);
	glDeleteBuffers(1, &trianglePositionVBO);
	glDeleteBuffers(1, &triangleColorVBO);
}

void Scene0::DrawTriangle()
{
	glBindVertexArray(triangleVAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
}