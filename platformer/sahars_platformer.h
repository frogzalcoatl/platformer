#ifndef sahars_platformer
#define sahars_platformer
#include "raylib.h"
#include "raygui.h"
#include <vector>
struct Box {
    Vector2 min;
    Vector2 max;
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
    constexpr float MIN_CAMERA_FOCUS_X = -GROUND_WIDTH / 2.0f + (SOURCE_WIDTH / 2.0f);
    constexpr float MAX_CAMERA_FOCUS_X = GROUND_WIDTH / 2.0f - (SOURCE_WIDTH / 2.0f);
    constexpr float MIN_CAMERA_FOCUS_Y = SOURCE_HEIGHT / 2.0f;
    constexpr float MAX_CAMERA_FOCUS_Y = MIN_CAMERA_FOCUS_Y;
    inline bool showDebugInfo = true;
    inline bool cameraShouldFollow = true;
    inline bool prevCameraFollowState = true;
    inline Vector2 cameraFocus = { 0.0f, 0.0f };
}
namespace SessionData {
    inline int currentMonitor = 0;
    inline int monitorRefreshRate = 60;
    inline int windowWidth = GameConfig::MIN_WINDOW_WIDTH;
    inline int windowHeight = GameConfig::MIN_WINDOW_HEIGHT;
    inline int monitorAspectRatio = windowWidth / windowHeight;
}
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
inline std::vector<Entity> platforms{};
inline std::vector<Entity> entities{};
inline Entity player{ 0, 0, 20, 20, DARKPURPLE, false };
void newGroundObstacle(float x, float height, Color color = LIGHTGRAY);
void collision(Entity& entity);
Box getBox(Vector2 position, float width, float height);
#endif