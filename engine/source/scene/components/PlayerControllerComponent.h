#pragma once

#include "scene/Component.h"
#include "physics/KinematicCharacterController.h"
#include <memory>

namespace eng
{
    class PlayerControllerComponent : public Component
    {
        COMPONENT(PlayerControllerComponent)

        public:
            void Init() override;
            void Update(float deltaTime) override;

            bool OnGround() const;
            bool HasJumpedThisFrame() const { return m_jumpedThisFrame; }

            void StopMoving();

            void SetMoveSpeed(float metersPerSecond) { m_moveSpeed = metersPerSecond; }
            void SetSensitivity(float degreesPerPixel) { m_sensitivity = degreesPerPixel; }

        private:
            float m_sensitivity = 0.075f;

            float m_moveSpeed = 30.0f;

            float m_jumpSpeed = 5.0f;

            float m_xRot = 0.0f;   // pitch (gradi)
            float m_yRot = 0.0f;   // yaw (gradi)
            bool m_jumpedThisFrame = false;

            std::unique_ptr<KinematicCharacterController> m_kinematicController;
    };
}
