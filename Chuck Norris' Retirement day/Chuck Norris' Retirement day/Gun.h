#ifndef GUN_HGUARD
#define GUN_HGUARD
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include "Bullet.h"

#include <Engine/Pixels2D/AudioEngine.h>

#include <glm/glm.hpp>

#include <string>
#include <vector>

class Gun
{
public:
	Gun(const std::string &name, const unsigned int &fireRate, const unsigned int &bulletsPerShot,
		const float &spread, const float &bulletDamage, const float &bulletSpeed, Pixels2D::SoundEffect fireEffect);
	~Gun();

	void update(const bool &isMouseDown, const glm::vec2 &direction, const glm::vec2 &position, std::vector<Bullet> &bullets, const float &deltaTime);

private:
	void fire(const glm::vec2 &direction, const glm::vec2 &position, std::vector<Bullet> &bullets);

private:
	std::string m_name;

	// fire rate in terms of frames
	unsigned int m_fireRate;

	// the number of bullets fired at a time
	unsigned int m_bulletsPerShot;

	// determines shot accuracy
	float m_spread;

	float m_bulletSpeed;

	float m_bulletDamage;

	float m_frameCounter;

	Pixels2D::SoundEffect m_fireEffect;
};

#endif // !GUN_HGUARD