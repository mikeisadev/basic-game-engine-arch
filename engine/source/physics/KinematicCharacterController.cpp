#include "physics/KinematicCharacterController.h"
#include "Engine.h"

#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

namespace eng
{
    KinematicCharacterController::KinematicCharacterController(float radius, float height, const glm::vec3& position)
        : m_radius(radius), m_height(height)
    {
        m_collisionObjectType = CollisionObjectType::KinematicCharacterController;
        m_eyeOffset = m_height + 0.5f + m_radius;

        auto world = Engine::GetInstance().GetPhysicsManager().GetWorld();

        m_shape = std::make_unique<btCapsuleShape>(m_radius, m_height);

        m_ghost = std::make_unique<btPairCachingGhostObject>();
        btTransform start;
        start.setIdentity();
        start.setOrigin(btVector3(position.x, position.y, position.z));
        m_ghost->setWorldTransform(start);
        m_ghost->setCollisionShape(m_shape.get());
        m_ghost->setCollisionFlags(m_ghost->getCollisionFlags() | btCollisionObject::CF_CHARACTER_OBJECT);
        m_ghost->setUserPointer(this);

        const btScalar stepHeight = 0.35f; // how high we can step up
        m_controller = std::make_unique<btKinematicCharacterController>(m_ghost.get(), m_shape.get(), stepHeight);

        m_controller->setMaxSlope(btRadians(50.0f));
        m_controller->setGravity(world->getGravity()); // negative value is fine

        world->addCollisionObject(
            m_ghost.get(),
            btBroadphaseProxy::CharacterFilter,
            btBroadphaseProxy::AllFilter & ~btBroadphaseProxy::SensorTrigger); // collide with most things
        world->addAction(m_controller.get());
    }

    KinematicCharacterController::~KinematicCharacterController()
    {
        auto world = Engine::GetInstance().GetPhysicsManager().GetWorld();

        if (world)
        {
            if (m_controller)
            {
                world->removeAction(m_controller.get());
            }

            if (m_ghost)
            {
                world->removeCollisionObject(m_ghost.get());
            }
        }
    }

    glm::vec3 KinematicCharacterController::GetPosition() const
    {
        const auto& pos = m_ghost->getWorldTransform().getOrigin();
        return glm::vec3(pos.x(), pos.y() + m_eyeOffset, pos.z());
    }
    glm::quat KinematicCharacterController::GetRotation() const
    {
        const auto& rot = m_ghost->getWorldTransform().getRotation();
        return glm::quat(rot.w(), rot.x(), rot.y(), rot.z());
    }

    void KinematicCharacterController::Walk(const glm::vec3& displacementPerStep)
    {
        m_controller->setWalkDirection(btVector3(
            btScalar(displacementPerStep.x),
            btScalar(displacementPerStep.y),
            btScalar(displacementPerStep.z)));
    }

    void KinematicCharacterController::Stop()
    {
        m_controller->setWalkDirection(btVector3(0, 0, 0));
    }

    bool KinematicCharacterController::Jump(const glm::vec3& velocity)
    {
        if (!m_controller->onGround())
        {
            return false;
        }

        m_controller->jump(btVector3(
            btScalar(velocity.x),
            btScalar(velocity.y),
            btScalar(velocity.z)));
        return true;
    }
    
    bool KinematicCharacterController::OnGround() const
    {
        return m_controller->onGround();
    }
}