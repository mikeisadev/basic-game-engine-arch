#include "Player.h"
#include "Bullet.h"
#include <GLFW/glfw3.h>

void Player::Init()
{
    if (auto bullet = FindChildByName("bullet_33"))
    {
        bullet->SetActive(false);
    }

    if (auto fire = FindChildByName("BOOM_35"))
    {
        fire->SetActive(false);
    }

    if (auto gun = FindChildByName("Gun"))
    {
        m_animationComponent = gun->GetComponent<eng::AnimationComponent>();
    }

    m_audioComponent = GetComponent<eng::AudioComponent>();
    m_playerControllerComponent = GetComponent<eng::PlayerControllerComponent>();

    m_bulletMaterial = eng::Material::Load("materials/suzanne.mat");
    m_bulletMesh = eng::Mesh::CreateSphere(0.2f, 16, 16);
}

void Player::Update(float deltaTime) 
{
    eng::GameObject::Update(deltaTime);

    auto& input = eng::Engine::GetInstance().GetInputManager();

    if (input.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
    {
        Shoot();
    }

    if (m_playerControllerComponent && m_playerControllerComponent->HasJumpedThisFrame())
    {
        if (m_audioComponent)
        {
            if (m_audioComponent->IsPlaying("jump"))
            {
                m_audioComponent->Stop("jump");
            }
            m_audioComponent->Play("jump");
        }
    }

    const bool walking = 
        input.IsKeyPressed(GLFW_KEY_W) ||
        input.IsKeyPressed(GLFW_KEY_A) ||
        input.IsKeyPressed(GLFW_KEY_S) ||
        input.IsKeyPressed(GLFW_KEY_D);

    const bool onGround = m_playerControllerComponent && m_playerControllerComponent->OnGround();

    if (m_audioComponent)
    {
        if (walking && onGround)
        {
            if (!m_audioComponent->IsPlaying("step"))
            {
                m_audioComponent->Play("step", true);
            }
        }
        else if (m_audioComponent->IsPlaying("step"))
        {
            m_audioComponent->Stop("step");
        }
    }
}

void Player::Shoot()
{
    if (!m_animationComponent || m_animationComponent->IsPlaying())
    {
        return;
    }

    m_animationComponent->Play("shoot", false);

    if (m_audioComponent)
    {
        if (m_audioComponent->IsPlaying("shoot"))
        {
            m_audioComponent->Stop("shoot");
        }
        m_audioComponent->Play("shoot");
    }

    auto bullet = m_scene->CreateObject<Bullet>("Bullet", GetParent());
    bullet->AddComponent(new eng::MeshComponent(m_bulletMaterial, m_bulletMesh));

    glm::vec3 spawnPos = GetWorldPosition();

    if (auto muzzle = FindChildByName("BOOM_35"))
    {
        spawnPos = muzzle->GetWorldPosition();
    }

    bullet->SetPosition(spawnPos + m_rotation * glm::vec3(-0.2f, 0.2f, -1.75f));

    auto collider = std::make_shared<eng::SphereCollider>(0.2f);
    auto rigidBody = std::make_shared<eng::RigidBody>(eng::BodyType::Dynamic, collider, 10.0f, 0.1f);
    bullet->AddComponent(new eng::PhysicsComponent(rigidBody));

    const glm::vec3 front = m_rotation * glm::vec3(0.0f, 0.0f, -1.0f);
    rigidBody->ApplyImpulse(front * 500.0f);
}