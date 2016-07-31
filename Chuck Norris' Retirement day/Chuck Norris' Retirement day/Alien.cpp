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
	_position = position;
	_speed = speed;
	_health = health;
	_color.setColor(0, 255, 0, 255);
}

void Alien::update(const std::vector<std::string> &levelData, std::vector<Human*> &humans, std::vector<Alien*> &aliens)
{
	Human *closestHuman = getNearestHuman(humans);

	if (closestHuman != nullptr)
	{
		glm::vec2 direction = glm::normalize(closestHuman->getPosition() - _position);
		_position += direction * _speed;
	}

	collideWithLevel(levelData);
}

Human *Alien::getNearestHuman(std::vector<Human*> &humans)
{
	Human* closestHuman = nullptr;

	float smallestDistance = 9999999.0f;

	for (auto human : humans)
	{
		glm::vec2 distVec = human->getPosition() - _position;
		float distance = glm::length(distVec);

		if (distance < smallestDistance)
		{
			smallestDistance = distance;
			closestHuman = human;
		}
	}

	return closestHuman;
}