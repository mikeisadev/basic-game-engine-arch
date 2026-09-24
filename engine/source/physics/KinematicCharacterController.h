#pragma once

#include "physics/CollisionObject.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>

class btCapsuleShape;
class btPairCachingGhostObject;
class btKinematicCharacterController;

namespace eng
{
    class KinematicCharacterController : public CollisionObject
    {
        public:
            KinematicCharacterController(float radius, float height, const glm::vec3& position);
            ~KinematicCharacterController();

            glm::vec3 GetPosition() const;
            glm::quat GetRotation() const;

            void Walk(const glm::vec3& displacementPerStep);

            void Stop();

            bool Jump(const glm::vec3& velocity);
            bool OnGround() const;

            void SetEyeOffset(float offset) { m_eyeOffset = offset; }
            float GetEyeOffset() const { return m_eyeOffset; }
        
        private:
            float m_height = 1.2f;
            float m_radius = 0.4f;

            float m_eyeOffset = 2.1f;

            std::unique_ptr<btCapsuleShape> m_shape;
            std::unique_ptr<btPairCachingGhostObject> m_ghost;
            std::unique_ptr<btKinematicCharacterController> m_controller;
    };
}