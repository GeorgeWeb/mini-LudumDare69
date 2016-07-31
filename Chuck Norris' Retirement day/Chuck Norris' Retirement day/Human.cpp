#include "Human.h"

#include <glm/gtx/rotate_vector.hpp>

#include <random>
#include <ctime>

Human::Human() : _frames(0.0f)
{
}

Human::~Human()
{
}

void Human::init(const glm::vec2 &position, const float &speed, const float &health)
{
	// random number generator
	static std::mt19937 randomEngine(time(nullptr));
	static std::uniform_real_distribution<float> randDir(-1.0f, 1.0f);

	_color = Pixels2D::ColorRGBA8(255, 255, 255, 255);
	// initializations
	_position = position;
	_direction = glm::vec2(randDir(randomEngine), randDir(randomEngine));
	_speed = speed;
	_health = health;
	// make sure so that direction is not 0
	if (_direction.length() == 0) _direction = glm::vec2(1.0f, 0.0f);

	// normalize it => get unit length
	_direction = glm::normalize(_direction);
}

void Human::update(const std::vector<std::string> &levelData, std::vector<Human*> &humans, std::vector<Alien*> &aliens, const float &deltaTime)
{
	static std::mt19937 randomEngine(time(nullptr));
	static std::uniform_real_distribution<float> randRotate(-60.0f, 60.0f);

	_position += _direction * _speed * deltaTime;

	// humans change direction every few frames
	if (_frames == 60.0f)
	{
		_direction = glm::rotate(_direction, glm::radians(randRotate(randomEngine)));
		_frames = 0.0f;
	}
	else _frames += 1.0f;

	if(collideWithLevel(levelData))
		_direction = glm::rotate(_direction, glm::radians(randRotate(randomEngine)));
}