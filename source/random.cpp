#include "sahars_platformer.h"
#include <random>
static thread_local std::minstd_rand generator(std::random_device{}());
float Random::getFloat(float min, float max) {
	static thread_local std::uniform_real_distribution<float> dist;
	return dist(generator, decltype(dist)::param_type{ min, max });
}
int Random::getInt(int min, int max) {
	static thread_local std::uniform_int_distribution<int> dist;
	return dist(generator, decltype(dist)::param_type{ min, max });
}