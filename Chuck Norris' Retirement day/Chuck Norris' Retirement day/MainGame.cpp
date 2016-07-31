#include "MainGame.h"

#include <Engine/Pixels2D/Pixels2D.h>
#include <Engine/Pixels2D/Timing.h>
#include <Engine/Pixels2D/Errors.h>

#include "Gun.h"

#include <SDL/SDL.h>
#include <iostream>
#include <random>
#include <ctime>

MainGame::MainGame() : _screenWidth(1024), _screenHeight(768), _gameState(GameState::PLAY), _fps(0), _maxFPS(60.0f), _player(nullptr), _numHumansKilled(0), _numAliensKilled(0)
{
}

MainGame::~MainGame()
{
	if (!_levels.empty())
	{
		for (auto level : _levels)
			delete level;
	}

	if (!_humans.empty())
	{
		for (auto human : _humans)
			delete human;
	}

	if (!_aliens.empty())
	{
		for (auto alien : _aliens)
			delete alien;
	}
}

void MainGame::run()
{
	initSystems();

	initLevel();

	gameLoop();
}

void MainGame::initSystems()
{
	// initialize the game engine
	Pixels2D::init();

	// create the window
	_window.create("Chuck Norris' Retirement day", _screenWidth, _screenHeight, 0);

	// set background color
	glClearColor(0.17255, 0.27059, 0.40000, 0);

	// set up the shaders
	initShaders();

	// initialize a sprite batch
	_agentSpriteBatch.init();

	// set up the camera
	_camera.init(_screenWidth, _screenHeight);
}

void MainGame::initLevel()
{
	// level 1
	_levels.push_back(new Level("Levels/level1.txt"));
	_currentLevel = 0;

	// initialize player;
	_player = new Player();
	_player->init(_levels[_currentLevel]->getPlayerStartPos(), PLAYER_SPEED, PLAYER_HEALTH, &_inputManager, &_camera, &_bullets);

	// store humans
	_humans.push_back(_player);

	// random number generator
	std::mt19937 randomEngine;
	randomEngine.seed(time(nullptr));
	std::uniform_int_distribution<int> randX(2, _levels[_currentLevel]->getWidth() - 2);
	std::uniform_int_distribution<int> randY(2, _levels[_currentLevel]->getHeight() - 2);

	// add the humans
	for (unsigned int i = 0; i < _levels[_currentLevel]->getNumHumans(); i++)
	{
		_humans.push_back(new Human());
		glm::vec2 position = glm::vec2(randX(randomEngine) * TILE_WIDTH, randY(randomEngine) * TILE_WIDTH);
		_humans.back()->init(position, HUMAN_SPEED, HUMAN_HEALTH);
	}

	// add the aliens
	const std::vector<glm::vec2> &alienPositions = _levels[_currentLevel]->getAlienStartPositions();
	for (unsigned int i = 0; i < alienPositions.size(); i++)
	{
		_aliens.push_back(new Alien());
		_aliens.back()->init(alienPositions[i], HUMAN_SPEED, ALIEAN_HEALTH);
	}

	// Set up the players guns
	_player->addGun(new Gun("Revolver", 10, 1, 5.0f, 30, BULLET_SPEED));
	_player->addGun(new Gun("Shotgun", 30, 12, 20.0f, 4, BULLET_SPEED));
	_player->addGun(new Gun("MP5", 2, 1, 10.0f, 20, BULLET_SPEED));
}

void MainGame::initShaders()
{
    // Compile our color shader
    _textureProgram.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
    _textureProgram.addAttribute("vertexPosition");
    _textureProgram.addAttribute("vertexColor");
    _textureProgram.addAttribute("vertexUV");
    _textureProgram.linkShaders();
}

void MainGame::gameLoop()
{
	Pixels2D::FPSLimiter fpsLimiter;
	fpsLimiter.setMaxFPS(_maxFPS);

	// main loop
	while (_gameState == GameState::PLAY)
	{
		fpsLimiter.beginFrame();

		checkVictory();

		processInput();

		updateAgents();

		updateBullets();

		_camera.setPosition(_player->getPosition());

		_camera.update();		

		drawGame();

		_fps = fpsLimiter.endFrame();
	}
}

void MainGame::updateAgents()
{
	// update all humans
	for (auto human : _humans)
		human->update(_levels[_currentLevel]->getLevelData(), _humans, _aliens);

	// update all aliens
	for (auto alien : _aliens)
		alien->update(_levels[_currentLevel]->getLevelData(), _humans, _aliens);

	// update alien collisions
	for (unsigned int i = 0; i < _aliens.size(); i++)
	{
		// collide with other aliens
		for (unsigned int j = i + 1; j < _aliens.size(); j++)
			_aliens[i]->collideWithAgent(_aliens[j]);

		// collide with humans other than player
		for (unsigned int j = 1; j < _humans.size(); j++)
		{
			// turn the human into zombie
			if (_aliens[i]->collideWithAgent(_humans[j]))
			{
				// add the new alien
				_aliens.push_back(new Alien());
				_aliens.back()->init(_humans[j]->getPosition(), ALIEN_SPEED, ALIEAN_HEALTH);

				// delete the human
				delete _humans[j];
				_humans[j] = _humans.back();
				_humans.pop_back();
			}
		}

		// collide with player
		if (_aliens[i]->collideWithAgent(_player))
		{
			Pixels2D::Errors::fatalError("YOU LOSE!");
		}
	}

	// update human collisions
	for (unsigned int i = 0; i < _humans.size(); i++)
	{
		// collide with other humans
		for (unsigned int j = i + 1; j < _humans.size(); j++)
			_humans[i]->collideWithAgent(_humans[j]);
	}
}

