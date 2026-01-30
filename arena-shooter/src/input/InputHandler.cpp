#include "InputHandler.h"

namespace arena {

InputHandler::InputHandler()
    : m_mouseSensitivity(0.1f)
    , m_invertY(false)
    , m_cursorLocked(false)
    , m_inputSequence(0)
    , m_lastMousePos({ 0, 0 })
{
}

InputHandler::~InputHandler() = default;

PlayerInput InputHandler::gatherInput()
{
    PlayerInput input;
    input.moveDirection = { 0.0f, 0.0f, 0.0f };
    input.lookDelta = { 0.0f, 0.0f };
    input.jump = false;
    input.sprint = false;
    input.primaryFire = false;
    input.secondaryFire = false;
    input.reload = false;
    input.weaponSlot = 0;
    input.sequence = ++m_inputSequence;

    // Movement (WASD)
    if (IsKeyDown(KEY_W)) input.moveDirection.z -= 1.0f;
    if (IsKeyDown(KEY_S)) input.moveDirection.z += 1.0f;
    if (IsKeyDown(KEY_A)) input.moveDirection.x -= 1.0f;
    if (IsKeyDown(KEY_D)) input.moveDirection.x += 1.0f;

    // Normalize diagonal movement
    if (Vector3Length(input.moveDirection) > 1.0f) {
        input.moveDirection = Vector3Normalize(input.moveDirection);
    }

    // Jump
    input.jump = IsKeyPressed(KEY_SPACE);

    // Sprint
    input.sprint = IsKeyDown(KEY_LEFT_SHIFT);

    // Look (mouse)
    if (m_cursorLocked) {
        Vector2 mousePos = GetMousePosition();
        Vector2 mouseDelta = GetMouseDelta();

        input.lookDelta.x = mouseDelta.x * m_mouseSensitivity;
        input.lookDelta.y = mouseDelta.y * m_mouseSensitivity * (m_invertY ? -1.0f : 1.0f);

        m_lastMousePos = mousePos;
    }

    // Shooting
    input.primaryFire = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    input.secondaryFire = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);

    // Reload
    input.reload = IsKeyPressed(KEY_R);

    // Weapon switching
    if (IsKeyPressed(KEY_ONE)) input.weaponSlot = 1;
    if (IsKeyPressed(KEY_TWO)) input.weaponSlot = 2;

    // Mouse wheel weapon switch
    float wheel = GetMouseWheelMove();
    if (wheel > 0) input.weaponSlot = 1;
    if (wheel < 0) input.weaponSlot = 2;

    return input;
}

bool InputHandler::isKeyPressed(int key) const
{
    return IsKeyPressed(key);
}

bool InputHandler::isKeyDown(int key) const
{
    return IsKeyDown(key);
}

bool InputHandler::isMouseButtonPressed(int button) const
{
    return IsMouseButtonPressed(button);
}

bool InputHandler::isMouseButtonDown(int button) const
{
    return IsMouseButtonDown(button);
}

void InputHandler::lockCursor()
{
    DisableCursor();
    m_cursorLocked = true;
    m_lastMousePos = GetMousePosition();
}

void InputHandler::unlockCursor()
{
    EnableCursor();
    m_cursorLocked = false;
}

} // namespace arena
