#include "input/InputManager.h"

namespace eng
{
    namespace
    {
        bool InRange(int index, int size)
        {
            return index >= 0 && index < size;
        }
    }

    // ---------------- Tastiera ----------------

    void InputManager::SetKeyPressed(int key, bool pressed)
    {
        if (!InRange(key, MaxKeys))
        {
            return;
        }

        // I flag "just" si accendono solo sul cambio di stato: GLFW_REPEAT non li tocca.
        if (pressed && !m_keys[key])
        {
            m_keysJustPressed[key] = true;
        }
        else if (!pressed && m_keys[key])
        {
            m_keysJustReleased[key] = true;
        }

        m_keys[key] = pressed;
    }

    bool InputManager::IsKeyPressed(int key) const
    {
        return InRange(key, MaxKeys) && m_keys[key];
    }

    bool InputManager::WasKeyPressed(int key) const
    {
        return InRange(key, MaxKeys) && m_keysJustPressed[key];
    }

    bool InputManager::WasKeyReleased(int key) const
    {
        return InRange(key, MaxKeys) && m_keysJustReleased[key];
    }

    // ---------------- Mouse ----------------

    void InputManager::SetMouseButtonPressed(int button, bool pressed)
    {
        if (!InRange(button, MaxMouseButtons))
        {
            return;
        }

        if (pressed && !m_mouseKeys[button])
        {
            m_mouseKeysJustPressed[button] = true;
        }
        else if (!pressed && m_mouseKeys[button])
        {
            m_mouseKeysJustReleased[button] = true;
        }

        m_mouseKeys[button] = pressed;
    }

    bool InputManager::IsMouseButtonPressed(int button) const
    {
        return InRange(button, MaxMouseButtons) && m_mouseKeys[button];
    }

    bool InputManager::WasMouseButtonPressed(int button) const
    {
        return InRange(button, MaxMouseButtons) && m_mouseKeysJustPressed[button];
    }

    bool InputManager::WasMouseButtonReleased(int button) const
    {
        return InRange(button, MaxMouseButtons) && m_mouseKeysJustReleased[button];
    }

    void InputManager::SetMousePositionCurrent(const glm::vec2& pos)
    {
        // Il primo evento (o il primo dopo ResetMouse) inizializza soltanto la posizione:
        // niente delta calcolato da (0,0) o dalla posizione del cursore nel menu.
        if (m_firstMouseEvent)
        {
            m_mousePositionCurrent = pos;
            m_firstMouseEvent = false;
            return;
        }

        // Accumulo: più eventi nello stesso frame non si perdono.
        m_mouseDelta += pos - m_mousePositionCurrent;
        m_mousePositionCurrent = pos;
    }

    const glm::vec2& InputManager::GetMousePositionCurrent() const
    {
        return m_mousePositionCurrent;
    }

    const glm::vec2& InputManager::GetMouseDelta() const
    {
        return m_mouseDelta;
    }

    bool InputManager::IsMousePositionChanged() const
    {
        return m_mouseDelta.x != 0.0f || m_mouseDelta.y != 0.0f;
    }

    // ---------------- Gestione stati ----------------

    void InputManager::ResetMouse()
    {
        m_mouseDelta = glm::vec2(0.0f);
        m_firstMouseEvent = true;
    }

    void InputManager::ReleaseAll()
    {
        m_keys.fill(false);
        m_keysJustPressed.fill(false);
        m_keysJustReleased.fill(false);
        m_mouseKeys.fill(false);
        m_mouseKeysJustPressed.fill(false);
        m_mouseKeysJustReleased.fill(false);
        ResetMouse();
    }

    void InputManager::ClearStates()
    {
        m_keysJustPressed.fill(false);
        m_keysJustReleased.fill(false);
        m_mouseKeysJustPressed.fill(false);
        m_mouseKeysJustReleased.fill(false);
        m_mouseDelta = glm::vec2(0.0f);
    }
}
