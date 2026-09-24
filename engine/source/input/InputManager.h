#pragma once
#include <array>
#include "glm/vec2.hpp"

namespace eng
{
    class InputManager
    {
        private:
            InputManager() = default;
            InputManager(const InputManager&) = delete;
            InputManager(InputManager&&) = delete;
            InputManager& operator=(const InputManager&) = delete;
            InputManager& operator=(InputManager&&) = delete;

        public:
            // GLFW_KEY_LAST vale 348, GLFW_MOUSE_BUTTON_LAST vale 7.
            static constexpr int MaxKeys = 512;
            static constexpr int MaxMouseButtons = 16;

            // --- Tastiera ---
            bool IsKeyPressed(int key) const;     // vero finché il tasto è tenuto
            bool WasKeyPressed(int key) const;    // vero solo nel frame della pressione
            bool WasKeyReleased(int key) const;   // vero solo nel frame del rilascio

            // --- Mouse ---
            bool IsMouseButtonPressed(int button) const;
            bool WasMouseButtonPressed(int button) const;
            bool WasMouseButtonReleased(int button) const;

            const glm::vec2& GetMousePositionCurrent() const;
            const glm::vec2& GetMouseDelta() const;   // spostamento accumulato nel frame
            bool IsMousePositionChanged() const;      // delta diverso da zero

            // --- Chiamati dall'Engine ---
            void SetKeyPressed(int key, bool pressed);            // keyCallback
            void SetMouseButtonPressed(int button, bool pressed); // mouseButtonCallback
            void SetMousePositionCurrent(const glm::vec2& pos);   // cursorPositionCallback
            void ResetMouse();     // dopo un cambio di modalità del cursore
            void ReleaseAll();     // quando la finestra perde il focus
            void ClearStates();    // a fine frame

        private:
            std::array<bool, MaxKeys> m_keys = {};
            std::array<bool, MaxKeys> m_keysJustPressed = {};
            std::array<bool, MaxKeys> m_keysJustReleased = {};

            std::array<bool, MaxMouseButtons> m_mouseKeys = {};
            std::array<bool, MaxMouseButtons> m_mouseKeysJustPressed = {};
            std::array<bool, MaxMouseButtons> m_mouseKeysJustReleased = {};

            glm::vec2 m_mousePositionCurrent = glm::vec2(0.0f);
            glm::vec2 m_mouseDelta = glm::vec2(0.0f);
            bool m_firstMouseEvent = true;

            friend class Engine;
    };
}
