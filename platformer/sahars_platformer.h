#ifndef sahars_platformer
#define sahars_platformer
#include "raylib.h"
#include "raygui.h"
#include <vector>
#include <chrono>
struct Box {
    Vector2 min;
    Vector2 max;
};
class Entity {
public:
    Vector2 position;
    Vector2 velocity;
    float width;
    float height;
    Box aabb;
    Color color;
    bool isStatic;
    bool isGrounded;
    Entity(float xPosition, float yPosition, float entityWidth, float entityHeight, Color entityColor, bool isEntityStatic = true);
    void updateAABB();
    void moveBy(float x, float y);
    void moveTo(float x, float y);
    void applyGravity(Vector2 gravityCenter, float pullFactor);
    void tick();
    void draw();
};
namespace GameConfig {
    constexpr float SOURCE_WIDTH = 1280.0f;
    constexpr float SOURCE_HEIGHT = 720.0f;
    constexpr float SOURCE_ASPECT_RATIO = SOURCE_WIDTH / SOURCE_HEIGHT;
    constexpr int MIN_WINDOW_WIDTH = 800;
    constexpr int MIN_WINDOW_HEIGHT = 600;
    constexpr int GROUND_WIDTH = SOURCE_WIDTH * 4;
    constexpr int GROUND_HEIGHT = 100;
    constexpr float GRAVITY_STRENGTH = 0.2f;
    constexpr float JUMP_STRENGTH = -12.5f;
    constexpr float DEAD_ZONE_PERCENT_X = 0.2f;
    constexpr float DEAD_ZONE_PERCENT_Y = 0.3f;
    constexpr int DUMMY_ENTITY_COUNT = 0;
    inline bool showDebugInfo = true;
    inline bool cameraShouldFollow = true;
    inline bool prevCameraFollowState = true;
    inline Vector2 cameraFocus = { 0.0f, 0.0f };
}
namespace SessionData {
    inline int currentMonitor = 0;
    inline int monitorRefreshRate = 60;
    inline int monitorAspectRatio = 0;
    inline int windowWidth = GameConfig::MIN_WINDOW_WIDTH;
    inline int windowHeight = GameConfig::MIN_WINDOW_HEIGHT;
    inline float windowAspectRatio = windowWidth / windowHeight;
    inline Rectangle source = { 0.0f, 0.0f, GameConfig::SOURCE_WIDTH, GameConfig::SOURCE_HEIGHT };
    inline Rectangle display = { 0.0f, 0.0f, windowWidth, windowHeight };
    inline int command = 0;
    inline RenderTexture2D target;
    inline Camera2D gameCamera = { 0 };
    inline Rectangle effectiveRenderTextureSource;
    inline float minCameraFocusX = 0;
    inline float maxCameraFocusX = 0;
    inline float minCameraFocusY = 0;
    inline float maxCameraFocusY = 0;
    inline long long prevTime = 0;
    inline long long deltaTime;
}
namespace Random {
    float getFloat(float min, float max);
    int getInt(int min, int max);
}
inline std::vector<Entity> platforms{};
inline std::vector<Entity> entities{};
inline Entity player{ 0, 0, 20, 20, DARKPURPLE, false };
void collision(Entity& entity);
Box getBox(Vector2 position, float width, float height);
void newGroundObstacle(float x, float height, Color color = LIGHTGRAY);
void spawnTestEntities();
#endif