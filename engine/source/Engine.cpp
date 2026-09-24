#include "Engine.h"
#include "Application.h"
#include "scene/GameObject.h"
#include "scene/Component.h"
#include "scene/components/CameraComponent.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace eng
{
    void keyCallback(GLFWwindow* window, int key, int, int action, int)
    {
        auto& inputManager = eng::Engine::GetInstance().GetInputManager();
        
        if (action == GLFW_PRESS)
        {
            inputManager.SetKeyPressed(key, true);
        }
        else if (action == GLFW_RELEASE)
        {
            inputManager.SetKeyPressed(key, false);
        }
    }

    void mouseButtonCallback(GLFWwindow* window, int button, int action, int)
    {
        auto& inputManager = eng::Engine::GetInstance().GetInputManager();

        if (action == GLFW_PRESS)
        {
            inputManager.SetMouseButtonPressed(button, true);
            inputManager.SetMouseButtonWasPressed(button, true);
        }
        else if (action == GLFW_RELEASE)
        {
            inputManager.SetMouseButtonPressed(button, false);
            inputManager.SetMouseButtonWasReleased(button, true);
        }
    }

    void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
    {
        auto& inputManager = eng::Engine::GetInstance().GetInputManager();
     
        inputManager.SetMousePositionOld(inputManager.GetMousePositionCurrent());

        glm::vec2 currentPos(static_cast<float>(xpos), static_cast<float>(ypos));
        inputManager.SetMousePositionCurrent(currentPos);

        inputManager.SetMousePositionChanged(true);
    }

    // This is triggered when the logic dimension of the window changes (points)
    void windowSizeCallback(GLFWwindow* window, int width, int height)
    {
        auto& engine = eng::Engine::GetInstance();
        engine.m_windowSize = glm::ivec2(width, height);
    }

    // This is triggered when the framebuffer changes (pixels): resize, position change between
    // monitors with different scale, resolution change.
    // This is the only function, beyond Init, that should touch the GL viewport.
    void frameBufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        eng::Engine::GetInstance().GetGraphicsAPI().SetViewport(0, 0, width, height);
    }

    Engine& Engine::GetInstance() 
    {
        static Engine instance;
        return instance;
    }

    bool Engine::Init(int width, int height) 
    {
        if (!m_application)
        {
            return false;
        }

        Scene::RegisterTypes();
        m_application->RegisterTypes();

        if (!glfwInit())
        {
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_window = glfwCreateWindow(width, height, "GameEngine", nullptr, nullptr);

        if (m_window == nullptr)
        {
            std::cout << "Error creating window" << std::endl;
            glfwTerminate();
            return false;
        }

        glfwSetKeyCallback(m_window, keyCallback);
        glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
        glfwSetCursorPosCallback(m_window, cursorPositionCallback);
        glfwSetWindowSizeCallback(m_window, windowSizeCallback);
        glfwSetFramebufferSizeCallback(m_window, frameBufferSizeCallback);

        glfwMakeContextCurrent(m_window);

        if (glewInit() != GLEW_OK)
        {
            glfwTerminate();
            return false;
        }

        // Logic dimension (points): can change from width/height, if system locked the window it must be re-readed
        glfwGetWindowSize(m_window, &m_windowSize.x, &m_windowSize.y);

        // Dimension of the framebuffer (pixel). Especially on Retina screens (like the macbook) is the double of points.
        int framebufferWidth = 0;
        int framebufferHeight = 0;
        glfwGetFramebufferSize(m_window, &framebufferWidth, &framebufferHeight);

        m_graphicsAPI.Init();
        m_graphicsAPI.SetViewport(0, 0, framebufferWidth, framebufferHeight);
        m_physicsManager.Init();
        m_audioManager.Init();
        m_renderQueue.Init();
        m_fontManager.Init();
        return m_application->Init();
    }

    // Here the main game loop will live
    void Engine::Run() 
    {
        if (!m_application)
        {
            return;
        }

        m_lastTimePoint = std::chrono::high_resolution_clock::now();

        while(!glfwWindowShouldClose(m_window) && !m_application->NeedsToBeClosed())
        {
            // process events
            glfwPollEvents();

            // update application logic
            auto now = std::chrono::high_resolution_clock::now();
            float deltaTime = std::chrono::duration<float>(now - m_lastTimePoint).count();
            m_lastTimePoint = now;

            m_physicsManager.Update(deltaTime);

            if (m_uiInputSystem.IsActive())
            {
                m_uiInputSystem.Update(deltaTime);
            }

            m_application->Update(deltaTime);

            m_graphicsAPI.ClearBuffers();

            // Collect current active camera matrices
            CameraData cameraData;
            std::vector<LightData> lights;

            // Get window size
            const int width = m_windowSize.x;
            const int height = m_windowSize.y;
            float aspect = static_cast<float>(width) / static_cast<float>(height);

            if (m_currentScene)
            {
                if (auto cameraObject = m_currentScene->GetMainCamera())
                {
                    // logic for getting camera matrices
                    auto cameraComponent = cameraObject->GetComponent<CameraComponent>();

                    if (cameraComponent)
                    {
                        cameraData.viewMatrix = cameraComponent->GetViewMatrix();
                        cameraData.projectionMatrix = cameraComponent->GetProjectionMatrix(aspect);
                        cameraData.orthoMatrix = glm::ortho(
                            0.0f, static_cast<float>(width),
                            0.0f, static_cast<float>(height)
                        );
                        cameraData.position = cameraObject->GetWorldPosition();
                    }
                }

                lights = m_currentScene->CollectLights();
            }

            // Rendering calls
            m_renderQueue.Draw(m_graphicsAPI, cameraData, lights);

            // handle some rendering (only buffer swaps)
            glfwSwapBuffers(m_window);

            m_inputManager.ClearStates();
        }

        m_application.reset(nullptr);
    }

    void Engine::Destroy()
    {
        if (m_application)
        {
            m_application->Destroy();
            m_application.reset();
            glfwTerminate();
            m_window = nullptr;
        }
    }

    void Engine::SetCursorEnabled(bool enabled)
    {
        glfwSetInputMode(m_window, GLFW_CURSOR, enabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }

    const glm::ivec2& Engine::GetWindowSize() const
    {
        return m_windowSize;
    }

    void Engine::SetApplication(Application* app)
    {
        m_application.reset(app);
    }

    Application* Engine::GetApplication()
    {
        return m_application.get();
    }

    InputManager& Engine::GetInputManager()
    {
        return m_inputManager;
    }

    GraphicsAPI& Engine::GetGraphicsAPI()
    {
        return m_graphicsAPI;
    }

    RenderQueue& Engine::GetRenderQueue()
    {
        return m_renderQueue;
    }

    FileSystem& Engine::GetFileSystem() 
    {
        return m_fileSystem;
    }

    TextureManager& Engine::GetTextureManager()
    {
        return m_textureManager;
    }

    PhysicsManager& Engine::GetPhysicsManager()
    {
        return m_physicsManager;
    }

    AudioManager& Engine::GetAudioManager()
    {
        return m_audioManager;
    }

    FontManager& Engine::GetFontManager()
    {
        return m_fontManager;
    }

    UIInputSystem& Engine::GetUIInputSystem()
    {
        return m_uiInputSystem;
    }

    void Engine::SetScene(const std::shared_ptr<Scene>& scene)
    {
        m_currentScene = scene;
    }

    const std::shared_ptr<Scene>& Engine::GetScene() const
    {
        return m_currentScene;
    }
}