#ifndef LEVEL_HGUARD
#define LEVEL_HGUARD

#include <Engine/Pixels2D/SpriteBatch.h>

#include <string>
#include <vector>

const float TILE_WIDTH = 96.0f;

class Level
{
public:
	// load the level
	Level(const std::string &fileName);
	~Level();

	void draw();

	// getters
	inline glm::vec2 getPlayerStartPos() const { return _playerStartPosition; }
	inline const std::vector<glm::vec2> &getAlienStartPositions() const { return _alienStartPositions; }
	inline const std::vector<std::string> &getLevelData() const { return _levelData; }
	inline const unsigned int &getNumHumans() const { return _numHumans; }
	inline const unsigned int &getWidth() const { return _levelData[0].size(); }
	inline const unsigned int &getHeight() const { return _levelData.size(); }

private:
	std::vector<std::string> _levelData;
	unsigned int _numHumans;

	Pixels2D::SpriteBatch _spriteBatch;

	glm::vec2 _playerStartPosition;
	std::vector<glm::vec2> _alienStartPositions;
};

#endif // !LEVEL_HGUARD