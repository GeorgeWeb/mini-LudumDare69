#ifndef GUN_HGUARD
#define GUN_HGUARD

#include "Bullet.h"

#include <glm/glm.hpp>

#include <string>
#include <vector>

class Gun
{
public:
	Gun(const std::string &name, const unsigned int &fireRate, const unsigned int &bulletsPerShot, const float &spread, const float &bulletDamage, const float &bulletSpeed);
	~Gun();

	void update(const bool &isMouseDown, const glm::vec2 &direction, const glm::vec2 &position, std::vector<Bullet> &bullets);

private:
	void fire(const glm::vec2 &direction, const glm::vec2 &position, std::vector<Bullet> &bullets);

private:
	std::string _name;

	// fire rate in terms of frames
	unsigned int _fireRate;

	// the number of bullets fired at a time
	unsigned int _bulletsPerShot;

	// determines shot accuracy
	float _spread;

	float _bulletSpeed;

	float _bulletDamage;

	unsigned int _frameCounter;
};

#endif // !GUN_HGUARD