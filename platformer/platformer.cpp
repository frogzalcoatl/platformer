#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <vector>
#include <string>

struct Box {
    Vector2 min;
    Vector2 max;
};

Box getBox(Vector2 position, float width, float height) {
    return { position, { position.x + width, position.y + height } };
};

class Entity {
public:
    Vector2 position;
    Vector2 velocity;
    float width;
    float height;
    Box aabb;
    Color color;
    Entity(float xPosition, float yPosition, float entityWidth, float entityHeight, Color entityColor) {
        position = { xPosition, yPosition };
        velocity = { 0, 0 };
        width = entityWidth;
        height = entityHeight;
        aabb = getBox(position, width, height);
        color = entityColor;
    };
    void Draw() {
        DrawRectangle(position.x, position.y, width, height, color);
        //DrawRectangleLines(box.min.x, box.min.y, box.max.x - box.min.x, box.max.y - box.min.y, RED);
    };
    void MoveBy(int x, int y) {
        position.x += x;
        position.y += y;
    };
    void MoveTo(int x, int y) {
        position.x = x;
        position.y = y;
        velocity.x = 0;
        velocity.y = 0;
    };
    void Tick() {
        position.x += velocity.x;
        position.y += velocity.y;
        aabb = getBox(position, width, height);
    }
    void ApplyGravity(Vector2 point, float factor) {
        float distanceX = point.x - position.x;
        float distanceY = point.y - position.y;
        float distance = std::sqrt(std::pow(distanceX, 2) + std::pow(distanceY, 2));
        if (distance == 0) distance = 1;
        velocity.x += (distanceX / distance) * factor;
        velocity.y += (distanceY / distance) * factor;
    }
};

int windowHeight = GetScreenHeight();
int windowWidth = GetScreenWidth();
std::vector<Entity> platforms = {};
Entity player = Entity(380, 555, 20, 20, DARKPURPLE);

void Collision(Entity& entity) {
    for (int i = 0; i < platforms.size(); i++) {
        if (
            entity.position.x + entity.width > platforms[i].position.x &&
            ((entity.position.y < platforms[i].position.y && entity.position.y + entity.height < platforms[i].position.y) ||
                (entity.position.y < platforms[i].position.y + platforms[i].height && entity.position.y + entity.height < platforms[i].position.y + platforms[i].height))
            ) {
            std::cout << entity.position.x + entity.width << std::endl;
        }
    }
}

int main() {
    InitWindow(800, 600, "Platformer");
    SetTargetFPS(GetMonitorRefreshRate(0));
    //platforms.push_back(Entity(0, 580, 800, 20, DARKGREEN));
    platforms.push_back(Entity(350, 250, 50, 50, RED));
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(SKYBLUE);
        int command = 0;
        if (IsKeyDown(KEY_W)) command |= 1;
        if (IsKeyDown(KEY_S)) command |= 2;
        if (IsKeyDown(KEY_A)) command |= 4;
        if (IsKeyDown(KEY_D)) command |= 8;
        if (IsKeyDown(KEY_R)) command |= 16;
        if (IsKeyDown(KEY_V)) command |= 32;
        if (command & 1) player.velocity.y -= 0.1;
        if (command & 2) player.velocity.y += 0.1;
        if (command & 4) player.velocity.x -= 0.1;
        if (command & 8) player.velocity.x += 0.1;
        if (command & 16) {
            player.position = { 380, 555 };
            player.velocity.x = 0;
            player.velocity.y = 0;
        }
        if (command & 32) {
            player.velocity.x = 0;
            player.velocity.y = 0;
        }
        //player.ApplyGravity({ player.position.x, 600 }, 0.1);
        Collision(player);
        player.Tick();
        for (int i = 0; i < platforms.size(); i++) {
            platforms[i].Draw();
        }
        player.Draw();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}