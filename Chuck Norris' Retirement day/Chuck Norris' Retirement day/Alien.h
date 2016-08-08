#ifndef ALIEN_HGUARD
#define ALIEN_HGUARD
#ifdef PRAGMA_ONCE
	#pragma once
#endif

#include "Agent.h"

const float ALIEN_WIDTH = 120.0f;
const float ALIEN_SPEED = 2.0f;
const float ALIEAN_HEALTH = 50.0f;
const float ALIEN_DAMAGE = 10.0f;

class Alien : public Agent
{
public:
	Alien();
	~Alien();

	void init(const glm::vec2 &position, const float &speed, const float &health);

	void update(std::vector<std::string> levelData, std::vector<Human*> &humans, std::vector<Alien*> &aliens, const float &deltaTime) final override;

private:
	Human *getNearestHuman(std::vector<Human*> &humans);
};

#endif // !ALIEN_HGUARD
