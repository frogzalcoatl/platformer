#ifndef sahars_platformer
#define sahars_platformer
#include "raylib.h"
#include "raygui.h"
#include <vector>
#include <memory>

struct AABB {
    Vector2 min;
    Vector2 max;
};
class Entity {
public:
    Vector2 position;
    Vector2 velocity;
    float width;
    float height;
    AABB aabb;
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
class QuadTree {
public:
    static const int NODE_CAPACITY = 4;
    AABB boundary;
    std::vector<Entity*> entityPtrs;
    std::unique_ptr<QuadTree> northWest;
    std::unique_ptr<QuadTree> northEast;
    std::unique_ptr<QuadTree> southWest;
    std::unique_ptr<QuadTree> southEast;
    QuadTree(AABB _boundary);
    bool insert(Entity* entityPtr);
    void subdivide();
    std::vector<Entity*> queryRange(AABB range);
    void quadTreeCollision();
    void draw() const;
};
namespace GameConfig {
    constexpr int SOURCE_WIDTH = 1280;
    constexpr int SOURCE_HEIGHT = 720;
    constexpr float SOURCE_ASPECT_RATIO = (float)SOURCE_WIDTH / SOURCE_HEIGHT;
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
    constexpr float MAX_CAMERA_ZOOM = 1.50f;
    constexpr float MIN_CAMERA_ZOOM = 0.01f;
    constexpr int DUMMY_ENTITY_COUNT = 0;
    inline bool showDebugInfo = true;
    inline bool cameraShouldFollow = true;
    inline bool prevCameraFollowState = true;
}
namespace SessionData {
    inline QuadTree quadTree = QuadTree({ { -GameConfig::GROUND_WIDTH / 2.0f, 0.0f }, { GameConfig::GROUND_WIDTH / 2.0f, 10.0f } });
    inline float deltaTime = 0.0f;
    inline int command = 0;
    inline int currentMonitor = 0;
    inline int monitorRefreshRate = 60;
    inline float monitorAspectRatio = 0.0f;
    inline int windowWidth = GameConfig::MIN_WINDOW_WIDTH;
    inline int windowHeight = GameConfig::MIN_WINDOW_HEIGHT;
    inline float windowAspectRatio = (float)(windowWidth / windowHeight);
    inline int preFullscreenWindowWidth = windowWidth;
    inline int preFullscreenWindowHeight = windowHeight;
    inline Rectangle effectiveSource;
    inline RenderTexture2D target;
    inline Camera2D gameCamera = { 0 };
    inline float minCameraFocusX = 0.0f;
    inline float maxCameraFocusX = 0.0f;
    inline float minCameraFocusY = 0.0f;
    inline float maxCameraFocusY = 0.0f;
}
namespace Random {
    float getFloat(float min, float max);
    int getInt(int min, int max);
}
inline std::vector<Entity> platforms{};
inline std::vector<Entity> entities{};
inline Entity player{ 0, 0, 20, 20, DARKPURPLE, false };
void collision(Entity& entity, std::vector<Entity>& collidingEntities);
AABB getBox(Vector2 position, float width, float height);
bool areAABBsColliding(AABB box1, AABB box2);
Rectangle AABBtoRectangle(AABB aabb);
void newGroundObstacle(float x, float height, Color color = LIGHTGRAY);
void spawnTestEntities();
void monitorAndWindowChecks(bool overRideWindowResizedCheck = false);
void keyCommands();
void cameraFocus();
void displayDebugInfo();
void drawScaledScreen();
void windowSetup();
void PlatformerFullscreenToggle();
#endif