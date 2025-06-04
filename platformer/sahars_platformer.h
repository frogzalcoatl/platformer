#ifndef sahars_platformer
#define sahars_platformer
#include "raylib.h"
#include "raygui.h"
#include <vector>
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
    void draw() const;
};
namespace GameConfig {
    constexpr int SOURCE_WIDTH = 1280;
    constexpr int SOURCE_HEIGHT = 720;
    constexpr float SOURCE_ASPECT_RATIO = SOURCE_WIDTH / SOURCE_HEIGHT;
    constexpr int MIN_WINDOW_WIDTH = 800;
    constexpr int MIN_WINDOW_HEIGHT = 600;
    constexpr float MAX_VELOCITY_X = 600.0f;
    constexpr float MAX_VELOCITY_Y = 1500.0f;
    constexpr float PLAYER_ACCELERATION = 1500.0f;
    constexpr float GRAVITY_PULL_FACTOR = 2500.0f;
    constexpr float JUMP_VELOCITY = 1000.0f;
    constexpr float AIR_FRICTION = 0.105f;
    constexpr float GROUND_FRICTION = 6.0f;
    constexpr float GROUND_WIDTH = SOURCE_WIDTH * 4.0f;
    constexpr float GROUND_HEIGHT = 100.0f;
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
    inline float windowAspectRatio = (float)(windowWidth / windowHeight);
    inline Rectangle source = { 0.0f, 0.0f, GameConfig::SOURCE_WIDTH, GameConfig::SOURCE_HEIGHT };
    inline Rectangle display = { 0.0f, 0.0f, (float)windowWidth, (float)windowHeight };
    inline int command = 0;
    inline RenderTexture2D target;
    inline Camera2D gameCamera = { 0 };
    inline Rectangle effectiveRenderTextureSource;
    inline float minCameraFocusX = 0.0f;
    inline float maxCameraFocusX = 0.0f;
    inline float minCameraFocusY = 0.0f;
    inline float maxCameraFocusY = 0.0f;
    inline float deltaTime = 0.0f;
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
void monitorAndWindowChecks();
void keyCommands();
void cameraFocus();
void updateEffectiveRenderTextureSource();
void displayDebugInfo();
void drawScaledScreen();
void windowSetup();
#endif