#include "physics/PhysicsManager.h"
#include "physics/RigidBody.h"
#include "physics/CollisionObject.h"

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

namespace eng
{
    PhysicsManager::PhysicsManager() 
    {

    }

    PhysicsManager::~PhysicsManager()
    {
        
    }

    void PhysicsManager::Init()
    {
        m_broadphase = std::make_unique<btDbvtBroadphase>();
        m_collisionConfig = std::make_unique<btDefaultCollisionConfiguration>();
        m_dispatcher = std::make_unique<btCollisionDispatcher>(m_collisionConfig.get());
        m_solver = std::make_unique<btSequentialImpulseConstraintSolver>();
        m_world = std::make_unique<btDiscreteDynamicsWorld>(
            m_dispatcher.get(), m_broadphase.get(), m_solver.get(), m_collisionConfig.get()
        );

        m_world->setGravity(btVector3(0, -9.81f, 0));

        m_ghostPairCallback = std::make_unique<btGhostPairCallback>();
        m_broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(m_ghostPairCallback.get());
    }

    void PhysicsManager::Update(float deltaTime)
    {
        m_world->stepSimulation(deltaTime, MaxSubSteps, FixedTimeStep);

        // Process collisions
        auto dispatcher = m_world->getDispatcher();
        const auto numManifolds = dispatcher->getNumManifolds();

        for (int i = 0; i < numManifolds; ++i)
        {
            auto manifold = dispatcher->getManifoldByIndexInternal(i);
            if (!manifold)
            {
                continue;
            }

            auto bodyA = reinterpret_cast<CollisionObject*>(manifold->getBody0()->getUserPointer());
            auto bodyB = reinterpret_cast<CollisionObject*>(manifold->getBody1()->getUserPointer());

            if (!bodyA || !bodyB)
            {
                continue;
            }

            const auto numContacts = manifold->getNumContacts();
            for (int j = 0; j < numContacts; ++j)
            {
                const auto& point = manifold->getContactPoint(j);

                const glm::vec3 pos(
                    point.m_positionWorldOnB.x(),
                    point.m_positionWorldOnB.y(),
                    point.m_positionWorldOnB.z());

                const glm::vec3 norm(
                    point.m_normalWorldOnB.x(),
                    point.m_normalWorldOnB.y(),
                    point.m_normalWorldOnB.z());

                bodyA->DispatchContactEvent(bodyB, pos, norm);
                bodyB->DispatchContactEvent(bodyA, pos, norm);
            }
        }
    }

    void PhysicsManager::AddRigidBody(RigidBody* body)
    {
        if (!body || !m_world)
        {
            return;
        }

        auto rigidBody = body->GetBody();
        if (!rigidBody)
        {
            return;
        }

        int group = btBroadphaseProxy::DefaultFilter;

        switch (body->GetType())
        {
            case BodyType::Static:    group = btBroadphaseProxy::StaticFilter;    break;
            case BodyType::Kinematic: group = btBroadphaseProxy::KinematicFilter; break;
            case BodyType::Dynamic:   group = btBroadphaseProxy::DefaultFilter;   break;
        }

        m_world->addRigidBody(rigidBody, group, btBroadphaseProxy::AllFilter);
        body->SetAddedToWorld(true);
    }

    void PhysicsManager::RemoveRigidBody(RigidBody* body)
    {
        if (!body || !m_world)
        {
            return;
        }

        if (auto rigidBody = body->GetBody())
        {
            m_world->removeRigidBody(rigidBody);
            body->SetAddedToWorld(false);
        }
    }

    btDiscreteDynamicsWorld* PhysicsManager::GetWorld()
    {
        return m_world.get();
    }
}