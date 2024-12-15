#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>	

#include "Camera.h"

// Constants
#define MAX_KEYS GLFW_KEY_LAST
#define MAX_MOUSE_BUTTONS 3
constexpr float CAMERA_FAST_SPEED = 20.0f;
constexpr float CAMERA_NORMAL_SPEED = 5.0f;

class OpenGL_Graphics
{
private:
	int m_width;
	int m_height;
	std::string m_sAppName;

	short m_keyNewState[MAX_KEYS] = { 0 };
	short m_keyOldState[MAX_KEYS] = { 0 };
	short m_mouseOldState[MAX_MOUSE_BUTTONS];
	short m_mouseNewState[MAX_MOUSE_BUTTONS];
	struct sKeyState
	{
		bool bPressed;
		bool bReleased;
		bool bHeld;
	} m_keys[MAX_KEYS], m_mouse[MAX_MOUSE_BUTTONS];

	float m_mousePosX;
	float m_mousePosY;
	int m_mouseScroll;
	bool m_bMouseButtonHeld[MAX_MOUSE_BUTTONS] = { false };

	static std::atomic<bool> m_bIsRunning;
protected:
	GLFWwindow* window;

	enum class Mouse
	{
		LEFT = 0,
		RIGHT = 1,
		MIDDLE = 2,
		SCROLL_UP = 3,
		SCROLL_DOWN = 4
	};

public:
	float fTimeSinceStart = 0.0f;
	bool bFirstMouse = true;

	Camera camera;
	glm::mat4 matProjection;
	float fFov = 80.0f;
	unsigned int uboMatrices;	

private:
	// Main renderer thread which constantly renders to the screen
	void RendererThread()
	{
		// We want the window to be in the renderer thread context (important!)
		glfwMakeContextCurrent(window);

		float fAccumulatedTime = 0.0f;
		int iFrameCount = 0;

		if (!Setup())
			m_bIsRunning = false;

		UpdateProjectionMatrix();

		auto dt1 = std::chrono::system_clock::now();
		auto dt2 = std::chrono::system_clock::now();

		// Run as fast as possible
		while (m_bIsRunning)
		{
			dt2 = std::chrono::system_clock::now();
			std::chrono::duration<float> elapsedTime = dt2 - dt1;
			dt1 = dt2;

			float fElapsedTime = elapsedTime.count();
			fTimeSinceStart += fElapsedTime;

			// Update key and mouse states on each frame, they may be used later by the programmer
			// 1. Update key states
			for (int i = 0; i < MAX_KEYS; i++)
			{
				m_keyNewState[i] = glfwGetKey(window, i) == GLFW_PRESS;

				m_keys[i].bPressed = false;
				m_keys[i].bReleased = false;

				if (m_keyNewState[i] != m_keyOldState[i])
				{
					if (m_keyNewState[i])
					{
						m_keys[i].bPressed = !m_keys[i].bHeld;
						m_keys[i].bHeld = true;
					}
					else
					{
						m_keys[i].bReleased = true;
						m_keys[i].bHeld = false;
					}
				}

				m_keyOldState[i] = m_keyNewState[i];
			}
			
			// 2. Update mouse states
			for (int i = 0; i < MAX_MOUSE_BUTTONS; i++)
			{
				m_mouseNewState[i] = m_bMouseButtonHeld[i];

				m_mouse[i].bPressed = false;
				m_mouse[i].bReleased = false;

				if (m_mouseNewState[i] != m_mouseOldState[i])
				{
					if (m_mouseNewState[i])
					{
						m_mouse[i].bPressed = !m_mouse[i].bHeld;
						m_mouse[i].bHeld = true;
					}
					else
					{
						m_mouse[i].bReleased = true;
						m_mouse[i].bHeld = false;
					}
					m_mouseOldState[i] = m_mouseNewState[i];
				}
			}

			HandleInputs(fElapsedTime);

			if (!Update(fElapsedTime))
			{
				m_bIsRunning = false;
			}

			// FPS calculation
			iFrameCount++;
			fAccumulatedTime += fElapsedTime;

			// Update FPS every 0.5 seconds
			if (fAccumulatedTime >= 0.5f)
			{
				int fps = (int)(iFrameCount / fAccumulatedTime);
				
				if (window)
				{
					char s[32];
					sprintf_s(s, 32, "%s : %d FPS", m_sAppName.c_str(), fps);
					glfwSetWindowTitle(window, s);
				}

				fAccumulatedTime = 0.0f;
				iFrameCount = 0;
			}

			m_mouseScroll = 0;
			//m_mouse[2].bPressed = false;
			m_mouse[2].bReleased = false;

			// Swap buffers
			glfwSwapBuffers(window);
		}

		// Give the window context back to the main thread
		glfwMakeContextCurrent(nullptr);
	}

