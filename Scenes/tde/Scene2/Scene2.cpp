#include "Scene2.h"
#include <cmath>

Scene2::Scene2()
{
	pCamera = new CCamera(glm::vec3(0.0f, 3.0f, 20.0f));
	pCamera->MovementSpeed = 100.0f;

	pTimer = new CTimer();
	pTimer->Init();

	pText = new CText();
	pText->InitTextManager();

	pShader = new CShader();
	pShader->LoadShader("Grid", "Scenes/Common/grid.vert", "Scenes/Common/grid.frag");
	pShader->LoadShader("Axis", "Scenes/Common/axis.vert", "Scenes/Common/axis.frag");
	pShader->LoadShader("Text2D", "Scenes/Common/Text2D.vert", "Scenes/Common/Text2D.frag");
	pShader->LoadShader("Solid", "Scenes/Common/Solid.vert", "Scenes/Common/Solid.frag");
	pShader->LoadShader("exerc3", "Scenes/tde/Scene2/exerc3.vert", "Scenes/tde/Scene2/exerc3.frag");

	pGridAxis = new CGridAxis(50.0f);
	CreateFloor();
	InitTree();
	InitSnowman();
}

Scene2::~Scene2()
{
	CleanupTree();
	CleanupSnowman();
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
	pTimer->Update();

	glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection = glm::perspective(glm::radians(pCamera->Zoom), ASPECT_RATIO, Z_NEAR, Z_FAR);
	glm::mat4 view = pCamera->GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);

	// grid
	pShader->Use("Grid");
	pShader->SetMat4("uProj", projection);
	pShader->SetMat4("uView", view);
	pShader->SetFloat("uGridSpacing", 1.0f);
	pShader->SetFloat("uFadeStart", 18.f);
	pShader->SetFloat("uFadeEnd", 100.f);
	pShader->SetVec3("uCamPos", pCamera->Position);
	pGridAxis->DrawGrid();

	// eixos
	pShader->Use("Axis");
	pShader->SetMat4("uProj", projection);
	pShader->SetMat4("uView", view);
	pGridAxis->DrawAxis();

	// chão
	pShader->Use("Solid");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);
	pShader->SetMat4("model", glm::mat4(1.0f));
	DrawFloor();

	// árvores (cada uma: translate no chão + scale + tronco + 2 copas)
	pShader->Use("exerc3");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);

	// Árvore 1 — centro em (3, 0, -5), tamanho normal
	{
		glm::mat4 base = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, -5.0f));
		base = glm::scale(base, glm::vec3(1.0f));

		model = base * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.55f, 0.0f));
		pShader->SetMat4("model", model);
		pShader->SetVec3("objColor", 0.32f, 0.2f, 0.12f);
		DrawTreeTrunk();

		model = base * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.1f, 0.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(0.55f, 0.9f, 0.55f));
		pShader->SetMat4("model", model);
		pShader->SetVec3("objColor", 0.12f, 0.48f, 0.14f);
		DrawTreeCone();

		model = base * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.85f, 0.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(0.38f, 0.65f, 0.38f));
		pShader->SetMat4("model", model);
		pShader->SetVec3("objColor", 0.1f, 0.42f, 0.13f);
		DrawTreeCone();
	}

	// Árvore 2 — (-4, 0, 2), menor
	{
		glm::mat4 base = glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, 0.0f, 2.0f));
		base = glm::scale(base, glm::vec3(0.65f));

		model = base * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.55f, 0.0f));
		pShader->SetMat4("model", model);
		pShader->SetVec3("objColor", 0.32f, 0.2f, 0.12f);
		DrawTreeTrunk();

		model = base * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.1f, 0.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(0.55f, 0.9f, 0.55f));
		pShader->SetMat4("model", model);
		pShader->SetVec3("objColor", 0.12f, 0.48f, 0.14f);
		DrawTreeCone();

		model = base * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.85f, 0.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(0.38f, 0.65f, 0.38f));
		pShader->SetMat4("model", model);
		pShader->SetVec3("objColor", 0.1f, 0.42f, 0.13f);
		DrawTreeCone();
	}

	// Árvore 3 — (8, 0, -8), maior
	{
		glm::mat4 base = glm::translate(glm::mat4(1.0f), glm::vec3(8.0f, 0.0f, -8.0f));
		base = glm::scale(base, glm::vec3(1.35f));

		model = base * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.55f, 0.0f));
		pShader->SetMat4("model", model);
		pShader->SetVec3("objColor", 0.32f, 0.2f, 0.12f);
		DrawTreeTrunk();

		model = base * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.1f, 0.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(0.55f, 0.9f, 0.55f));
		pShader->SetMat4("model", model);
		pShader->SetVec3("objColor", 0.12f, 0.48f, 0.14f);
		DrawTreeCone();

		model = base * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.85f, 0.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(0.38f, 0.65f, 0.38f));
		pShader->SetMat4("model", model);
		pShader->SetVec3("objColor", 0.1f, 0.42f, 0.13f);
		DrawTreeCone();
	}

	// Árvore 4 — (-6, 0, -6)
	{
		glm::mat4 base = glm::translate(glm::mat4(1.0f), glm::vec3(-6.0f, 0.0f, -6.0f));
		base = glm::scale(base, glm::vec3(0.85f));

		model = base * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.55f, 0.0f));
		pShader->SetMat4("model", model);
		pShader->SetVec3("objColor", 0.32f, 0.2f, 0.12f);
		DrawTreeTrunk();

		model = base * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.1f, 0.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(0.55f, 0.9f, 0.55f));
		pShader->SetMat4("model", model);
		pShader->SetVec3("objColor", 0.12f, 0.48f, 0.14f);
		DrawTreeCone();

		model = base * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.85f, 0.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(0.38f, 0.65f, 0.38f));
		pShader->SetMat4("model", model);
		pShader->SetVec3("objColor", 0.1f, 0.42f, 0.13f);
		DrawTreeCone();
	}

	// Árvore 5 — (5, 0, 4)
	{
		glm::mat4 base = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 4.0f));
		base = glm::scale(base, glm::vec3(1.1f));

		model = base * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.55f, 0.0f));
		pShader->SetMat4("model", model);
		pShader->SetVec3("objColor", 0.32f, 0.2f, 0.12f);
		DrawTreeTrunk();

		model = base * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.1f, 0.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(0.55f, 0.9f, 0.55f));
		pShader->SetMat4("model", model);
		pShader->SetVec3("objColor", 0.12f, 0.48f, 0.14f);
		DrawTreeCone();

		model = base * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.85f, 0.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(0.38f, 0.65f, 0.38f));
		pShader->SetMat4("model", model);
		pShader->SetVec3("objColor", 0.1f, 0.42f, 0.13f);
		DrawTreeCone();
	}

	// boneco
	{
		const float r0 = 0.2f;
		const float r1 = 0.13f;
		const float r2 = 0.08f;

		const glm::vec3 posChao(-2.0f, 0.0f, 3.0f);
		const glm::vec3 pivotCorpo(posChao.x, r0, posChao.z);

		float t = pTimer->GetTime() / 1000.0f;
		float balanco = sinf(t * 4.0f) * glm::radians(14.0f);

		glm::mat4 balancoMat = glm::translate(glm::mat4(1.0f), pivotCorpo);
		balancoMat = glm::rotate(balancoMat, balanco, glm::vec3(0.0f, 0.0f, 1.0f));
		balancoMat = glm::translate(balancoMat, -pivotCorpo);

		glm::mat4 corpo = glm::translate(balancoMat, glm::vec3(posChao.x, r0, posChao.z));
		corpo = glm::scale(corpo, glm::vec3(r0));

		glm::mat4 meio = glm::translate(corpo, glm::vec3(0.0f, 0.77f + r1 / r0, 0.0f));
		meio = glm::scale(meio, glm::vec3(r1 / r0));

		glm::mat4 cabeca = glm::translate(meio, glm::vec3(0.0f, 0.77f + r2 / r1, 0.0f));
		cabeca = glm::scale(cabeca, glm::vec3(r2 / r1));

		pShader->SetMat4("model", corpo);
		pShader->SetVec3("objColor", 0.96f, 0.96f, 1.0f);
		DrawSnowSphere();

		pShader->SetMat4("model", meio);
		pShader->SetVec3("objColor", 0.96f, 0.96f, 1.0f);
		DrawSnowSphere();

		pShader->SetMat4("model", cabeca);
		pShader->SetVec3("objColor", 0.96f, 0.96f, 1.0f);
		DrawSnowSphere();

		// cartola - filhos da cabeça
		const float topoCabecaLocal = 0.95f;
		const float abaAltura = 0.018f;
		const float copoAltura = 0.085f;

		glm::mat4 chapeuAba = glm::translate(cabeca, glm::vec3(0.0f, topoCabecaLocal + abaAltura / (2.0f * r2), 0.0f));
		chapeuAba = glm::scale(chapeuAba, glm::vec3(1.65f, abaAltura / r2, 1.65f));

		glm::mat4 chapeuCopo = glm::translate(cabeca, glm::vec3(0.0f, topoCabecaLocal + (abaAltura + copoAltura * 0.5f) / r2, 0.0f));
		chapeuCopo = glm::scale(chapeuCopo, glm::vec3(0.85f, copoAltura / r2, 0.85f));

		pShader->SetMat4("model", chapeuAba);
		pShader->SetVec3("objColor", 0.1f, 0.1f, 0.12f);
		DrawHatBrim();

		pShader->SetMat4("model", chapeuCopo);
		pShader->SetVec3("objColor", 0.07f, 0.07f, 0.09f);
		DrawHatCrown();

		// cenoura (nariz)
		const float narizRaio = r2 * 0.22f;
		const float narizComprimento = r2 * 0.85f;
		glm::mat4 cenoura = glm::translate(cabeca, glm::vec3(0.0f, 0.0f, 1.0f));
		cenoura = glm::rotate(cenoura, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		cenoura = glm::scale(cenoura, glm::vec3(narizRaio / r2, narizComprimento / r2, narizRaio / r2));

		pShader->SetMat4("model", cenoura);
		pShader->SetVec3("objColor", 0.92f, 0.42f, 0.08f);
		DrawCarrotNose();

		// olhos
		const float olhoEscala = 0.11f;
		glm::mat4 olhoEsquerdo = glm::translate(cabeca, glm::vec3(-0.32f, 0.22f, 0.92f));
		olhoEsquerdo = glm::scale(olhoEsquerdo, glm::vec3(olhoEscala));

		glm::mat4 olhoDireito = glm::translate(cabeca, glm::vec3(0.32f, 0.22f, 0.92f));
		olhoDireito = glm::scale(olhoDireito, glm::vec3(olhoEscala));

		pShader->SetMat4("model", olhoEsquerdo);
		pShader->SetVec3("objColor", 0.05f, 0.05f, 0.05f);
		DrawSnowSphere();

		pShader->SetMat4("model", olhoDireito);
		pShader->SetVec3("objColor", 0.05f, 0.05f, 0.05f);
		DrawSnowSphere();

		// botões
		const float botaoEscala = 0.09f;
		glm::mat4 botaoCima = glm::translate(meio, glm::vec3(0.0f, 0.18f, 1.0f));
		botaoCima = glm::scale(botaoCima, glm::vec3(botaoEscala));

		glm::mat4 botaoBaixo = glm::translate(meio, glm::vec3(0.0f, -0.18f, 1.0f));
		botaoBaixo = glm::scale(botaoBaixo, glm::vec3(botaoEscala));

		pShader->SetMat4("model", botaoCima);
		pShader->SetVec3("objColor", 0.88f, 0.08f, 0.08f);
		DrawSnowSphere();

		pShader->SetMat4("model", botaoBaixo);
		pShader->SetVec3("objColor", 0.88f, 0.08f, 0.08f);
		DrawSnowSphere();
	}

	glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));

	pShader->Use("Text2D");
	pShader->SetMat4("projection", projection);
	pShader->SetVec3("textColor", 1.0f, 1.0f, 1.0f);

	pText->RenderText("Scene 4 — teclas 0-4 trocam de cena", 5.0f, 120.0f, 0.35f);
	pText->RenderText(std::format("CamPosition\tx: {0:.2f} \t y: {1:.2f} \t z: {2:.2f}", pCamera->Position.x, pCamera->Position.y, pCamera->Position.z), 5.0f, 100.0f, 0.3f);
	pText->RenderText(std::format("CamForward\tx: {0:.2f} \t y: {1:.2f} \t z: {2:.2f}", pCamera->Front.x, pCamera->Front.y, pCamera->Front.z), 5.0f, 80.0f, 0.3f);
	pText->RenderText(std::format("FPS: {0:.0f}\tDeltatime: {1:.2f}ms\tTimer: {2:.2f}s", pTimer->GetFPS(), pTimer->GetDeltaTime(), pTimer->GetTime() / 1000), 5.0f, 10.0f, 0.3f);
	glEnable(GL_DEPTH_TEST);
}

