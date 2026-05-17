#include "Scene1.h"


Scene1::Scene1()
{
	// Cria a câmera
	pCamera = NULL;
	pCamera = new CCamera(glm::vec3(0.0f, 1.0f, 5.0f));
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
	pShader->LoadShader("Textured", "Scenes/Scene1/Textured.vert", "Scenes/Scene1/Textured.frag");

	// Cria o gerenciador de grid e axis
	pGridAxis = NULL;
	pGridAxis = new CGridAxis(50.0f);

	// Cria o gerenciador de texturas
	pTextures = NULL;
	pTextures = new CTextures();
	pTextures->LoadTextureAnisotropic(0, "Scenes/Scene1/CRATE.BMP");
	pTextures->LoadTextureAnisotropic(1, "Scenes/Scene1/TriangleFaces.BMP");

	objScale = glm::vec3(1.0f);
	rotationAngle = 0.0f;
	rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	rotSpeed = 1.0f;
	rotAxisMode = 1;
	objectColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	enableRotation = false;
	moveX = 0.0f;
	moveY = 0.0f;
	moveZ = 0.0f;

	// Cursor state
	bCursorDisabled = true;

	tileS = 1;
	tileT = 1;
	isTile = false;
	
	CreateCube();
	CreatePyramid();
}

Scene1::~Scene1()
{
	DestroyCube();
	DestroyPyramid();

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

void Scene1::DrawScene()
{
	// Update timer (must be called at start of frame)
	pTimer->Update();

	// Clear the color and depth buffers
	glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ShowGUI();

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

	// Draw the textured cube
	pShader->Use("Textured");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(moveX, moveY, moveZ));
	model = glm::rotate(model, glm::radians(rotationAngle), rotationAxis);
	model = glm::scale(model, objScale);
	pShader->SetMat4("model", model);
	pShader->SetVec4("objColor", objectColor);
	pShader->SetFloat("tileS", tileS);
	pShader->SetFloat("tileT", tileT);
	pShader->SetBool("isTile", isTile);

	pTextures->ApplyTexture(0);
	glActiveTexture(GL_TEXTURE0);
	pShader->SetInt("Texture0", 0);

	DrawCube();



	// Draw the textured Pyramid
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0f));
	pShader->SetMat4("model", model);
	pShader->SetVec4("objColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	pShader->SetFloat("tileS", 2);
	pShader->SetFloat("tileT", 2);
	pShader->SetBool("isTile", false);

	pTextures->ApplyTexture(1);
	glActiveTexture(GL_TEXTURE0);
	pShader->SetInt("Texture0", 0);

	DrawPyramid();
















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


void Scene1::ProcessSceneInput(GLFWwindow* window, float deltaTime)
{
	InputManager& input = InputManager::GetInstance();
	
	if (input.IsKeyJustPressed(GLFW_KEY_ENTER))
	{
		bCursorDisabled = !bCursorDisabled;
		input.SetCursorMode(bCursorDisabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

		std::cout << "Cursor Disabled: '" << bCursorDisabled << "'" << std::endl;
	}

	if (bCursorDisabled)
	{
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
	else
	{

	}
}


void Scene1::CreateCube()
{
	// Create a textured cube
	float v[] = 
	{
		// positions          // TexCoords
		 0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 1.0f,

		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f
	};


	// Generate and bind VAO and VBO
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);

	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);

	// Set position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Set texture coordinate attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO
}

void Scene1::DestroyCube()
{
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);
}

void Scene1::DrawCube()
{
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}


