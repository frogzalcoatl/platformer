#ifndef sahars_platformer
#define sahars_platformer
#include "raylib.h"
#include "raygui.h"
#include "raymath.h"
#include <memory>
#include <optional>
#include <vector>

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
  Entity(float xPosition, float yPosition, float entityWidth,
         float entityHeight, Color entityColor, bool isEntityStatic = true);
  void updateAABB();
  void moveBy(float x, float y);
  void moveTo(float x, float y);
  void applyGravity(Vector2 gravityCenter, float pullFactor);
  void tick();
  void draw() const;
};
class QuadTree {
 public:
  static constexpr int NODE_CAPACITY = 8;
  AABB boundary;
  std::vector<Entity *> entityPtrs;
  std::unique_ptr<QuadTree> northWest;
  std::unique_ptr<QuadTree> northEast;
  std::unique_ptr<QuadTree> southWest;
  std::unique_ptr<QuadTree> southEast;
  QuadTree(AABB _boundary);
  void clear();
  bool insert(Entity *entityPtr);
  void subdivide();
  std::vector<Entity *> queryRange(AABB &range);
  void quadTreeCollision();
  void drawBounds() const;
  void drawText() const;
};
namespace GameConfig {
constexpr int MIN_WINDOW_WIDTH = 800;
constexpr int MIN_WINDOW_HEIGHT = 600;
constexpr float WORLD_FLOOR_Y = 0.0f;
constexpr float WORLD_CEILING_Y = 100.0f;
constexpr float WORLD_LEFT_BORDER = -100.0f;
constexpr float WORLD_RIGHT_BORDER = 100.0f;
constexpr float GROUND_HEIGHT = 3.0f;
constexpr float MAX_VELOCITY_X = 35.0f;
constexpr float MAX_VELOCITY_Y = 90.0f;
constexpr float PLAYER_ACCELERATION = 75.0f;
constexpr float GRAVITY_PULL_FACTOR = 150.0f;
constexpr float JUMP_VELOCITY = 65.0f;
constexpr float AIR_FRICTION = 0.105f;
constexpr float GROUND_FRICTION = 6.0f;
constexpr Vector2 PLAYER_SPAWN_POINT = {0.0f, 3.5f};
constexpr float DEAD_ZONE_PERCENT_X = 0.3f;
constexpr float DEAD_ZONE_PERCENT_Y = 0.3f;
constexpr float MAX_USER_ZOOM = 1.50f;
constexpr float MIN_USER_ZOOM = 0.01f;
constexpr int DUMMY_ENTITY_COUNT = 0;
constexpr bool ARE_DUMMY_ENTITIES_STATIC = true;
}  // namespace GameConfig
namespace SessionData {
inline QuadTree quadTree =
    QuadTree({{GameConfig::WORLD_LEFT_BORDER, GameConfig::WORLD_FLOOR_Y},
              {GameConfig::WORLD_RIGHT_BORDER, GameConfig::WORLD_CEILING_Y}});
inline float deltaTime = 0.0f;
inline int currentMonitor = 0;
inline int monitorRefreshRate = 60;
inline float monitorAspectRatio = 0.0f;
inline int windowWidth = GameConfig::MIN_WINDOW_WIDTH;
inline int windowHeight = GameConfig::MIN_WINDOW_HEIGHT;
inline float windowAspectRatio = (float)(windowWidth / windowHeight);
inline int preFullscreenWindowWidth = windowWidth;
inline int preFullscreenWindowHeight = windowHeight;
inline Camera2D camera = {0};
inline float pixelsPerBlock = 20.0f;
inline float userZoom = 1.0f;
inline Vector2 mousePosition = {0.0f, 0.0f};
inline float mouseWheelMove = 0.0f;
inline bool leftClickDown = false;
inline bool rightClickDown = false;
inline bool isGravityEnabled = true;
inline bool showDebugInfo = true;
inline bool cameraShouldFollow = true;
inline bool prevCameraFollowState = true;
inline bool exitWindow = false;
inline int uiScreen = 0;
}  // namespace SessionData
namespace Random {
float getFloat(float min, float max);
int getInt(int min, int max);
}  // namespace Random
inline std::vector<Entity> entities{};
inline Entity player{0, 0, 1, 1, DARKPURPLE, false};
void collision(Entity *entity, std::vector<Entity *> collidingEntities);
AABB getBox(Vector2 position, float width, float height);
bool areAABBsColliding(AABB box1, AABB box2);
Rectangle AABBtoRectangle(AABB aabb, int multiplier = 1);
void newGroundObstacle(float x, float height, Color color = LIGHTGRAY);
void spawnTestEntities();
void monitorAndWindowChecks(const bool overRideWindowResizedCheck = false);
void userInput();
void scrollZoom();
void gameplayInput();
void cameraFollow(Camera2D &camera);
void cameraStatic(Camera2D &camera);
void displayDebugInfo();
void windowSetup();
void platformerFullscreenToggle();
void drawAABB(const AABB *aabb, Color color);
void drawAABBLines(const AABB *aabb, float lineThick, Color color);
void drawWorldText(const char *text, float posX, float posY, float fontSize,
                   float spacing, Color color);
void drawGameCamera();
bool gamePause();
#endif