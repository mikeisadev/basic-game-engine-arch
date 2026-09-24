#include "scene/components/PlayerControllerComponent.h"
#include "input/InputManager.h"
#include "Engine.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/quaternion.hpp>
#include <algorithm>

namespace eng
{
    void PlayerControllerComponent::Init()
    {
        m_kinematicController = std::make_unique<KinematicCharacterController>(
            0.4f, 1.2f, m_owner->GetWorldPosition());
    }

    void PlayerControllerComponent::Update(float deltaTime)
    {
        (void)deltaTime; 

        auto& engine = Engine::GetInstance();
        auto& input = engine.GetInputManager();

        m_jumpedThisFrame = false;

        if (input.IsMousePositionChanged())
        {
            const glm::vec2 mouseDelta = input.GetMouseDelta();

            m_yRot -= mouseDelta.x * m_sensitivity;
            m_xRot -= mouseDelta.y * m_sensitivity;
            m_xRot = std::clamp(m_xRot, -89.0f, 89.0f);

            const glm::quat yawRot   = glm::angleAxis(glm::radians(m_yRot), glm::vec3(0.0f, 1.0f, 0.0f));
            const glm::quat pitchRot = glm::angleAxis(glm::radians(m_xRot), glm::vec3(1.0f, 0.0f, 0.0f));

            m_owner->SetRotation(glm::normalize(yawRot * pitchRot));
        }

        const glm::quat yawOnly = glm::angleAxis(glm::radians(m_yRot), glm::vec3(0.0f, 1.0f, 0.0f));
        const glm::vec3 front = yawOnly * glm::vec3(0.0f, 0.0f, -1.0f);
        const glm::vec3 right = yawOnly * glm::vec3(1.0f, 0.0f, 0.0f);

        glm::vec3 move(0.0f);

        if (input.IsKeyPressed(GLFW_KEY_A)) move -= right;
        if (input.IsKeyPressed(GLFW_KEY_D)) move += right;
        if (input.IsKeyPressed(GLFW_KEY_W)) move += front;
        if (input.IsKeyPressed(GLFW_KEY_S)) move -= front;

        if (glm::dot(move, move) > 0.0f)
        {
            move = glm::normalize(move);
        }

        const float fixedStep = engine.GetPhysicsManager().GetFixedTimeStep();
        m_kinematicController->Walk(move * m_moveSpeed * fixedStep);

        if (input.WasKeyPressed(GLFW_KEY_SPACE))
        {
            m_jumpedThisFrame = m_kinematicController->Jump(glm::vec3(0.0f, m_jumpSpeed, 0.0f));
        }

        m_owner->SetPosition(m_kinematicController->GetPosition());
    }

    void PlayerControllerComponent::StopMoving()
    {
        if (m_kinematicController)
        {
            m_kinematicController->Stop();
        }
    }

    bool PlayerControllerComponent::OnGround() const
    {
        return m_kinematicController ? m_kinematicController->OnGround() : false;
    }
}
