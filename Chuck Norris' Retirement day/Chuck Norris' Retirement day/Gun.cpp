#include "Gun.h"

#include <random>
#include <ctime>

#include <glm/gtx/rotate_vector.hpp>

Gun::Gun(const std::string &name, const unsigned int &fireRate, const unsigned int &bulletsPerShot, const float &spread, const float &bulletDamage, const float &bulletSpeed)
	: _name(name), _fireRate(fireRate), _bulletsPerShot(bulletsPerShot), _spread(spread), _bulletDamage(bulletDamage), _bulletSpeed(bulletSpeed),
	_frameCounter(0)
{}

Gun::~Gun()
{
}

void Gun::update(const bool &isMouseDown, const glm::vec2 &direction, const glm::vec2 &position, std::vector<Bullet> &bullets)
{
	_frameCounter++;
	if (_frameCounter >= _fireRate && isMouseDown)
	{
		fire(direction, position, bullets);
		_frameCounter = 0;
	}
}

void Gun::fire(const glm::vec2 &direction, const glm::vec2 &position, std::vector<Bullet> &bullets)
{
	static std::mt19937 randomEngine(time(nullptr));
	std::uniform_real_distribution<float> randRotate(-_spread, _spread);

	for (int i = 0; i < _bulletsPerShot; i++)
		bullets.emplace_back(glm::rotate(direction, glm::radians(randRotate(randomEngine))), position, _bulletDamage, _bulletSpeed);
}