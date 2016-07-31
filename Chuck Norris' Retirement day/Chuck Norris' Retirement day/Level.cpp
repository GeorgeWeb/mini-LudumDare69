#include "Level.h"

#include <Engine/Pixels2D/Errors.h>
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
	file >> tempStr >> _numHumans;

	// throw away the rest of the first line
	getline(file, tempStr);

	// read file and get each line
	while (getline(file, tempStr))
		_levelData.push_back(tempStr);

	_spriteBatch.init();
	_spriteBatch.begin();

	glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
	Pixels2D::ColorRGBA8 whiteColor(255,255,255,255);

	// render tiles
	for (int y = 0; y < _levelData.size(); y++)
	{
		for (int x = 0; x < _levelData[y].size(); x++)
		{
			// get the tile
			char tile = _levelData[y][x];

			// get dest rect
			glm::vec4 destRect(x * TILE_WIDTH, y * TILE_WIDTH, TILE_WIDTH, TILE_WIDTH);

			// process the tile
			switch (tile)
			{
				// environment
				case '.': ///<
					_spriteBatch.draw(destRect, uvRect, Pixels2D::ResourceManager::getTexture("Textures/Environment/floor_grass2.png").id, 0.0f, whiteColor);
					break;
				case 'B': ///<
				case 'R': ///<
					_spriteBatch.draw(destRect, uvRect, Pixels2D::ResourceManager::getTexture("Textures/red_bricks.png").id, 0.0f, whiteColor);
					break;
				case 'L': ///<
					_spriteBatch.draw(destRect, uvRect, Pixels2D::ResourceManager::getTexture("Textures/light_bricks.png").id, 0.0f, whiteColor);
					break;
				case 'G': ///<
					_spriteBatch.draw(destRect, uvRect, Pixels2D::ResourceManager::getTexture("Textures/glass.png").id, 0.0f, whiteColor);
					break;
				
				// agents
				case '@': ///< player
					_levelData[y][x] = '.'; // to avoid collision with aliens
					_playerStartPosition.x = x * TILE_WIDTH;
					_playerStartPosition.y = y * TILE_WIDTH;
					_spriteBatch.draw(destRect, uvRect, Pixels2D::ResourceManager::getTexture("Textures/Environment/floor_grass2.png").id, 0.0f, whiteColor);
					break;
				case 'A': ///< aliens
					_levelData[y][x] = '.'; // to avoid collision with player
					_alienStartPositions.emplace_back(x * TILE_WIDTH, y * TILE_WIDTH);
					_spriteBatch.draw(destRect, uvRect, Pixels2D::ResourceManager::getTexture("Textures/Environment/floor_grass2.png").id, 0.0f, whiteColor);
					break;
				
				default:
					printf("Unexpected symbol %c at (%d, %d)", tile, x, y);
			}
		}
	}

	_spriteBatch.end();
}

Level::~Level()
{
}

void Level::draw()
{
	_spriteBatch.renderBatch();
}