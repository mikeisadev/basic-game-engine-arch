#include "TestObject.h"
#include <GLFW/glfw3.h>
#include <iostream>

TestObject::TestObject()
{
    std::string vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 position;
        layout (location = 1) in vec3 color;

        out vec3 vColor;

        uniform vec2 uOffset;

        void main()
        {
            vColor = color;
            gl_Position = vec4(position.x + uOffset.x, position.y + uOffset.y, position.z, 1.0);
        }
    )";

    std::string fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;

        in vec3 vColor;

        void main()
        {
            FragColor = vec4(vColor, 1.0);
        }
    )";

    auto& graphicsAPI = eng::Engine::GetInstance().GetGraphicsAPI();
    auto shaderProgram = graphicsAPI.CreateShaderProgram(vertexShaderSource, fragmentShaderSource);

    m_material.SetShaderProgram(shaderProgram);

    std::vector<float> vertices =
    {
        0.5f, 0.5f, 0.0f,       1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,      0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.0f,      1.0f, 1.0f, 0.0f
    };

    std::vector<unsigned int> indices =
    {
        0, 1, 2,
        0, 2, 3
    };

    eng::VertexLayout vertexLayout;

    // Position
    vertexLayout.elements.push_back({
        0,
        3,
        GL_FLOAT,
        0
    });

    // Color
    vertexLayout.elements.push_back({
        1,
        3,
        GL_FLOAT,
        sizeof(float) * 3
    });

    vertexLayout.stride = sizeof(float) * 6;

    // Once having vertexLayout, we can create a mesh
    m_mesh = std::make_shared<eng::Mesh>(vertexLayout, vertices, indices);
}

void TestObject::Update(float deltaTime)
{
    eng::GameObject::Update(deltaTime);

    auto& input = eng::Engine::GetInstance().GetInputManager();

    // HORIZONTAL MOVEMENT (X)
    if (input.IsKeyPressed(GLFW_KEY_A))
    {
        m_offsetX -= 0.005f;
    }
    else if (input.IsKeyPressed(GLFW_KEY_D))
    {
        m_offsetX += 0.005f;
    }
    
    // VERTICAL MOVEMENT (Y)
    if (input.IsKeyPressed(GLFW_KEY_W))
    {
        m_offsetY += 0.005f;
    }
    else if (input.IsKeyPressed(GLFW_KEY_S))
    {
        m_offsetY -= 0.005f;
    }

    m_material.SetParam("uOffset", m_offsetX, m_offsetY);

    eng::RenderCommand command;
    command.material = &m_material;
    command.mesh = m_mesh.get();

    auto& renderQueue = eng::Engine::GetInstance().GetRenderQueue();
    renderQueue.Submit(command);
}