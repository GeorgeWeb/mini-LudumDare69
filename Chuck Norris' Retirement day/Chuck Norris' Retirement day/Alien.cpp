#include "Alien.h"
#include "Human.h"

Alien::Alien()
{
}

Alien::~Alien()
{
}

void Alien::init(const glm::vec2 &position, const float &speed, const float &health)
{
	m_position = position;
	m_speed = speed;
	m_health = health;
	m_color = Pixels2D::ColorRGBA8(0, 255, 0, 255);
}

void Alien::update(const std::vector<std::string> &levelData, std::vector<Human*> &humans, std::vector<Alien*> &aliens, const float &deltaTime)
{
	Human *closestHuman = getNearestHuman(humans);

	if (closestHuman != nullptr)
	{
		glm::vec2 direction = glm::normalize(closestHuman->getPosition() - m_position);
		m_position += direction * m_speed * deltaTime;
	}

	collideWithLevel(levelData);
}

Human *Alien::getNearestHuman(std::vector<Human*> &humans)
{
	Human* closestHuman = nullptr;

	float smallestDistance = 9999999.0f;

	for (auto human : humans)
	{
		glm::vec2 distVec = human->getPosition() - m_position;
		float distance = glm::length(distVec);

		if (distance < smallestDistance)
		{
			smallestDistance = distance;
			closestHuman = human;
		}
	}

	return closestHuman;
}