#include "Bullet.h"
#include "Agent.h"
#include "Human.h"
#include "Alien.h"
#include "Level.h"

#include <Engine/Pixels2D/ResourceManager.h>

#include <iostream>

Bullet::Bullet(const glm::vec2 &direction, const glm::vec2 &position, const float &damage, const float &speed)
	: m_direction(direction), m_position(position), m_damage(damage), m_speed(speed)
{}

Bullet::~Bullet()
{
}

bool Bullet::update(const std::vector<std::string> &levelData, const float &deltaTime)
{
	m_position += m_direction * m_speed * deltaTime;
	
	// handle collision ...
	return collideWithWorld(levelData);
}

void Bullet::draw(Pixels2D::SpriteBatch &spriteBatch)
{
	glm::vec4 destRect(m_position.x + BULLET_RADIUS, m_position.y + BULLET_RADIUS, BULLET_RADIUS * 2, BULLET_RADIUS * 2);
	const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

	Pixels2D::ColorRGBA8 color = Pixels2D::ColorRGBA8(75, 75, 75, 255);

	spriteBatch.draw(destRect, uvRect, Pixels2D::ResourceManager::getTexture("Textures/circle.png").id, 0.0f, color);
}

bool Bullet::collideWithAgent(Agent *agent)
{
	const float MIN_DISTANCE = AGENT_RADIUS + BULLET_RADIUS;

	glm::vec2 centerPosA = m_position;
	glm::vec2 centerPosB = agent->getPosition() + glm::vec2(AGENT_RADIUS);

	glm::vec2 distVec = centerPosA - centerPosB;

	float distance = glm::length(distVec);

	float collisionDepth = MIN_DISTANCE - distance;
	if (collisionDepth > 0)
	{
		return true;
	}

	return false;
}

bool Bullet::collideWithWorld(const std::vector<std::string> &levelData)
{
	glm::ivec2 gridPosition(floor(m_position.x / (float)TILE_WIDTH), floor(m_position.y / (float)TILE_WIDTH));

	// if we are outside the world, return ture => delete bullet
	if (gridPosition.x < 0 || gridPosition.x >= levelData[0].size() ||
		gridPosition.y < 0 || gridPosition.y >= levelData.size()) {
		return true;
	}

	return levelData[gridPosition.y][gridPosition.x] != '.'
		&& levelData[gridPosition.y][gridPosition.x] != 'f'
		&& levelData[gridPosition.y][gridPosition.x] != 'm'
		&& levelData[gridPosition.y][gridPosition.x] != 'G';
}