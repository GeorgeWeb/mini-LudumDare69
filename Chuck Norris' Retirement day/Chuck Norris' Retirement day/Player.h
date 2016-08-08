#ifndef PLAYER_HGUARD
#define PLAYER_HGUARD
#ifdef PRAGMA_ONCE
	#pragma once
#endif

#include <Engine/Pixels2D/InputManager.h>
#include <Engine/Pixels2D/Camera2D.h>
#include <Engine/Pixels2D/AudioEngine.h>

#include "Human.h"
#include "Bullet.h"
//#include "Level.h"

class Level;
class Gun;

const float PLAYER_WIDTH = 110.0f;
const float PLAYER_SPEED = 5.0f;
const float PLAYER_HEALTH = 100.0f;

class Player : public Human
{
public:
	Player();
	~Player();

	void init(const glm::vec2 &position, const float &speed, const float &health, Pixels2D::InputManager *inputManager, Pixels2D::Camera2D *camera, std::vector<Bullet> *bullets);

	void addGun(Gun *gun);
	void changeGun(const int &gunID);

	void update(std::vector<std::string> levelData, std::vector<Human*> &humans, std::vector<Alien*> &aliens, const float &deltaTime) final override;

	const int &collideWithGun(std::vector<std::string> &levelData, int &gunID);
	
	void checkGunTiles(std::vector<std::string> &levelData, std::vector<glm::vec2> &collideTilePositions, const float &x, const float &y, int &gunID);

	// temporary
	bool wasPickedUpShotgun = false;

private:
	Pixels2D::InputManager *m_inputManager;
	Level *m_level;
	std::vector<Gun*> m_guns;
	int m_currentGunIndex;
	Pixels2D::Camera2D *m_mainCamera;
	Pixels2D::AudioEngine m_soundManager;
	std::vector<Bullet> *m_bullets;
};

#endif // !PLAYER_HGUARD