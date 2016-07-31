#ifndef BULLET_HGUARD
#define BULLET_HGUARD

#include <Engine/Pixels2D/SpriteBatch.h>

#include <glm/glm.hpp>

#include <vector>

class Alien;
class Human;
class Agent;

const int BULLET_RADIUS = 3;
const float BULLET_SPEED = 20.0f;

class Bullet
{
public:
	Bullet(const glm::vec2 &direction, const glm::vec2 &position, const float &damage, const float &speed);
	~Bullet();

	// on true => delete bullet
	bool update(const std::vector<std::string> &levelData);

	void draw(Pixels2D::SpriteBatch &spriteBatch);
	
	// implementing simple spherical collision
	bool collideWithAgent(Agent *agent);

	inline const float getDamage() const { return _damage; }

private:
	bool collideWithWorld(const std::vector<std::string> &levelData);

private:
	glm::vec2 _position;
	glm::vec2 _direction;
	float _damage;
	float _speed;
};

#endif // !BULLET_HGUARD