#include "Level.h"

#include <Engine/Pixels2D/Pixels2DErrors.h>
#include <Engine/Pixels2D/ResourceManager.h>

#include <iostream>
#include <fstream>

// useful stuff
using std::string;
using std::ifstream;

Level::Level(const string &fileName)
{
	ifstream file;
	file.open(fileName);

	// error checking
	if (file.fail())
		Pixels2D::Errors::fatalError("Failed to open " + fileName);

	// throw away the first string in temp
	string tempStr;
	file >> tempStr >> m_numHumans;

	// throw away the rest of the first line
	getline(file, tempStr);

	// read file and get each line
	while (getline(file, tempStr))
		m_levelData.push_back(tempStr);

	m_spriteBatch.init();
	m_spriteBatch.begin();

	glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
	Pixels2D::ColorRGBA8 whiteColor(255,255,255,255);

	// render tiles
	for (int y = 0; y < m_levelData.size(); y++)
	{
		for (int x = 0; x < m_levelData[y].size(); x++)
		{
			// get the tile
			char tile = m_levelData[y][x];

			// get dest rect
			glm::vec4 destRect(x * TILE_WIDTH, y * TILE_WIDTH, TILE_WIDTH, TILE_WIDTH);
			// process the tile
			switch (tile)
			{
				// environment
				case '.':
					m_spriteBatch.draw(destRect, uvRect, Pixels2D::ResourceManager::getTexture("Textures/Environment/floor_grass2.png").id, 0.0f, whiteColor);
					break;
				case 'f':
					m_spriteBatch.draw(destRect, uvRect, Pixels2D::ResourceManager::getTexture("Textures/Environment/woodenfloor.png").id, 0.0f, whiteColor);
					break;
				case 'm':
					m_spriteBatch.draw(destRect, uvRect, Pixels2D::ResourceManager::getTexture("Textures/Environment/floor_grass1.png").id, 0.0f, whiteColor);
					break;
				case 'X':
					m_spriteBatch.draw(destRect, uvRect, Pixels2D::ResourceManager::getTexture("Textures/Environment/light_bricks.png").id, 0.0f, whiteColor);
					break;
				case 'B':
					m_spriteBatch.draw(destRect, uvRect, Pixels2D::ResourceManager::getTexture("Textures/Environment/light_bricks.png").id, 0.0f, whiteColor);
					break;
				case 'R':
					m_spriteBatch.draw(destRect, uvRect, Pixels2D::ResourceManager::getTexture("Textures/Environment/cryptwall.png").id, 0.0f, whiteColor);
					break;
				case 'L':
					m_spriteBatch.draw(destRect, uvRect, Pixels2D::ResourceManager::getTexture("Textures/Environment/redbrickwall.png").id, 0.0f, whiteColor);
					break;
				case 'G':
					m_spriteBatch.draw(destRect, uvRect, Pixels2D::ResourceManager::getTexture("Textures/Environment/glass.png").id, 0.0f, whiteColor);
					break;
				
				// agents
				case '@': ///< player
					m_levelData[y][x] = '.'; ///< to avoid collision with aliens
					m_playerStartPosition.x = x * TILE_WIDTH;
					m_playerStartPosition.y = y * TILE_WIDTH;
					m_spriteBatch.draw(destRect, uvRect, Pixels2D::ResourceManager::getTexture("Textures/Environment/floor_grass1.png").id, 0.0f, whiteColor);
					break;
				case 'A': ///< aliens
					m_levelData[y][x] = '.'; ///< to avoid collision with player
					m_alienStartPositions.emplace_back(x * TILE_WIDTH, y * TILE_WIDTH);
					m_spriteBatch.draw(destRect, uvRect, Pixels2D::ResourceManager::getTexture("Textures/Environment/floor_grass2.png").id, 0.0f, whiteColor);
					break;
				
				default:
					printf("Unexpected symbol %c at (%d, %d)", tile, x, y);
			}
		}
	}

	m_spriteBatch.end();
}

Level::~Level()
{
}

void Level::draw()
{
	m_spriteBatch.renderBatch();
}