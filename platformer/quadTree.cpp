#include "sahars_platformer.h"
#include <iostream>

QuadTree::QuadTree(AABB _boundary) {
	boundary = _boundary;
}
bool QuadTree::insert(Entity* entityPtr) {
	if (!areAABBsColliding(boundary, entityPtr->aabb)) return false;
	if (entities.size() < QuadTree::NODE_CAPACITY && northWest == nullptr) {
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
	SessionData::quadTree.quadTreeCollision();
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
		//collision(*entityPtrs[i], entityPtrs);
	}
}
void QuadTree::draw() const {
	DrawRectangleLinesEx(AABBtoRectangle(boundary), 10.0f, BLUE);
	if (northWest == nullptr) return;
	northWest->draw();
	northEast->draw();
	southWest->draw();
	southEast->draw();
}