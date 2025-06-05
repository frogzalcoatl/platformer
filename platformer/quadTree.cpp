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
	if (northWest != nullptr) {
		if (northWest->insert(entityPtr)) return true;
		if (northEast->insert(entityPtr)) return true;
		if (southWest->insert(entityPtr)) return true;
		if (southEast->insert(entityPtr)) return true;
	}
	entityPtrs.push_back(entityPtr);
	if (entityPtrs.size() > NODE_CAPACITY && northWest == nullptr) {
		subdivide();
		std::vector<Entity*> parentEntities;
		size_t i = 0;
		while (i < entityPtrs.size()) {
			bool movedToChild = false;
			if (northWest->insert(entityPtrs[i])) movedToChild = true;
			if (northEast->insert(entityPtrs[i])) movedToChild = true;
			if (southWest->insert(entityPtrs[i])) movedToChild = true;
			if (southEast->insert(entityPtrs[i])) movedToChild = true;
			if (movedToChild) {
				entityPtrs.erase(entityPtrs.begin() + i);
			} else {
				i++;
			}
		}
	}
	return true;
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
	platformerDrawAABBLines(&boundary, 0.125f, { 0, 121, 241, 128 });
	if (northWest == nullptr) {
		const char* text = TextFormat("Max Capacity: %.0f\nminPos: %.0f, %.0f\nmaxPos: %.0f, %.0f", (float)QuadTree::NODE_CAPACITY, boundary.min.x, boundary.min.y, boundary.max.x, boundary.max.y);
		int fontSize = (int)(boundary.max.x - boundary.min.x) / 10.0f;
		Vector2 textDimensions = MeasureTextEx(GetFontDefault(), text, (float)fontSize, TEXT_SPACING);
		platformerDrawText(text, (int)boundary.min.x, (int)boundary.min.y - 1, fontSize, { 0, 121, 241, 128 }, &textDimensions);
		return;
	}
	northWest->draw();
	northEast->draw();
	southWest->draw();
	southEast->draw();
}