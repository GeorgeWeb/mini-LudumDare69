#ifndef AGENT_HGUARD
#define AGENT_HGUARD

#include <Engine/Pixels2D/SpriteBatch.h>

#include <glm/glm.hpp>

const float AGENT_WIDTH = 92.0f;
const float AGENT_RADIUS = AGENT_WIDTH / 2.0f;

class Alien;
class Human;

class Agent
{
public:
	Agent();
	virtual ~Agent();

	virtual void update(const std::vector<std::string> &levelData, std::vector<Human*> &humans, std::vector<Alien*> &aliens) = 0;

	bool collideWithLevel(const std::vector<std::string> &levelData);

	bool collideWithAgent(Agent *agent);

	void draw(Pixels2D::SpriteBatch &spriteBatch, const int &textureID);

	// true if died
	bool applyDamage(const float &damage);

	// getters
	inline const glm::vec2 &getPosition() const { return _position; }

protected:
	void checkTilePosition(const std::vector<std::string> &levelData, std::vector<glm::vec2> &collideTilePositions, const float &x, const float &y);

	// implementing Axis Aligned Bounding Box collision
	void collideWithTile(const glm::vec2 &tilePosition);
public:
	glm::vec2 _position;
	Pixels2D::Color _color;
	float _speed;
	float _health;
};

#endif // !AGENT_HGUARD
