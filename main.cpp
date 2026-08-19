#include "raylib.h"
#include "raymath.h"
#include <algorithm>
#include <cmath>

struct Body {
    Vector3 position{0, 1, 0};
    Vector3 velocity{0, 0, 0};
    Vector3 halfExtents{0.5f, 0.5f, 0.5f};
    bool grounded = false;
};

class PhysicsWorld {
public:
    Vector3 gravity{0, -20.0f, 0};
    float arena = 9.5f;

    void Step(Body& body, float dt) const {
        body.velocity = Vector3Add(body.velocity, Vector3Scale(gravity, dt));
        body.position = Vector3Add(body.position, Vector3Scale(body.velocity, dt));

        if (body.position.y - body.halfExtents.y < 0) {
            body.position.y = body.halfExtents.y;
            if (body.velocity.y < 0) body.velocity.y = 0;
            body.grounded = true;
        } else {
            body.grounded = false;
        }

        body.position.x = std::clamp(body.position.x, -arena + body.halfExtents.x, arena - body.halfExtents.x);
        body.position.z = std::clamp(body.position.z, -arena + body.halfExtents.z, arena - body.halfExtents.z);
    }
};

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1280, 720, "3D Physics Engine");
    SetTargetFPS(144);
    DisableCursor();

    PhysicsWorld physics;
    Body player;

    Camera3D camera{};
    camera.up = {0, 1, 0};
    camera.fovy = 70;
    camera.projection = CAMERA_PERSPECTIVE;

    float yaw = 225.0f * DEG2RAD;
    float pitch = -18.0f * DEG2RAD;
    const float speed = 7.0f;
    const float jump = 8.0f;

    while (!WindowShouldClose()) {
        float dt = std::min(GetFrameTime(), 1.0f / 30.0f);

        Vector2 mouse = GetMouseDelta();
        yaw -= mouse.x * 0.003f;
        pitch -= mouse.y * 0.003f;
        pitch = std::clamp(pitch, -1.4f, 1.4f);

        Vector3 forward{
            std::cos(pitch) * std::sin(yaw),
            std::sin(pitch),
            std::cos(pitch) * std::cos(yaw)
        };
        forward = Vector3Normalize(forward);
        Vector3 flatForward = Vector3Normalize({forward.x, 0, forward.z});
        Vector3 right = Vector3Normalize(Vector3CrossProduct(flatForward, {0, 1, 0}));

        Vector3 input{0, 0, 0};
        if (IsKeyDown(KEY_W)) input = Vector3Add(input, flatForward);
        if (IsKeyDown(KEY_S)) input = Vector3Subtract(input, flatForward);
        if (IsKeyDown(KEY_D)) input = Vector3Add(input, right);
        if (IsKeyDown(KEY_A)) input = Vector3Subtract(input, right);
        if (Vector3LengthSqr(input) > 0) input = Vector3Normalize(input);

        const float acceleration = 30.0f;
        player.velocity.x += (input.x * speed - player.velocity.x) * std::min(acceleration * dt, 1.0f);
        player.velocity.z += (input.z * speed - player.velocity.z) * std::min(acceleration * dt, 1.0f);

        if (IsKeyPressed(KEY_SPACE) && player.grounded)
            player.velocity.y = jump;

        physics.Step(player, dt);

        camera.position = Vector3Add(player.position, Vector3Scale(forward, -6.0f));
        camera.position.y += 2.0f;
        camera.target = Vector3Add(player.position, {0, 0.6f, 0});

        BeginDrawing();
        ClearBackground({15, 18, 27, 255});
        BeginMode3D(camera);

        DrawPlane({0, 0, 0}, {20, 20}, {65, 70, 82, 255});
        DrawGrid(20, 1.0f);

        DrawCube({0, 1, -10}, {20, 2, 0.4f}, {110, 120, 145, 255});
        DrawCube({0, 1, 10}, {20, 2, 0.4f}, {110, 120, 145, 255});
        DrawCube({-10, 1, 0}, {0.4f, 2, 20}, {110, 120, 145, 255});
        DrawCube({10, 1, 0}, {0.4f, 2, 20}, {110, 120, 145, 255});

        DrawCube({-3, 1, -2}, {2, 2, 2}, {220, 110, 70, 255});
        DrawCubeWires({-3, 1, -2}, {2, 2, 2}, WHITE);
        DrawCube({3, 0.75f, 2}, {1.5f, 1.5f, 1.5f}, {80, 190, 110, 255});
        DrawCubeWires({3, 0.75f, 2}, {1.5f, 1.5f, 1.5f}, WHITE);

        DrawCube(player.position, Vector3Scale(player.halfExtents, 2), {70, 165, 255, 255});
        DrawCubeWires(player.position, Vector3Scale(player.halfExtents, 2), WHITE);

        EndMode3D();

        DrawText("3D PHYSICS ENGINE", 20, 18, 30, RAYWHITE);
        DrawText("WASD: Move   SPACE: Jump   Mouse: Look", 20, 55, 20, LIGHTGRAY);
        DrawText(TextFormat("Position: %.2f %.2f %.2f", player.position.x, player.position.y, player.position.z), 20, 88, 18, LIGHTGRAY);
        DrawText(TextFormat("Velocity: %.2f %.2f %.2f", player.velocity.x, player.velocity.y, player.velocity.z), 20, 114, 18, LIGHTGRAY);
        DrawText(player.grounded ? "Grounded" : "Airborne", 20, 140, 18, player.grounded ? GREEN : ORANGE);
        EndDrawing();
    }

    EnableCursor();
    CloseWindow();
    return 0;
}
