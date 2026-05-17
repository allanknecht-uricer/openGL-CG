#include "SceneManager.h"

// Construtor
CSceneManager::CSceneManager()
{
	// Inicializa ponteiro da cena como NULL
	this->Scene = NULL;

	// Desabilita modo wireframe por padrão
	bWireframeMode = false; 

	// Habilita VSync por padrão
	bVsyncEnabled = true;

	// Define a cena inicial
	uiCurrentScene = 2;
	ChangeScene(uiCurrentScene);
}


// Destrutor
CSceneManager::~CSceneManager(void)
{
	if (this->Scene)
	{
		delete this->Scene;
		this->Scene = NULL;
	}
}

// Chama o método de desenho da cena atual
void CSceneManager::DrawScene()
{
	this->Scene->DrawScene();
}

// Processa o input da cena atual e também inputs globais (como troca de cena, toggle de wireframe, etc)
void CSceneManager::ProcessSceneInput(GLFWwindow* window, float deltaTime)
{
	InputManager& input = InputManager::GetInstance();

	// Close window on ESC
	if (input.IsKeyJustPressed(GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, true);

	// Toggle cursor mode with TAB or VSync
	if (input.IsKeyJustPressed(GLFW_KEY_V))
	{
		//static bool cursorDisabled = true;
		//cursorDisabled = !cursorDisabled;
		//input.SetCursorMode(cursorDisabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

		bVsyncEnabled = !bVsyncEnabled;
		glfwSwapInterval(bVsyncEnabled);

		std::cout << "Vsync: '" << bVsyncEnabled << "'" << std::endl;
	}

	if (input.IsKeyJustPressed(GLFW_KEY_TAB))
	{
		// Toggle wireframe mode
		bWireframeMode = !bWireframeMode;

		std::cout << "Wireframe: '" << bWireframeMode << "'" << std::endl;
	}
	if (bWireframeMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Fill mode
	

	if (input.IsKeyJustPressed(GLFW_KEY_0))
		ChangeScene(0);
	if (input.IsKeyJustPressed(GLFW_KEY_1))
		ChangeScene(1);
	if (input.IsKeyJustPressed(GLFW_KEY_2))
		ChangeScene(2);
	if (input.IsKeyJustPressed(GLFW_KEY_3))
		ChangeScene(3);
	if (input.IsKeyJustPressed(GLFW_KEY_4))
		ChangeScene(4);



	// Processa input da cena atual
	Scene->ProcessSceneInput(window, deltaTime);
}

// Troca a cena atual, destruindo a cena anterior e criando a nova cena com base no ID passado
void CSceneManager::ChangeScene(unsigned int _uiCurrentScene)
{
	std::system("cls"); // Clear Console on Windows

	this->uiCurrentScene = _uiCurrentScene;

	if (this->Scene)
	{
		delete this->Scene;
		this->Scene = NULL;
	}


	switch (uiCurrentScene)
	{
	case 0:
		this->Scene = new Scene0();
		break;

	case 1:
		this->Scene = new Scene1();

		break;

	case 2:
		this->Scene = new Scene2();
		break;

	case 3:
		this->Scene = new Scene3();
		break;

	case 4:
		this->Scene = new Scene4();
		break;






	}
}

