#include "Agent.h"
#include "Human.h"
#include "Alien.h"
#include "Player.h"
#include "Level.h"

#include <Engine/Pixels2D/ResourceManager.h>

#include <math.h>
#include <algorithm>

Agent::Agent() : m_direction(1.0f, 0.0f)
{
}

Agent::~Agent()
{
}

// add different texture for humans than the one that the player uses.
void Agent::draw(Pixels2D::SpriteBatch &spriteBatch)
{
	const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
	const glm::vec4 destRect(m_position.x, m_position.y, AGENT_WIDTH, AGENT_WIDTH);

	spriteBatch.draw(destRect, uvRect, m_textureID, 0.0f, m_color, m_direction);
}

bool Agent::collideWithLevel(const std::vector<std::string> &levelData)
{
	std::vector<glm::vec2> collideTilePositions;

	/* check the 4 corners */

	// 1st corner
	checkTilePosition(levelData, collideTilePositions, m_position.x, m_position.y);
	// 2nd corner
	checkTilePosition(levelData, collideTilePositions, m_position.x + AGENT_WIDTH, m_position.y);
	// 3rd corner
	checkTilePosition(levelData, collideTilePositions, m_position.x, m_position.y + AGENT_WIDTH);
	// 4th corner
	checkTilePosition(levelData, collideTilePositions, m_position.x + AGENT_WIDTH, m_position.y + AGENT_WIDTH);

	// do the collision
	if (collideTilePositions.size() == 0)
		return false;

	for (auto tilePosition : collideTilePositions)
		collideWithTile(tilePosition);

	return true;

	// TODO:
	// sort the tiles based on distnace and update only the closest ones!
}

bool Agent::collideWithAgent(Agent *agent)
{
	// is the agent too far away in the x direction
	if (agent->m_position.x < m_position.x - AGENT_WIDTH)
		return false;
	else if (agent->m_position.x > m_position.x + AGENT_WIDTH)
		return false;

	// is the agent too far away in the y direction
	if (agent->m_position.y < m_position.y - AGENT_WIDTH)
		return false;
	else if (agent->m_position.y > m_position.y + AGENT_WIDTH)
		return false;

	const float MIN_DISTANCE = AGENT_RADIUS * 2.0f;

	glm::vec2 centerPosA = m_position + glm::vec2(AGENT_RADIUS);
	glm::vec2 centerPosB = agent->getPosition() + glm::vec2(AGENT_RADIUS);

	glm::vec2 distVec = centerPosA - centerPosB;

	float distance = glm::length(distVec);
	
	float collisionDepth = MIN_DISTANCE - distance;
	if (collisionDepth > 0)
	{
		glm::vec2 collisionDepthVec = glm::normalize(distVec) * collisionDepth;

		m_position += collisionDepthVec / 2.0f;
		agent->m_position -= collisionDepthVec / 2.0f;
		return true;
	}

	return false;
}

bool Agent::applyDamage(const float &damage)
{
	m_health -= damage;
	
	if (m_health <= 0.0f)
		return true;
	
	return false;
}

void Agent::checkTilePosition(const std::vector<std::string> &levelData, std::vector<glm::vec2> &collideTilePositions, const float &x, const float &y)
{
	// get the position of this corner in grid-space
	glm::vec2 cornerPosition = glm::vec2(floor(x / (float)TILE_WIDTH), floor(y / (float)TILE_WIDTH));
	
	// if we are outside the world, just return
	if (cornerPosition.x < 0 || cornerPosition.x >= levelData[0].size() ||
		cornerPosition.y < 0 || cornerPosition.y >= levelData.size()) {
		return;
	}
	
	// if this is not an ground tile types, we should collide with it
	if (levelData[cornerPosition.y][cornerPosition.x] != '.'
	&& levelData[cornerPosition.y][cornerPosition.x] != 'f'
	&& levelData[cornerPosition.y][cornerPosition.x] != 'm'
	&& levelData[cornerPosition.y][cornerPosition.x] != 'G')
		collideTilePositions.push_back(cornerPosition * (float)TILE_WIDTH + glm::vec2((float)TILE_WIDTH / 2.0f));
}

void Agent::collideWithTile(const glm::vec2 &tilePosition)
{
	const float TILE_RADIUS = (float)TILE_WIDTH / 2;
	const float MIN_DISTANCE = AGENT_RADIUS + TILE_RADIUS;

	glm::vec2 centerPlayerPosition = m_position + glm::vec2(AGENT_RADIUS);
	glm::vec2 distVec = centerPlayerPosition - tilePosition;

	float xDepth = MIN_DISTANCE - abs(distVec.x);
	float yDepth = MIN_DISTANCE - abs(distVec.y);

	// if this is ture then we collide
	if (xDepth > 0 || yDepth > 0)
	{
		if (std::max(xDepth, 0.0f) < std::max(yDepth, 0.0f))
		{
			if(distVec.x < 0)
				m_position.x -= xDepth;
			else
				m_position.x += xDepth;
		}
		else
		{
			if(distVec.y < 0)
				m_position.y -= yDepth;
			else
				m_position.y += yDepth;
		}
	}
}