void Scene2::ProcessSceneInput(GLFWwindow* window, float deltaTime)
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

void Scene2::CreateFloor()
{
	// VBO
	const float w = 20.0f;
	float v[] = {
		-w, 0.0f,  w,  1.0f, 1.0f, 1.0f,
		 w, 0.0f,  w,  1.0f, 1.0f, 1.0f,
		 w, 0.0f, -w,  1.0f, 1.0f, 1.0f,
		 w, 0.0f, -w,  1.0f, 1.0f, 1.0f,
		-w, 0.0f, -w,  1.0f, 1.0f, 1.0f,
		-w, 0.0f,  w,  1.0f, 1.0f, 1.0f,
	};
	glGenVertexArrays(1, &floorVAO);
	glGenBuffers(1, &floorVBO);
	glBindVertexArray(floorVAO);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
	const GLsizei stride = static_cast<GLsizei>(6 * sizeof(float));
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}

void Scene2::DestroyFloor()
{
	glDeleteVertexArrays(1, &floorVAO);
	glDeleteBuffers(1, &floorVBO);
}

void Scene2::DrawFloor()
{
	glBindVertexArray(floorVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Scene2::InitTree()
{
	treeTrunk.generateCylinder(0.14f, 1.1f, 18);
	treeTrunk.setupBuffers();
	treeCone.generateCone(1.0f, 1.0f, 20);
	treeCone.setupBuffers();
}

void Scene2::CleanupTree()
{
	treeTrunk.cleanup();
	treeCone.cleanup();
}

void Scene2::DrawTreeTrunk()
{
	treeTrunk.render();
}

void Scene2::DrawTreeCone()
{
	treeCone.render();
}

void Scene2::InitSnowman()
{
	snowSphere.generateSphere(1.0f, 24, 16);
	snowSphere.setupBuffers();

	hatBrim.generateCylinder(1.0f, 1.0f, 24);
	hatBrim.setupBuffers();
	hatCrown.generateCylinder(1.0f, 1.0f, 24);
	hatCrown.setupBuffers();
	carrotNose.generateCone(1.0f, 1.0f, 16);
	carrotNose.setupBuffers();
}

void Scene2::CleanupSnowman()
{
	snowSphere.cleanup();
	hatBrim.cleanup();
	hatCrown.cleanup();
	carrotNose.cleanup();
}

void Scene2::DrawSnowSphere()
{
	snowSphere.render();
}

void Scene2::DrawHatBrim()
{
	hatBrim.render();
}

void Scene2::DrawHatCrown()
{
	hatCrown.render();
}

void Scene2::DrawCarrotNose()
{
	carrotNose.render();
}