	void HandleInputs(float fElapsedTime)
	{
		/* ------------------------------------------ - Keyboard Control - ------------------------------------------- */
		if (GetKey('W').bHeld && !GetKey('S').bHeld)
			camera.ProcessKeyboard(CameraMovement::FORWARD, fElapsedTime);
		else if (GetKey('S').bHeld && !GetKey('W').bHeld)
			camera.ProcessKeyboard(CameraMovement::BACKWARD, fElapsedTime);

		if (GetKey('A').bHeld && !GetKey('D').bHeld)
			camera.ProcessKeyboard(CameraMovement::LEFT, fElapsedTime);
		else if (GetKey('D').bHeld && !GetKey('A').bHeld)
			camera.ProcessKeyboard(CameraMovement::RIGHT, fElapsedTime);

		if (GetKey(GLFW_KEY_SPACE).bHeld && !GetKey(GLFW_KEY_LEFT_SHIFT).bHeld)
			camera.ProcessKeyboard(CameraMovement::UP, fElapsedTime);
		else if (GetKey(GLFW_KEY_LEFT_SHIFT).bHeld && !GetKey(GLFW_KEY_SPACE).bHeld)
			camera.ProcessKeyboard(CameraMovement::DOWN, fElapsedTime);

		// Emulate a "zoom-in" view by decreasing the FOV if 'C' is pressed
		if (GetKey('C').bHeld)
		{
			if (fFov > 10.0f)
				fFov -= fElapsedTime * 200.0f;
			UpdateProjectionMatrix();
		}

		else if (GetKey('C').bReleased)
		{
			fFov = 80.0f;
			UpdateProjectionMatrix();
		}

		if (GetKey(GLFW_KEY_LEFT_CONTROL).bHeld)
			camera.fCameraSpeed = CAMERA_FAST_SPEED;
		else
			camera.fCameraSpeed = CAMERA_NORMAL_SPEED;

		if (GetKey(GLFW_KEY_HOME).bPressed)
			camera.init(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f));

		/* ------------------------------------------ - Mouse Control - ------------------------------------------- */
		camera.ProcessMouse(GetMousePosX(), GetMousePosY(), ScreenWidth(), ScreenHeight(), bFirstMouse);

		// Update view matrix
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera.getLookAt()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void UpdateProjectionMatrix()
	{
		matProjection = glm::perspective(fFov * pi / 180.0f, (float)ScreenWidth() / (float)ScreenHeight(), 0.1f, 1000.0f);
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(matProjection));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

public:
	int ScreenWidth() const  { return m_width; }
	int ScreenHeight() const { return m_height; }
	float GetMousePosX() const { return m_mousePosX; }
	float GetMousePosY() const { return m_mousePosY; }
	Mouse GetMouseScroll() const { return (Mouse)m_mouseScroll; }
	sKeyState GetMouseButton(Mouse button) const { return m_mouse[(int)button]; }
	sKeyState GetKey(int nKeyID) const { return m_keys[nKeyID]; }

	OpenGL_Graphics() : window(nullptr), m_width(0), m_height(0) {}

	~OpenGL_Graphics()
	{
		glDeleteBuffers(1, &uboMatrices);
	}