void Scene1::ShowGUI()
{
	std::string sEnableRotation = enableRotation ? "Disable Rotation" : "Enable Rotation";

	//ImGuiColorEditFlags colorFlags = ImGuiColorEditFlags_Float;
	ImGuiColorEditFlags colorFlags = ImGuiColorEditFlags_Uint8;

	// Create a window called "Box Options"
	ImGui::Begin("Box Options");

	ImGui::Text("Rotate Axis:");
	ImGui::RadioButton("X", &rotAxisMode, 0);
	ImGui::RadioButton("Y", &rotAxisMode, 1);
	ImGui::RadioButton("Z", &rotAxisMode, 2);
	ImGui::NewLine();
	ImGui::Text("Rotate Speed:");
	ImGui::SliderFloat("Speed", &rotSpeed, 0.0f, 2.0f, "%.01f");
	ImGui::NewLine();
	if (ImGui::Button(sEnableRotation.c_str()))
		enableRotation = !enableRotation;
	ImGui::NewLine();
	ImGui::Text("Move Object:");
	ImGui::SliderFloat("Move X: ", &moveX, -10.0f, 10.0f, "%.1f");
	ImGui::SliderFloat("Move Y: ", &moveY, -10.0f, 10.0f, "%.1f");
	ImGui::SliderFloat("Move Z: ", &moveZ, -10.0f, 10.0f, "%.1f");
	ImGui::NewLine();
	ImGui::ColorEdit4("Object Color", (float*)&objectColor, colorFlags);
	ImGui::NewLine();
	ImGui::InputFloat("Scale X: ", &objScale.x, 0.01f, 5.0f, "%.1f");
	ImGui::InputFloat("Scale Y: ", &objScale.y, 0.01f, 5.0f, "%.1f");
	ImGui::InputFloat("Scale Z: ", &objScale.z, 0.01f, 5.0f, "%.1f");	
	ImGui::NewLine();
	if (ImGui::Button("Reset Scene"))
	{
		objScale = glm::vec3(1.0f);
		rotationAngle = 0.0f;
		rotAxisMode = 1;
		rotSpeed = 1.0f;
		enableRotation = false;
		moveX = 0.0f;
		moveY = 0.0f;
		moveZ = 0.0f;
		objectColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		objScale = glm::vec3(1.0f);
	}

	// Close the window 
	ImGui::End(); 
		


	ImGui::Begin("Texure Options");
	ImGui::Checkbox("Tile Texture", &isTile);
	ImGui::SliderInt("Tile S", &tileS, 1, 5, "%d");
	ImGui::SliderInt("Tile T", &tileT, 1, 5, "%d");
	ImGui::End();



	switch (rotAxisMode)
	{
	case 0:
		rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f);
		break;

	case 1:
		rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
		break;

	case 2:
		rotationAxis = glm::vec3(0.0f, 0.0f, 1.0f);
		break;

	}

	if (enableRotation)
		rotationAngle += rotSpeed * pTimer->GetDeltaTime();

	if (rotationAngle > 360.0f)
		rotationAngle -= 360.0f;
}


void Scene1::CreatePyramid()
{
	// Define vertices for a simple pyramid (3D triangle)
	float v[] = {
		// Vertices			// Coord. Textura
		// Front face
	   -0.5f, 0.0f,  0.5f, 0.02f, 0.56f,
		0.5f, 0.0f,  0.5f, 0.435f, 0.56f,
		0.0f, 1.0f,  0.0f, 0.23f, 0.9f,

		0.5f, 0.0f,  0.5f, 0.53f, 0.56f,
		0.5f, 0.0f, -0.5f, 0.93f, 0.56f,
		0.0f, 1.0f,  0.0f, 0.73f, 0.9f,

		0.5f, 0.0f, -0.5f, 0.02f, 0.1f,
	   -0.5f, 0.0f, -0.5f, 0.435f, 0.1f,
		0.0f, 1.0f,  0.0f, 0.23f, 0.45f,

	   -0.5f, 0.0f, -0.5f, 0.52f, 0.1f,
	   -0.5f, 0.0f,  0.5f, 0.94f, 0.1f,
		0.0f, 1.0f,  0.0f, 0.73f, 0.45f,

		// Base (two triangl
	   -0.5f, 0.0f,  -0.5f, 0.2f, 0.30f,
		0.5f, 0.0f,  -0.5f,	0.3f, 0.30f,
		0.5f, 0.0f,   0.5f,	0.3f, 0.31f,

		0.5f, 0.0f,  0.5f,  0.3f, 0.31f,
	   -0.5f, 0.0f,  0.5f,	0.3f, 0.30f,
	   -0.5f, 0.0f, -0.5f,	0.2f, 0.30f,
	};

	// Generate and bind VAO and VBO
	glGenVertexArrays(1, &pyramidVAO);
	glGenBuffers(1, &pyramidVBO);

	glBindVertexArray(pyramidVAO);
	glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);

	float stride = 5 * sizeof(float);

	// Atributo de Posições
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	// Atributo de Coord. Textura
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	glBindVertexArray(0); // Unbind VAO
}

void Scene1::DestroyPyramid()
{
	glDeleteVertexArrays(1, &pyramidVAO);
	glDeleteBuffers(1, &pyramidVBO);
}

void Scene1::DrawPyramid()
{
	glBindVertexArray(pyramidVAO);
	glDrawArrays(GL_TRIANGLES, 0, 18);
	glBindVertexArray(0);
}
