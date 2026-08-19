#include "raylib.h"
#include "raymath.h"
#include <algorithm>

struct Player {
    Vector3 position{0.0f, 2.0f, 0.0f};
    Vector3 velocity{0.0f, 0.0f, 0.0f};
    float radius = 0.5f;
    bool grounded = false;
};

static float ClampMagnitude(float value, float maxValue) {
    return std::clamp(value, -maxValue, maxValue);
}

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1280, 720, "C++ 3D Physics Engine - WASD");
    SetTargetFPS(144);

    Camera3D camera{};
    camera.position = {7.0f, 5.0f, 7.0f};
    camera.target = {0.0f, 1.0f, 0.0f};
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Player player;
    const float gravity = -18.0f;
    const float moveSpeed = 7.0f;
    const float jumpSpeed = 7.5f;
    const float groundY = 0.0f;

    DisableCursor();

    while (!WindowShouldClose()) {
        const float dt = std::min(GetFrameTime(), 0.033f);

        // Camera-relative WASD movement.
        Vector3 forward = Vector3Normalize({camera.target.x - camera.position.x, 0.0f,
                                            camera.target.z - camera.position.z});
        Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));
        Vector3 input{0.0f, 0.0f, 0.0f};
        if (IsKeyDown(KEY_W)) input = Vector3Add(input, forward);
        if (IsKeyDown(KEY_S)) input = Vector3Subtract(input, forward);
        if (IsKeyDown(KEY_D)) input = Vector3Add(input, right);
        if (IsKeyDown(KEY_A)) input = Vector3Subtract(input, right);
        if (Vector3LengthSqr(input) > 0.0f) input = Vector3Normalize(input);

        player.velocity.x = input.x * moveSpeed;
        player.velocity.z = input.z * moveSpeed;
        player.velocity.y += gravity * dt;

        if (player.grounded && IsKeyPressed(KEY_SPACE)) {
            player.velocity.y = jumpSpeed;
            player.grounded = false;
        }

        player.position = Vector3Add(player.position, Vector3Scale(player.velocity, dt));

        // Simple sphere-vs-ground collision.
        if (player.position.y - player.radius < groundY) {
            player.position.y = groundY + player.radius;
            player.velocity.y = 0.0f;
            player.grounded = true;
        } else {
            player.grounded = false;
        }

        // Keep the player inside a simple arena.
        constexpr float arena = 9.5f;
        player.position.x = ClampMagnitude(player.position.x, arena);
        player.position.z = ClampMagnitude(player.position.z, arena);

        // Mouse-look camera.
        Vector2 mouse = GetMouseDelta();
        Vector3 look = Vector3Subtract(camera.target, camera.position);
        float yaw = -mouse.x * 0.003f;
        float pitch = -mouse.y * 0.003f;
        Matrix rotation = MatrixRotateXYZ({pitch, yaw, 0.0f});
        look = Vector3Transform(look, rotation);
        look = Vector3Normalize(look);
        camera.position = Vector3Add(player.position, Vector3Scale(Vector3Normalize(Vector3Subtract(camera.position, player.position)), 5.5f));
        camera.target = Vector3Add(camera.position, Vector3Scale(look, 10.0f));

        BeginDrawing();
        ClearBackground({18, 22, 30, 255});
        BeginMode3D(camera);

        DrawPlane({0, 0, 0}, {20, 20}, {70, 75, 85, 255});
        DrawGrid(20, 1.0f);
        DrawCube({0, 1.0f, -10.0f}, {20, 2, 0.5f}, {120, 130, 150, 255});
        DrawCube({0, 1.0f, 10.0f}, {20, 2, 0.5f}, {120, 130, 150, 255});
        DrawCube({-10.0f, 1.0f, 0}, {0.5f, 2, 20}, {120, 130, 150, 255});
        DrawCube({10.0f, 1.0f, 0}, {0.5f, 2, 20}, {120, 130, 150, 255});

        // Physics test objects.
        DrawCube({-3, 1, -2}, {2, 2, 2}, {220, 120, 80, 255});
        DrawCubeWires({-3, 1, -2}, {2, 2, 2}, WHITE);
        DrawSphere(player.position, player.radius, {80, 180, 255, 255});
        DrawSphereWires(player.position, player.radius, 16, 16, WHITE);

        EndMode3D();

        DrawText("C++ 3D PHYSICS ENGINE", 20, 20, 28, RAYWHITE);
        DrawText("WASD: Move   SPACE: Jump   Mouse: Look", 20, 58, 20, LIGHTGRAY);
        DrawText(TextFormat("Velocity: %.2f, %.2f, %.2f", player.velocity.x, player.velocity.y, player.velocity.z),
                 20, 88, 18, LIGHTGRAY);
        DrawText("Gravity + ground collision + arena bounds", 20, 116, 18, LIGHTGRAY);
        EndDrawing();
    }

    EnableCursor();
    CloseWindow();
    return 0;
}