	void ConstructWindow(int width, int height, std::string windowName)
	{
		m_sAppName = windowName;
		m_width = width;
		m_height = height;
		
		// Initalize GLFW and initalize OpenGL to version 3.3
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Depth buffer
		glfwWindowHint(GLFW_DEPTH_BITS, 24);
		
		// Make the window non-resizable
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		// Enable anti-aliasing - MSAA
		//glfwWindowHint(GLFW_SAMPLES, 4);

		// Create a window
		window = glfwCreateWindow(m_width, m_height, m_sAppName.c_str(), NULL, NULL);
		if (window == NULL)
			Error("Failed to create window.");

		// Set window position on screen
		glfwSetWindowPos(window, 360, 75);

		// Make the window to in the current context
		glfwMakeContextCurrent(window);

		// Disable cursor
		glfwSetCursorPos(window, m_width / 2.0f, m_height / 2.0f);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// Disable V-Sync (to achieve 60+ fps)
		// Comment this out to get 60 fps (max)
		glfwSwapInterval(0);

		// Load all function pointers using GLAD
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			Error("Failed to initalize GLAD");

		// Set viewport and callback function when window gets resized 
		glViewport(0, 0, m_width, m_height);

		// Enable z-buffer and enable face-culling
		glEnable(GL_DEPTH_TEST);		  // Enable depth testing
		glDepthFunc(GL_LESS);

		//glEnable(GL_CULL_FACE);         // Enable face culling
		//glCullFace(GL_BACK);            // Cull back faces
		//glFrontFace(GL_CCW);            // Define front faces as counter-clockwise
		
		// Display GPU info
		DisplayGPU();
	}

	void Start()
	{
		m_bIsRunning = true;

		glfwSetWindowUserPointer(window, this);

		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);
		glfwSetMouseButtonCallback(window, mouse_button_callback);

		// Make the window context null before calling the renderer thread
		glfwMakeContextCurrent(nullptr);

		// Start the renderer
		std::thread rendererThread = std::thread(&OpenGL_Graphics::RendererThread, this);

		// While the renderer thread is running, the main thread handles poll events
		while (m_bIsRunning)
		{
			processInput(window);

			if (glfwWindowShouldClose(window))
				m_bIsRunning = false;

			//glfwPollEvents();
			glfwWaitEvents();
		}

		// Wait until the renderer thread exits
		if (rendererThread.joinable())
			rendererThread.join();

		// Cleanup functions
		Destroy();
		glfwDestroyWindow(window);
		glfwTerminate();
	}

protected:
	// Has to be overriden by subclasses
	virtual bool Setup() = 0;
	virtual bool Update(float fElapsedTime) = 0;

	// Optional to override
	virtual void Destroy() { }

// Private functions
private:
	void Error(const std::string& message)
	{
		std::cerr << "Error: " << message << std::endl;
		Destroy();
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	void processInput(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			m_bIsRunning = false;
			glfwSetWindowShouldClose(window, true);
		}
	}

	static void mouse_callback(GLFWwindow* window, double xPos, double yPos)
	{
		OpenGL_Graphics* instance = static_cast<OpenGL_Graphics*>(glfwGetWindowUserPointer(window));
		instance->m_mousePosX = (float)xPos;
		instance->m_mousePosY = (float)yPos;
		instance->bFirstMouse = false;
	}

	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		OpenGL_Graphics* instance = static_cast<OpenGL_Graphics*>(glfwGetWindowUserPointer(window));
		if ((int)yoffset == 1)
			instance->m_mouseScroll = (int)Mouse::SCROLL_UP;
		else if ((int)yoffset == -1)
			instance->m_mouseScroll = (int)Mouse::SCROLL_DOWN;
		else
			instance->m_mouseScroll = 0;
	}

	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{
		OpenGL_Graphics* instance = static_cast<OpenGL_Graphics*>(glfwGetWindowUserPointer(window));
		instance->m_bMouseButtonHeld[button] = action;
	}

	void DisplayGPU()
	{
		const unsigned char* renderer = glGetString(GL_RENDERER);
		const unsigned char* vendor = glGetString(GL_VENDOR);
		const unsigned char* version = glGetString(GL_VERSION);
		const unsigned char* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

		std::cout << "---------- GPU information ----------\n";
		std::cout << std::endl;
		std::cout << "Renderer: " << renderer << '\n';
		std::cout << "Vendor: " << vendor << '\n';
		std::cout << "OpenGL Version: " << version << '\n';
		std::cout << "GLSL Version: " << glslVersion << '\n';
		std::cout << std::endl;
		std::cout << "-------------------------------------\n\n";
	}
};

std::atomic<bool> OpenGL_Graphics::m_bIsRunning(false);
