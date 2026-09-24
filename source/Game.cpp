#include "Game.h"
#include "TestObject.h"
#include "Player.h"
#include "Bullet.h"
#include "JumpPlatform.h"

#include <GLFW/glfw3.h>

#include <iostream>

void Game::RegisterTypes()
{
    Player::Register();
    Bullet::Register();
    JumpPlatform::Register();
}

bool Game::Init() 
{
    auto scene = eng::Scene::Load("scenes/scene.sc");
    m_scene = scene;

    if (!m_scene)
    {
        std::cerr << "[Game] Scena non trovata, vuota o con formattazione errata" << std::endl;
        return false;
    }

    auto& engine = eng::Engine::GetInstance();
    engine.SetScene(m_scene);

    m_3DRoot = m_scene->FindObjectByName("3DRoot");

    if (m_3DRoot)
    {
        m_3DRoot->SetActive(false);
    }

    auto canvasComponent = engine.GetUIInputSystem().GetCanvas();

    if (!canvasComponent)
    {
        std::cerr << "[Game] La scena non ha un canvas attivo: controlla 'activeCanvas' in scene.sc" << std::endl;
        return false;
    }

    canvasComponent->SetActive(true);
    engine.SetCursorEnabled(true);
    engine.GetUIInputSystem().SetActive(true);

    if (auto button = canvasComponent->GetOwner()->FindChildByName("PlayButton"))
    {
        if (auto component = button->GetComponent<eng::ButtonComponent>())
        {
            component->onClick = [this]()
                {
                    auto& engine = eng::Engine::GetInstance();
                    engine.GetUIInputSystem().GetCanvas()->SetActive(false);
                    engine.SetCursorEnabled(false);

                    if (m_3DRoot)
                    {
                        m_3DRoot->SetActive(true);
                    }
                };
        }
    }

    if (auto button = canvasComponent->GetOwner()->FindChildByName("QuitButton"))
    {
        if (auto component = button->GetComponent<eng::ButtonComponent>())
        {
            component->onClick = [this]()
                {
                    SetNeedsToBeClosed(true);
                };
        }
    }

    eng::Engine::GetInstance().GetGraphicsAPI().SetClearColor(
        117.0f / 256.0f, 187.0f / 256.0f, 253.0f / 256.0f, 1.0f
    );

    return true;
}

void Game::Update(float deltaTime) 
{
    m_scene->Update(deltaTime);

    auto& engine = eng::Engine::GetInstance();

    if (engine.GetInputManager().WasKeyPressed(GLFW_KEY_ESCAPE))
    {
        if (m_3DRoot && m_3DRoot->IsActive())
        {
            if (auto player = m_scene->FindObjectByName("MainPlayer"))
            {
                if (auto controller = player->GetComponent<eng::PlayerControllerComponent>())
                {
                    controller->StopMoving();
                }
            }

            engine.GetUIInputSystem().GetCanvas()->SetActive(true);
            engine.SetCursorEnabled(true);
            m_3DRoot->SetActive(false);
        }
    }
}

void Game::Destroy()
{
    m_3DRoot = nullptr;
    m_scene.reset();
}