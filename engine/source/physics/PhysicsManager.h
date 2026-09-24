#pragma once

#include <memory>

class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btGhostPairCallback;

namespace eng
{
    class RigidBody;

    class PhysicsManager
    {
        public:
            static constexpr float FixedTimeStep = 1.0f / 60.0f;
            static constexpr int MaxSubSteps = 4;

            PhysicsManager();
            ~PhysicsManager();

            void Init();
            void Update(float deltaTime);

            void AddRigidBody(RigidBody* body);
            void RemoveRigidBody(RigidBody* body);

            float GetFixedTimeStep() const { return FixedTimeStep; }
            btDiscreteDynamicsWorld* GetWorld();

        private:
            std::unique_ptr<btGhostPairCallback> m_ghostPairCallback;

            std::unique_ptr<btBroadphaseInterface> m_broadphase;
            std::unique_ptr<btDefaultCollisionConfiguration> m_collisionConfig;
            std::unique_ptr<btCollisionDispatcher> m_dispatcher;
            std::unique_ptr<btSequentialImpulseConstraintSolver> m_solver;
            std::unique_ptr<btDiscreteDynamicsWorld> m_world;

    };
}