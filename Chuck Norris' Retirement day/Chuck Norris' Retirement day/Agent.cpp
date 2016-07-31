#include "Agent.h"
#include "Level.h"

#include <Engine/Pixels2D/ResourceManager.h>

#include <math.h>
#include <algorithm>

Agent::Agent()
{
}

Agent::~Agent()
{
}

// add different texture for humans than the one that the player uses.
void Agent::draw(Pixels2D::SpriteBatch &spriteBatch, const int &textureID)
{
	const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

	glm::vec4 destRect(_position.x, _position.y, AGENT_WIDTH, AGENT_WIDTH);
	
	spriteBatch.draw(destRect, uvRect, textureID, 0.0f, _color);
}

bool Agent::collideWithLevel(const std::vector<std::string> &levelData)
{
	std::vector<glm::vec2> collideTilePositions;

	/* check the 4 corners */

	// 1st corner
	checkTilePosition(levelData, collideTilePositions, _position.x, _position.y);
	// 2nd corner
	checkTilePosition(levelData, collideTilePositions, _position.x + AGENT_WIDTH, _position.y);
	// 3rd corner
	checkTilePosition(levelData, collideTilePositions, _position.x, _position.y + AGENT_WIDTH);
	// 4th corner
	checkTilePosition(levelData, collideTilePositions, _position.x + AGENT_WIDTH, _position.y + AGENT_WIDTH);

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
	if (agent->_position.x < _position.x - AGENT_WIDTH)
		return false;
	else if (agent->_position.x > _position.x + AGENT_WIDTH)
		return false;

	// is the agent too far away in the y direction
	if (agent->_position.y < _position.y - AGENT_WIDTH)
		return false;
	else if (agent->_position.y > _position.y + AGENT_WIDTH)
		return false;

	const float MIN_DISTANCE = AGENT_RADIUS * 2.0f;

	glm::vec2 centerPosA = _position + glm::vec2(AGENT_RADIUS);
	glm::vec2 centerPosB = agent->getPosition() + glm::vec2(AGENT_RADIUS);

	glm::vec2 distVec = centerPosA - centerPosB;

	float distance = glm::length(distVec);
	
	float collisionDepth = MIN_DISTANCE - distance;
	if (collisionDepth > 0)
	{
		glm::vec2 collisionDepthVec = glm::normalize(distVec) * collisionDepth;

		_position += collisionDepthVec / 2.0f;
		agent->_position -= collisionDepthVec / 2.0f;
		return true;
	}

	return false;
}

bool Agent::applyDamage(const float &damage)
{
	_health -= damage;
	
	if (_health <= 0.0f)
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
	
	// if this is not an air tile, we should collide with it
	if (levelData[cornerPosition.y][cornerPosition.x] != '.')
		collideTilePositions.push_back(cornerPosition * (float)TILE_WIDTH + glm::vec2((float)TILE_WIDTH / 2.0f));
}

void Agent::collideWithTile(const glm::vec2 &tilePosition)
{
	const float TILE_RADIUS = (float)TILE_WIDTH / 2;
	const float MIN_DISTANCE = AGENT_RADIUS + TILE_RADIUS;

	glm::vec2 centerPlayerPosition = _position + glm::vec2(AGENT_RADIUS);
	glm::vec2 distVec = centerPlayerPosition - tilePosition;

	float xDepth = MIN_DISTANCE - abs(distVec.x);
	float yDepth = MIN_DISTANCE - abs(distVec.y);

	// if this is ture then we collide
	if (xDepth > 0 || yDepth > 0)
	{
		if (std::max(xDepth, 0.0f) < std::max(yDepth, 0.0f))
		{
			if(distVec.x < 0)
				_position.x -= xDepth;
			else
				_position.x += xDepth;
		}
		else
		{
			if(distVec.y < 0)
				_position.y -= yDepth;
			else
				_position.y += yDepth;
		}
	}
}