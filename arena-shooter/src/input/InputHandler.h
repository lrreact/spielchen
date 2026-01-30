#pragma once

#include "raylib.h"
#include "game/Player.h"

namespace arena {

class InputHandler {
public:
    InputHandler();
    ~InputHandler();

    // Configuration
    void setMouseSensitivity(float sensitivity) { m_mouseSensitivity = sensitivity; }
    float getMouseSensitivity() const { return m_mouseSensitivity; }

    void setInvertY(bool invert) { m_invertY = invert; }
    bool getInvertY() const { return m_invertY; }

    // Input gathering
    PlayerInput gatherInput();

    // Input state queries
    bool isKeyPressed(int key) const;
    bool isKeyDown(int key) const;
    bool isMouseButtonPressed(int button) const;
    bool isMouseButtonDown(int button) const;

    // Cursor control
    void lockCursor();
    void unlockCursor();
    bool isCursorLocked() const { return m_cursorLocked; }

private:
    float m_mouseSensitivity;
    bool m_invertY;
    bool m_cursorLocked;

    uint32_t m_inputSequence;
    Vector2 m_lastMousePos;
};

} // namespace arena
