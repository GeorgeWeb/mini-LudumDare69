#ifndef BULLET_HGUARD
#define BULLET_HGUARD

#include <Engine/Pixels2D/SpriteBatch.h>

#include <glm/glm.hpp>

#include <vector>

class Alien;
class Human;
class Agent;

const int BULLET_RADIUS = 10;
const float BULLET_SPEED = 20.0f;

class Bullet
{
public:
	Bullet(const glm::vec2 &direction, const glm::vec2 &position, const float &damage, const float &speed);
	~Bullet();

	// on true => delete bullet
	bool update(const std::vector<std::string> &levelData, const float &deltaTime);

	void draw(Pixels2D::SpriteBatch &spriteBatch);
	
	// implementing simple spherical collision
	bool collideWithAgent(Agent *agent);
	
	// getters
	inline const float getDamage() const { return m_damage; }
	inline const glm::vec2 &getPosition() const { return m_position; }

private:
	bool collideWithWorld(const std::vector<std::string> &levelData);

private:
	glm::vec2 m_position;
	glm::vec2 m_direction;
	float m_damage;
	float m_speed;
};

#endif // !BULLET_HGUARD