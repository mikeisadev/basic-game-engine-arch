#pragma once
#include <eng.h>
#include <memory>

class Player : public eng::GameObject
{
    GAMEOBJECT(Player)
    public:
        void Init() override;
        void Update(float deltaTime) override;

    private:
        void Shoot();

        eng::AnimationComponent* m_animationComponent = nullptr;
        eng::AudioComponent* m_audioComponent = nullptr;
        eng::PlayerControllerComponent* m_playerControllerComponent = nullptr;

        std::shared_ptr<eng::Material> m_bulletMaterial;
        std::shared_ptr<eng::Mesh> m_bulletMesh;
};