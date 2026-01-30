#include "raylib.h"
#include "raymath.h"

#include "game/Game.h"

int main(int argc, char* argv[])
{
    // Window configuration
    const int screenWidth = 1280;
    const int screenHeight = 720;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "Arena Shooter - Phase 1");

    // 3D Camera setup
    Camera3D camera = { 0 };
    camera.position = { 10.0f, 10.0f, 10.0f };
    camera.target = { 0.0f, 0.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Cube rotation angle
    float rotationAngle = 0.0f;

    // Grid settings
    const int gridSlices = 20;
    const float gridSpacing = 1.0f;

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        float deltaTime = GetFrameTime();
        rotationAngle += 45.0f * deltaTime; // 45 degrees per second
        if (rotationAngle >= 360.0f) rotationAngle -= 360.0f;

        // Simple camera orbit with arrow keys
        if (IsKeyDown(KEY_LEFT)) {
            camera.position = Vector3Transform(camera.position,
                MatrixRotateY(2.0f * deltaTime));
        }
        if (IsKeyDown(KEY_RIGHT)) {
            camera.position = Vector3Transform(camera.position,
                MatrixRotateY(-2.0f * deltaTime));
        }
        if (IsKeyDown(KEY_UP)) {
            camera.position.y += 5.0f * deltaTime;
        }
        if (IsKeyDown(KEY_DOWN)) {
            camera.position.y -= 5.0f * deltaTime;
        }

        // Draw
        BeginDrawing();
        {
            ClearBackground(DARKGRAY);

            BeginMode3D(camera);
            {
                // Draw ground grid
                DrawGrid(gridSlices, gridSpacing);

                // Draw rotating cube
                DrawCubeWiresV({ 0.0f, 1.0f, 0.0f }, { 2.0f, 2.0f, 2.0f }, LIME);

                // Draw filled cube with rotation
                Vector3 cubePos = { 0.0f, 1.0f, 0.0f };

                // Apply rotation transformation
                rlPushMatrix();
                rlTranslatef(cubePos.x, cubePos.y, cubePos.z);
                rlRotatef(rotationAngle, 0.0f, 1.0f, 0.0f);
                rlRotatef(rotationAngle * 0.5f, 1.0f, 0.0f, 0.0f);

                // Draw cube at origin (already translated)
                DrawCube({ 0.0f, 0.0f, 0.0f }, 2.0f, 2.0f, 2.0f,
                    Fade(RED, 0.7f));
                DrawCubeWires({ 0.0f, 0.0f, 0.0f }, 2.0f, 2.0f, 2.0f, MAROON);

                rlPopMatrix();

                // Draw coordinate axes for reference
                DrawLine3D({ 0, 0, 0 }, { 5, 0, 0 }, RED);     // X-axis
                DrawLine3D({ 0, 0, 0 }, { 0, 5, 0 }, GREEN);   // Y-axis
                DrawLine3D({ 0, 0, 0 }, { 0, 0, 5 }, BLUE);    // Z-axis
            }
            EndMode3D();

            // HUD
            DrawRectangle(10, 10, 320, 100, Fade(BLACK, 0.5f));
            DrawText("ARENA SHOOTER - Phase 1", 20, 20, 20, WHITE);
            DrawText("Rotating Cube Demo", 20, 45, 16, LIGHTGRAY);
            DrawText("Arrow Keys: Orbit Camera", 20, 70, 14, GRAY);
            DrawText(TextFormat("FPS: %d", GetFPS()), 20, 90, 14, GREEN);
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
