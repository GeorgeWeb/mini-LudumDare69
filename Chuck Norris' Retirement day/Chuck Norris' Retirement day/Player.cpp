#include "Player.h"
#include "Gun.h"
#include "Level.h"

#include <Engine/Pixels2D/ResourceManager.h>

#include <SDL/SDL.h>

#include <algorithm>

Player::Player() : m_currentGunIndex(-1)
{}

Player::~Player()
{
	// Empty
}

void Player::init(const glm::vec2 &position, const float &speed, const float &health, Pixels2D::InputManager *inputManager, Pixels2D::Camera2D *camera, std::vector<Bullet> *bullets)
{
	m_position = position;
	m_speed = speed;
	m_health = health;
	
	m_color = Pixels2D::ColorRGBA8(255, 255, 255, 255);
	m_textureID = Pixels2D::ResourceManager::getTexture("Textures/Player/new-chuck.png").id;

	m_inputManager = inputManager;

	m_mainCamera = camera;

	m_bullets = bullets;
}

void Player::addGun(Gun* gun)
{
	// add the gun to player inventory
	m_guns.push_back(gun);

	// if no gun is equipped => equip this gun
	if (m_currentGunIndex == -1)
		m_currentGunIndex = 0;
}

void Player::changeGun(const int &gunID)
{
	if (m_currentGunIndex != -1 && gunID != -1)
		m_currentGunIndex = gunID;
}

void Player::update(std::vector<std::string> levelData, std::vector<Human*> &humans, std::vector<Alien*> &aliens, const float &deltaTime)
{
	if (m_inputManager->isKeyDown(SDLK_w) || m_inputManager->isKeyDown(SDLK_UP))
		m_position.y += m_speed;
	else if (m_inputManager->isKeyDown(SDLK_s) || m_inputManager->isKeyDown(SDLK_DOWN))
		m_position.y -= m_speed;

	if (m_inputManager->isKeyDown(SDLK_a) || m_inputManager->isKeyDown(SDLK_LEFT))
		m_position.x -= m_speed;
	else if (m_inputManager->isKeyDown(SDLK_d) || m_inputManager->isKeyDown(SDLK_RIGHT))
		m_position.x += m_speed;

	if (m_inputManager->isKeyPressed(SDLK_1) && m_guns.size() >= 0)
		m_currentGunIndex = 0;
	if (m_inputManager->isKeyPressed(SDLK_2) && m_guns.size() >= 1)
		m_currentGunIndex = 1;
	if (m_guns.size() >= 3)
	{
		if (m_inputManager->isKeyPressed(SDLK_3) && m_guns.size() >= 2)
			m_currentGunIndex = 2;
	}

	int gunID;
	if (collideWithGun(levelData, gunID) == 2)
	{
		wasPickedUpShotgun = true;	
		// changeGun(gunID);
	}

	glm::vec2 mouseCoords = m_inputManager->getMouseCoords();
	mouseCoords = m_mainCamera->convertScreenToWorld(mouseCoords);
	
	glm::vec2 centerPosition = m_position + glm::vec2(AGENT_RADIUS);

	m_direction = glm::normalize(mouseCoords - centerPosition);

	if (m_currentGunIndex != -1)
	{
		if (m_guns[m_currentGunIndex] == m_guns[0] || (m_guns.size() >= 3 && m_guns[m_currentGunIndex] == m_guns[2]))
			m_guns[m_currentGunIndex]->update(m_inputManager->isKeyPressed(SDL_BUTTON_LEFT), m_direction, centerPosition, *m_bullets, deltaTime);
		else
		{
			if(m_currentGunIndex == 2 && m_guns.size() >= 3)
				m_guns[m_currentGunIndex]->update(m_inputManager->isKeyDown(SDL_BUTTON_LEFT), m_direction, centerPosition, *m_bullets, deltaTime);
			else
				m_guns[1]->update(m_inputManager->isKeyDown(SDL_BUTTON_LEFT), m_direction, centerPosition, *m_bullets, deltaTime);
		}
	}
	
	collideWithLevel(levelData);
}

const int &Player::collideWithGun(std::vector<std::string> &levelData, int &gunID)
{
	std::vector<glm::vec2> collideGunTiles;
	
	/* check the 4 corners */

	// 1st corner
	checkGunTiles(levelData, collideGunTiles, m_position.x, m_position.y, gunID);
	// 2nd corner
	checkGunTiles(levelData, collideGunTiles, m_position.x + AGENT_WIDTH, m_position.y, gunID);
	// 3rd corner
	checkGunTiles(levelData, collideGunTiles, m_position.x, m_position.y + AGENT_WIDTH, gunID);
	// 4th corner
	checkGunTiles(levelData, collideGunTiles, m_position.x + AGENT_WIDTH, m_position.y + AGENT_WIDTH, gunID);

	Gun *shotgun = new Gun("Shotgun", 30, 12, 20.0f, 10, BULLET_SPEED, m_soundManager.loadSoundEffect("Sound/shots/shotgun.wav"));

	if (collideGunTiles.size() == 0)
		return -1;

	if (gunID == 2)
	{
		auto gun_it = std::find(m_guns.begin(), m_guns.end(), shotgun);
		
		if (gun_it == m_guns.end())
		{
			addGun(shotgun);
			return gunID;
		}
	}

	return -1;
}

void Player::checkGunTiles(std::vector<std::string> &levelData, std::vector<glm::vec2> &collideGunTiles, const float &x, const float &y, int &gunID)
{
	// get the position of this corner in grid-space
	glm::vec2 cornerPosition = glm::vec2(floor(x / (float)TILE_WIDTH), floor(y / (float)TILE_WIDTH));

	// if we are outside the world, just return
	if (cornerPosition.x < 0 || cornerPosition.x >= levelData[0].size() ||
		cornerPosition.y < 0 || cornerPosition.y >= levelData.size()) {
		return;
	}

	// if this is shotgun tile, pick it up
	if (levelData[cornerPosition.y][cornerPosition.x] == 's')
	{
		collideGunTiles.push_back(cornerPosition * (float)TILE_WIDTH + glm::vec2((float)TILE_WIDTH / 2.0f));
		//levelData[cornerPosition.y][cornerPosition.x] = '.';
		//m_level->replaceLevelTile(glm::vec2(cornerPosition.x, cornerPosition.y), levelData[cornerPosition.y][cornerPosition.x], '.');
		gunID = 2;
	}
	else gunID = m_currentGunIndex;
}