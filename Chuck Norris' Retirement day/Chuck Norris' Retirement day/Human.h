#ifndef HUMAN_HGUARD
#define HUMAN_HGUARD
#ifdef PRAGMA_ONCE
	#pragma once
#endif

#include "Agent.h"

// #include <SDL/SDL.h>
// const float DEG_TO_RAD = M_PI / 180.0f;
// const float RAD_TO_DEG = 180.0f / M_PI;
// no need of these since I found glm provides me with the glm::radians() function :)

const float HUMAN_WIDTH = 90.0f;
const float HUMAN_SPEED = 3.0f;
const float HUMAN_HEALTH = 20.0f;

class Human : public Agent
{
public:
	Human();
	virtual ~Human();

	void init(const glm::vec2 &position, const float &speed, const float &health);

	virtual void update(std::vector<std::string> levelData, std::vector<Human*> &humans, std::vector<Alien*> &aliens, const float &deltaTime) override;

private:
	float m_frames;
};

#endif // !HUMAN_HGUARD