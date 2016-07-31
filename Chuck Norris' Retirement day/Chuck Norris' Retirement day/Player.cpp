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
	_position = position;
	_speed = speed;
	_health = health;
	_color = Pixels2D::ColorRGBA8(255, 255, 255, 255);

	_inputManager = inputManager;

	_camera = camera;

	_bullets = bullets;
}

void Player::addGun(Gun* gun)
{
	// add the gun to player inventory
	_guns.push_back(gun);

	// if no gun is equipped => equip this gun
	if (_currentGunIndex == -1)
		_currentGunIndex = 0;
}

void Player::update(const std::vector<std::string> &levelData, std::vector<Human*> &humans, std::vector<Alien*> &aliens, const float &deltaTime)
{
	if (_inputManager->isKeyDown(SDLK_w) || _inputManager->isKeyDown(SDLK_UP))
		_position.y += _speed;
	else if (_inputManager->isKeyDown(SDLK_s) || _inputManager->isKeyDown(SDLK_DOWN))
		_position.y -= _speed;

	if (_inputManager->isKeyDown(SDLK_a) || _inputManager->isKeyDown(SDLK_LEFT))
		_position.x -= _speed;
	else if (_inputManager->isKeyDown(SDLK_d) || _inputManager->isKeyDown(SDLK_RIGHT))
		_position.x += _speed;

	if (_inputManager->isKeyPressed(SDLK_1) && _guns.size() >= 0)
		_currentGunIndex = 0;
	else if (_inputManager->isKeyPressed(SDLK_2) && _guns.size() >= 1)
		_currentGunIndex = 1;
	else if (_inputManager->isKeyPressed(SDLK_3) && _guns.size() >= 2)
		_currentGunIndex = 2;

	if (_currentGunIndex != -1) {

		glm::vec2 mouseCoords = _inputManager->getMouseCoords();
		mouseCoords = _camera->convertScreenToWorld(mouseCoords);

		glm::vec2 centerPosition = _position + glm::vec2(AGENT_RADIUS);

		glm::vec2 direction = glm::normalize(mouseCoords - centerPosition);

		if(_guns[_currentGunIndex] == _guns[0] || _guns[_currentGunIndex] == _guns[1])
			_guns[_currentGunIndex]->update(_inputManager->isKeyPressed(SDL_BUTTON_LEFT), direction, centerPosition, *_bullets, deltaTime);
		else
			_guns[_currentGunIndex]->update(_inputManager->isKeyDown(SDL_BUTTON_LEFT), direction, centerPosition, *_bullets, deltaTime);
	}
	
	collideWithLevel(levelData);
}