void MainGame::updateBullets()
{
	// update and collide with world
	for (unsigned int i = 0; i < _bullets.size();)
	{
		// if true => the bullet collided with the world objects
		if (_bullets[i].update(_levels[_currentLevel]->getLevelData()))
		{
			// remove the bullet from the vector
			_bullets[i] = _bullets.back();
			_bullets.pop_back();
		}
		else i++;
	}

	bool wasBulletRemoved;

	// collide with humans and aliens
	for (int i = 0; i < _bullets.size(); i++)
	{
		wasBulletRemoved = false;
		// loop through aliens
		for (int j = 0; j < _aliens.size();)
		{
			// check collision
			if (_bullets[i].collideWithAgent(_aliens[j]))
			{
				// damage aliens, and kill it if its out of health
				if (_aliens[j]->applyDamage(_bullets[i].getDamage()))
				{
					// If the zombie died, remove him
					delete _aliens[j];
					_aliens[j] = _aliens.back();
					_aliens.pop_back();
					_numAliensKilled++;
				}
				else j++;

				// remove the bullet
				_bullets[i] = _bullets.back();
				_bullets.pop_back();
				wasBulletRemoved = true;
				// avoid skipping a bullet
				i--;
				// since the bullet died, no need to loop through any more aliens
				break;
			}
			else j++;
		}
		// Loop through humans
		if (wasBulletRemoved == false)
		{
			for (int j = 1; j < _humans.size();)
			{
				// check collision
				if (_bullets[i].collideWithAgent(_humans[j]))
				{
					// damage human, and kill it if its out of health
					if (_humans[j]->applyDamage(_bullets[i].getDamage()))
					{
						// If the human died, remove him
						delete _humans[j];
						_humans[j] = _humans.back();
						_humans.pop_back();
					}
					else j++;

					// remove the bullet
					_bullets[i] = _bullets.back();
					_bullets.pop_back();
					_numHumansKilled++;
					// avoid skipping a bullet
					i--;
					// since the bullet died, no need to loop through any more humans
					break;
				}
				else j++;
			}
		}
	}
}

void MainGame::checkVictory()
{
	// ...implement more levels...
	// _currentLevel++; initLevel(...);

	// if all aliens are dead => player wins
	if (_aliens.empty())
	{
		std::printf("*** YOU WIN! ***\n You killed %d humans and %d aliens. There are %d/%d civilians remaining.", 
			_numHumansKilled, _numAliensKilled, _humans.size() - 1, _levels[_currentLevel]->getNumHumans());

		Pixels2D::Errors::fatalError("");
	}
}

void MainGame::processInput()
{
    SDL_Event evnt;

    //Will keep looping until there are no more events to process
    while (SDL_PollEvent(&evnt))
	{
        switch (evnt.type)
		{
            case SDL_QUIT:
				_gameState = GameState::EXIT;
                break;
            case SDL_MOUSEMOTION:
                _inputManager.setMouseCoords(evnt.motion.x, evnt.motion.y);
                break;
            case SDL_KEYDOWN:
                _inputManager.pressKey(evnt.key.keysym.sym);
                break;
            case SDL_KEYUP:
                _inputManager.releaseKey(evnt.key.keysym.sym);
                break;
            case SDL_MOUSEBUTTONDOWN:
                _inputManager.pressKey(evnt.button.button);
                break;
            case SDL_MOUSEBUTTONUP:
                _inputManager.releaseKey(evnt.button.button);
                break;
        }
		
		// quit the game
		if (_inputManager.isKeyPressed(SDLK_ESCAPE))
			_gameState = GameState::EXIT;

		if (_inputManager.isKeyPressed(SDL_BUTTON_LEFT))
		{

		}
    }
}

void MainGame::drawGame()
{
    // Set the base depth to 1.0
    glClearDepth(1.0);
    // Clear the color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_textureProgram.use();

	glActiveTexture(GL_TEXTURE0);

	// make sure the shader uses texture 0
	GLint textureUniform = _textureProgram.getUniformLocation("mySampler");
	glUniform1i(textureUniform, 0);

   // get the (projection) camera matrix 
	glm::mat4 cameraMatrix = _camera.getCameraMatrix();
	GLint pUniform = _textureProgram.getUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &cameraMatrix[0][0]);


	// draw level
	_levels[_currentLevel]->draw();

	_agentSpriteBatch.begin();

	// human texture types
	static int playerTextureID = Pixels2D::ResourceManager::getTexture("Textures/Player/bro5_fire0004@2x.png").id;
	static int humanTextureID = Pixels2D::ResourceManager::getTexture("Textures/NPCs/human.png").id;
	// draw humans
	for (auto human : _humans)
	{
		if(human == _humans.front()) ///< if the human is the player
			human->draw(_agentSpriteBatch, playerTextureID);
		else ///<  the other humans that are NPCs
			human->draw(_agentSpriteBatch, humanTextureID);
	}

	// alien texture
	static int alienTextureID = Pixels2D::ResourceManager::getTexture("Textures/NPCs/zombie-scared-face.png").id;
	// draw aliens
	for (auto alien : _aliens)
		alien->draw(_agentSpriteBatch, alienTextureID);
	
	// bullet texture: ...
	// draw bullets
	for (auto bullet : _bullets)
		bullet.draw(_agentSpriteBatch);

	_agentSpriteBatch.end();

	_agentSpriteBatch.renderBatch();

	_textureProgram.unuse();

    // Swap our buffer and draw everything to the screen!
    _window.swapBuffer();
}