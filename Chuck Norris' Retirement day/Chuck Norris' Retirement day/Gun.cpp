#include "Gun.h"

#include <random>
#include <ctime>

#include <glm/gtx/rotate_vector.hpp>

Gun::Gun(const std::string &name, const unsigned int &fireRate, const unsigned int &bulletsPerShot,
	const float &spread, const float &bulletDamage, const float &bulletSpeed, Pixels2D::SoundEffect fireEffect)
	: m_name(name),
	m_fireRate(fireRate),
	m_bulletsPerShot(bulletsPerShot),
	m_spread(spread),
	m_bulletDamage(bulletDamage),
	m_bulletSpeed(bulletSpeed),
	m_frameCounter(0),
	m_fireEffect(fireEffect) 
{}

Gun::~Gun()
{
}

void Gun::update(const bool &isMouseDown, const glm::vec2 &direction, const glm::vec2 &position, std::vector<Bullet> &bullets, const float &deltaTime)
{
	m_frameCounter += 1.0f * deltaTime;
	if (m_frameCounter >= m_fireRate && isMouseDown)
	{
		fire(direction, position, bullets);
		m_frameCounter = 0;
	}
}

void Gun::fire(const glm::vec2 &direction, const glm::vec2 &position, std::vector<Bullet> &bullets)
{
	static std::mt19937 randomEngine(time(nullptr));
	std::uniform_real_distribution<float> randRotate(-m_spread, m_spread);
	
	m_fireEffect.play();

	for (int i = 0; i < m_bulletsPerShot; i++)
		bullets.emplace_back(glm::rotate(direction, glm::radians(randRotate(randomEngine))), position, m_bulletDamage, m_bulletSpeed);
}