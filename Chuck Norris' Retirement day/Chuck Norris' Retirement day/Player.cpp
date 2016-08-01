#include "Player.h"
#include "Gun.h"

#include <SDL/SDL.h>

Player::Player() : _currentGunIndex(-1)
{}

Player::~Player()
{
	// delete bullets ptr
}

void Player::init(const glm::vec2 &position, const float &speed, const float &health, Pixels2D::InputManager *inputManager, Pixels2D::Camera2D *camera, std::vector<Bullet> *bullets)
{
	m_position = position;
	m_speed = speed;
	m_health = health;
	m_color = Pixels2D::ColorRGBA8(255, 255, 255, 255);

	_inputManager = inputManager;

	m_mainCamera = camera;

	m_bullets = bullets;
}

void Player::addGun(Gun* gun)
{
	// add the gun to player inventory
	m_guns.push_back(gun);

	// if no gun is equipped => equip this gun
	if (_currentGunIndex == -1)
		_currentGunIndex = 0;
}

void Player::update(const std::vector<std::string> &levelData, std::vector<Human*> &humans, std::vector<Alien*> &aliens, const float &deltaTime)
{
	if (_inputManager->isKeyDown(SDLK_w) || _inputManager->isKeyDown(SDLK_UP))
		m_position.y += m_speed;
	else if (_inputManager->isKeyDown(SDLK_s) || _inputManager->isKeyDown(SDLK_DOWN))
		m_position.y -= m_speed;

	if (_inputManager->isKeyDown(SDLK_a) || _inputManager->isKeyDown(SDLK_LEFT))
		m_position.x -= m_speed;
	else if (_inputManager->isKeyDown(SDLK_d) || _inputManager->isKeyDown(SDLK_RIGHT))
		m_position.x += m_speed;

	if (_inputManager->isKeyPressed(SDLK_1) && m_guns.size() >= 0)
		_currentGunIndex = 0;
	else if (_inputManager->isKeyPressed(SDLK_2) && m_guns.size() >= 1)
		_currentGunIndex = 1;
	else if (_inputManager->isKeyPressed(SDLK_3) && m_guns.size() >= 2)
		_currentGunIndex = 2;

	if (_currentGunIndex != -1) {

		glm::vec2 mouseCoords = _inputManager->getMouseCoords();
		mouseCoords = m_mainCamera->convertScreenToWorld(mouseCoords);

		glm::vec2 centerPosition = m_position + glm::vec2(AGENT_RADIUS);

		glm::vec2 direction = glm::normalize(mouseCoords - centerPosition);

		if(m_guns[_currentGunIndex] == m_guns[0] || m_guns[_currentGunIndex] == m_guns[1])
			m_guns[_currentGunIndex]->update(_inputManager->isKeyPressed(SDL_BUTTON_LEFT), direction, centerPosition, *m_bullets, deltaTime);
		else
			m_guns[_currentGunIndex]->update(_inputManager->isKeyDown(SDL_BUTTON_LEFT), direction, centerPosition, *m_bullets, deltaTime);
	}
	
	collideWithLevel(levelData);
}