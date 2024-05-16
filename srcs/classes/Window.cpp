#include <classes/Window.hpp>

void framebuffer_size_callback(__attribute__((unused)) GLFWwindow *window, int width, int height) // callback called by GLFW whevenever window is resized
{
	glViewport(0, 0, width, height); // We tell openGL the new size of the window
}

static void glfwError(__attribute__((unused)) int id, const char *description)
{
	std::cout << description << std::endl;
}

Window::Window(const char *name, DrawMode drawMode)
{
	glfwSetErrorCallback(&glfwError);
	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW" << std::endl;
		assert(!"Window::Window glfwInit failed");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);				   // OpenGL version ->4.6
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);				   // OpenGL version   4.6<-
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We use the core profile because we want to use the modulable part of OpenGL

	window = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, name, NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		assert(!"Window::Window glfwCreateWindow failed");
	}
	glfwMakeContextCurrent(window); // Set active context to the window we created

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // Load OpenGL function pointers with GLFW OS pointer
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		assert(!"Window::Window gladLoadGLLoader failed");
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Tell OpenGL that we want to capture and hide the cursor

	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT); // Tell OpenGL the size of the drawing window
	glClearColor(0.25f, 0.35f, 0.6f, 1.0f);						   // Sets clear color
	if (drawMode == DrawMode::LINE)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // draw as lines
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // default draw (filled)
	}
	glfwSwapInterval(1);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // We tell glfw which function to call whenever window is resized
}

int Window::ShouldContinue()
{
	if (glfwWindowShouldClose(window))
	{
		glfwTerminate();
		return 0;
	}
	return 1;
}

void Window::Clear()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); // Clears window with set clear color
}

void Window::SwapBuffersAndPollEvents()
{
	glfwSwapBuffers(window); // Swaps Window buffers to avoid flickering
	glfwPollEvents();		 // Checks for event and calls callbacks (framebuffer_size_callback wouldn't be called if it wasn't for this function)
}

void Window::SendKeys(u_char *keyState, __attribute__((unused)) double mouseMoveX, __attribute__((unused)) double mouseMoveY)
{
	if (keyState[KEY_ESCAPE] & KEY_PRESS)
		glfwSetWindowShouldClose(window, true);
}

Window::~Window() {}

GLFWwindow *Window::GetWindow() const
{
	return window;
}