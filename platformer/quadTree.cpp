#include "sahars_platformer.h"
#include <ranges>
#include <iostream>

QuadTree::QuadTree(AABB _boundary) {
	boundary = _boundary;
}
void QuadTree::clear() {
	entityPtrs.clear();
	if (northWest != nullptr) {
		northWest->clear();
		northEast->clear();
		southWest->clear();
		southEast->clear();
		northWest = nullptr;
		northEast = nullptr;
		southWest = nullptr;
		southEast = nullptr;
	}
}
bool QuadTree::insert(Entity* entityPtr) {
	if (!areAABBsColliding(boundary, entityPtr->aabb)) return false;
	if (entityPtrs.size() < QuadTree::NODE_CAPACITY && northWest == nullptr) {
		entityPtrs.push_back(entityPtr);
		return true;
	}
	if (northWest == nullptr) {
		subdivide();
	}
	if (northWest->insert(entityPtr)) return true;
	if (northEast->insert(entityPtr)) return true;
	if (southWest->insert(entityPtr)) return true;
	if (southEast->insert(entityPtr)) return true;
	return false;
}
void QuadTree::subdivide() {
	float xLengthHalf = (boundary.max.x - boundary.min.x) / 2.0f;
	float yLengthHalf = (boundary.max.y - boundary.min.y) / 2.0f;
	northWest = std::make_unique<QuadTree>(AABB{ { boundary.min.x, boundary.min.y }, { boundary.max.x - xLengthHalf, boundary.max.y - yLengthHalf } });
	northEast = std::make_unique<QuadTree>(AABB{ { boundary.min.x + xLengthHalf, boundary.min.y }, { boundary.max.x, boundary.max.y - yLengthHalf } });
	southWest = std::make_unique<QuadTree>(AABB{ { boundary.min.x, boundary.min.y + yLengthHalf }, { boundary.max.x - xLengthHalf, boundary.max.y } });
	southEast = std::make_unique<QuadTree>(AABB{ { boundary.min.x + xLengthHalf, boundary.min.y + yLengthHalf }, { boundary.max.x, boundary.max.y } });
}
std::vector<Entity*> QuadTree::queryRange(AABB range) {
	std::vector<Entity*> entitiesInRange;
	if (!areAABBsColliding(boundary, range)) return entitiesInRange;
	for (size_t i = 0; i < entityPtrs.size(); i++) {
		if (areAABBsColliding(range, entityPtrs[i]->aabb)) {
			entitiesInRange.push_back(entityPtrs[i]);
		}
	}
	if (northWest == nullptr) return entitiesInRange;
	std::vector<Entity*> childNodeEntities;
	childNodeEntities = northWest->queryRange(range);
	entitiesInRange.insert(entitiesInRange.end(), childNodeEntities.begin(), childNodeEntities.end());
	childNodeEntities = northEast->queryRange(range);
	entitiesInRange.insert(entitiesInRange.end(), childNodeEntities.begin(), childNodeEntities.end());
	childNodeEntities = southWest->queryRange(range);
	entitiesInRange.insert(entitiesInRange.end(), childNodeEntities.begin(), childNodeEntities.end());
	childNodeEntities = southEast->queryRange(range);
	entitiesInRange.insert(entitiesInRange.end(), childNodeEntities.begin(), childNodeEntities.end());
	return entitiesInRange;
}
void QuadTree::quadTreeCollision() {
	for (size_t i = 0; i < entityPtrs.size(); i++) {
		collision(entityPtrs[i], entityPtrs);
	}
}
void QuadTree::draw() const {
	DrawRectangleLinesEx(AABBtoRectangle(boundary, SessionData::pixelsPerBlock), 5.0f, BLUE);
	if (northWest == nullptr) {
		const char* text = TextFormat("%.0f:%.0f", entityPtrs.size(), (float)QuadTree::NODE_CAPACITY);
		int fontSize = (int)(boundary.max.y * SessionData::pixelsPerBlock - boundary.min.y * SessionData::pixelsPerBlock);
		int textWidth = MeasureText(text, fontSize);
		DrawText(
			text,
			(int)(boundary.min.x * SessionData::pixelsPerBlock + (boundary.max.x * SessionData::pixelsPerBlock - boundary.min.x * SessionData::pixelsPerBlock) / 2.0f - (textWidth / 2.0f)),
			(int)(boundary.min.y * SessionData::pixelsPerBlock),
			fontSize,
			BLUE
		);
		return;
	}
	northWest->draw();
	northEast->draw();
	southWest->draw();
	southEast->draw();
}