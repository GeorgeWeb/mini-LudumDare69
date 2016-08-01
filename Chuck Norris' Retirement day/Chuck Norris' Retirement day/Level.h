#ifndef LEVEL_HGUARD
#define LEVEL_HGUARD

#include <Engine/Pixels2D/SpriteBatch.h>

#include <string>
#include <vector>

const float TILE_WIDTH = 128.0f;

class Level
{
public:
	// load the level
	Level(const std::string &fileName);
	~Level();

	void draw();

	// getters
	inline glm::vec2 getPlayerStartPos() const { return m_playerStartPosition; }
	inline const std::vector<glm::vec2> &getAlienStartPositions() const { return m_alienStartPositions; }
	inline const std::vector<std::string> &getLevelData() const { return m_levelData; }
	inline const unsigned int &getNumHumans() const { return m_numHumans; }
	inline const unsigned int &getWidth() const { return m_levelData[0].size(); }
	inline const unsigned int &getHeight() const { return m_levelData.size(); }

private:
	std::vector<std::string> m_levelData;
	unsigned int m_numHumans;

	Pixels2D::SpriteBatch m_spriteBatch;

	glm::vec2 m_playerStartPosition;
	std::vector<glm::vec2> m_alienStartPositions;
};

#endif // !LEVEL_HGUARD