#ifndef ALIEN_HGUARD
#define ALIEN_HGUARD

#include "Agent.h"

const float ALIEN_SPEED = 1.0f;
const float ALIEAN_HEALTH = 120.0f;

class Alien : public Agent
{
public:
	Alien();
	~Alien();

	void init(const glm::vec2 &position, const float &speed, const float &health);

	void update(const std::vector<std::string> &levelData, std::vector<Human*> &humans, std::vector<Alien*> &aliens, const float &deltaTime) final override;

private:
	Human *getNearestHuman(std::vector<Human*> &humans);
};

#endif // !ALIEN_HGUARD
