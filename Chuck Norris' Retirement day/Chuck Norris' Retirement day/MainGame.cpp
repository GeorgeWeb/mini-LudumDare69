#include "MainGame.h"
#include "Gun.h"

#include <Engine/Pixels2D/Pixels2D.h>
#include <Engine/Pixels2D/Timing.h>
#include <Engine/Pixels2D/Pixels2DErrors.h>

#include <SDL/SDL.h>

#include <glm/gtx/rotate_vector.hpp>

#include <iostream>
#include <random>
#include <algorithm>
#include <string>
#include <ctime>
#include <cstdio>
#include <cstring>

MainGame::MainGame() : m_screenWidth(1280), m_screenHeight(720), m_gameState(GameState::PLAY), m_fps(0), m_maxFPS(60.0f), 
					   m_player(nullptr), m_numHumansKilled(0), m_numAliensKilled(0)
{
	// Empty
}

MainGame::~MainGame()
{
	for (auto level : m_levels)
		delete level;

	m_levels.clear();

	for (auto human : m_humans)
		delete human;
		
	m_humans.clear();

	for (auto alien : m_aliens)
		delete alien;
		
	m_aliens.clear();
}

void MainGame::run()
{
	initSystems();

	initLevel(new Level("Levels/level1.txt"), m_currentLevel);
	
	// load and play the music
	// - 1 => loop forever
	m_audioEngine.loadMusic("Sound/XYZ.ogg").play(-1);
	gameLoop();
}

void MainGame::initSystems()
{
	// initialize the game engine
	Pixels2D::init();

	// initialize sound
	m_audioEngine.init();

	// create the window
	m_window.create("Chuck Norris' Retirement day", m_screenWidth, m_screenHeight, 0);

	// set background color
	glClearColor(0.17255, 0.27059, 0.40000, 0);
	//glClearColor(0, 0, 0, 0);

	// set up the shaders
	initShaders();

	// initialize the sprite batch(es)
	m_agentSpriteBatch.init();
	m_hudSpriteBatch.init();

	// initialize the sprite font
	m_spriteFont = new Pixels2D::SpriteFont("Fonts/Consolas.ttf", 32);

	// set up the main camera
	m_mainCamera.init(m_screenWidth, m_screenHeight);

	// set up the hud camera
	m_hudCamera.init(m_screenWidth, m_screenHeight);
	m_hudCamera.setPosition(glm::vec2(m_screenWidth /2, m_screenHeight / 2));

	// initialize particles
	m_bloodParticleBatch = new Pixels2D::ParticleBatch2D();
	m_bloodParticleBatch->init(1000, 0.05f, Pixels2D::ResourceManager::getTexture("Textures/particle.png").id, 
		[] (Pixels2D::Particle2D& particle, const float &deltaTime)
		{
			particle.position += particle.velocity * deltaTime;
			particle.color.a = (GLubyte)(particle.life * 255.0f);
			// alpha fading not gonna work in release mode, 
			// I dunno why but there I've got coloring problems
		}
	);
	m_particleEngine.addParticleBatch(m_bloodParticleBatch);
}

void MainGame::initLevel(Level *level, const int &levelID)
{
	// level 1
	m_levels.push_back(level);
	if (levelID != 0)
		m_currentLevel = levelID;
	else
		m_currentLevel = 0;

	std::cout << m_currentLevel << std::endl;

	// initialize player;
	m_player = new Player();
	m_player->init(m_levels[m_currentLevel]->getPlayerStartPos(), PLAYER_SPEED, PLAYER_HEALTH, &m_inputManager, &m_mainCamera, &m_bullets);

	// store humans
	m_humans.push_back(m_player);

	// random number generator
	std::mt19937 randomEngine;
	randomEngine.seed(time(nullptr));
	std::uniform_int_distribution<int> randX(2, m_levels[m_currentLevel]->getWidth() - 2);
	std::uniform_int_distribution<int> randY(2, m_levels[m_currentLevel]->getHeight() - 2);

	// add the humans
	for (unsigned int i = 0; i < m_levels[m_currentLevel]->getNumHumans(); i++)
	{
		m_humans.push_back(new Human());
		glm::vec2 position = glm::vec2(randX(randomEngine) * TILE_WIDTH, randY(randomEngine) * TILE_WIDTH);
		m_humans.back()->init(position, HUMAN_SPEED, HUMAN_HEALTH);
	}

	// add the aliens
	const std::vector<glm::vec2> &alienPositions = m_levels[m_currentLevel]->getAlienStartPositions();
	for (unsigned int i = 0; i < alienPositions.size(); i++)
	{
		m_aliens.push_back(new Alien());
		m_aliens.back()->init(alienPositions[i], HUMAN_SPEED, ALIEAN_HEALTH);
	}

	// Set up the players guns
	m_player->addGun(new Gun("Revolver", 20, 1, 1.0f, 50, BULLET_SPEED, m_audioEngine.loadSoundEffect("Sound/shots/pistol.wav")));
	m_player->addGun(new Gun("MP5", 5, 1, 1.0f, 40, BULLET_SPEED, m_audioEngine.loadSoundEffect("Sound/shots/MP5_gun.wav")));
}

void MainGame::initShaders()
{
    // Compile our color shader
    m_textureProgram.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
    m_textureProgram.addAttribute("vertexPosition");
    m_textureProgram.addAttribute("vertexColor");
    m_textureProgram.addAttribute("vertexUV");
    m_textureProgram.linkShaders();
}

void MainGame::gameLoop()
{
	// max number of physics steps per frame
	const int MAX_PHYSICS_STEPS = 6;
	// number of milliseconds in a second
	const float MS_PER_SECOND = 1000.0f;
	// the desired frame time per frame
	const float DESIRED_FRAMETIME = MS_PER_SECOND / m_maxFPS;
	// maximum size of deltaTime
	const float MAX_DELTA_TIME = 1.0f;
	
	// used to cap the FPS
	Pixels2D::FPSLimiter fpsLimiter;
	fpsLimiter.setMaxFPS(m_maxFPS);

	// zoom out the cameras by 4x
	const float CAMERA_SCALE = 1.0f / 3.0f;
	m_mainCamera.setScale(CAMERA_SCALE);
	// _mainCamera.setScale(CAMERA_SCALE);

	// start our previousTicks variable
	float previousTicks = SDL_GetTicks();

	// main loop
	while (m_gameState == GameState::PLAY)
	{
		fpsLimiter.beginFrame();

		float newTicks = SDL_GetTicks();
		float frameTime = newTicks - previousTicks;
		previousTicks = newTicks;
		float totalDeltaTime = frameTime / DESIRED_FRAMETIME;		

		checkVictory();

		m_inputManager.update();

		processInput();

		// makes sure not to spiral to death!
		int i = 0;
		// loop while still have steps to process.
		while (totalDeltaTime > 0.0f && i < MAX_PHYSICS_STEPS)
		{
			// the deltaTime should be the the smaller of the totalDeltaTime and MAX_DELTA_TIME
			float deltaTime = std::min(totalDeltaTime, MAX_DELTA_TIME);
			// update all physics here and pass in deltaTime
			updateAgents(deltaTime);
			updateBullets(deltaTime);
			m_particleEngine.update(deltaTime);
			// subtract from totalDeltaTime (the length we took)
			totalDeltaTime -= deltaTime;
			// increment the frame counter
			i++;
		}

		m_mainCamera.setPosition(m_player->getPosition());
		m_mainCamera.update();
		m_hudCamera.update();

		drawGame();

		m_fps = fpsLimiter.endFrame();
	}
}

void MainGame::updateAgents(const float &deltaTime)
{
	// update all humans
	for (auto human : m_humans)
		human->update(m_levels[m_currentLevel]->getLevelData(), m_humans, m_aliens, deltaTime);

	// update all aliens
	for (auto alien : m_aliens)
		alien->update(m_levels[m_currentLevel]->getLevelData(), m_humans, m_aliens, deltaTime);

	// update alien collisions
	for (unsigned int i = 0; i < m_aliens.size(); i++)
	{
		// collide with other aliens
		for (unsigned int j = i + 1; j < m_aliens.size(); j++)
			m_aliens[i]->collideWithAgent(m_aliens[j]);

		// collide with humans other than player
		for (unsigned int j = 1; j < m_humans.size(); j++)
		{
			// turn the human into alien
			if (m_aliens[i]->collideWithAgent(m_humans[j]))
			{
				static unsigned int attackSpeedForHumans = 0;
				attackSpeedForHumans++;
				if (attackSpeedForHumans == 60)
				{
					if (m_humans[j]->applyDamage(ALIEN_DAMAGE))
					{
						// add the new alien
						m_aliens.push_back(new Alien());
						m_aliens.back()->init(m_humans[j]->getPosition(), ALIEN_SPEED, ALIEAN_HEALTH);

						// delete the human
						delete m_humans[j];
						m_humans[j] = m_humans.back();
						m_humans.pop_back();
					}
					attackSpeedForHumans = 0;
				}
			}
		}

		// collide with player
		if (m_aliens[i]->collideWithAgent(m_player))
		{
			static unsigned int attackSpeedForPlayer = 0;
			attackSpeedForPlayer++;
			if (attackSpeedForPlayer == 60)
			{
				if (m_player->applyDamage(ALIEN_DAMAGE))
				{
					// remove the player
					//delete m_player;
					m_humans.erase(m_humans.begin());
					m_numHumansKilled++;

					// lose the game
					m_loss = true;
				}
				attackSpeedForPlayer = 0;
			}
		}
	}

	// update human collisions
	for (unsigned int i = 0; i < m_humans.size(); i++)
	{
		// collide with other humans
		for (unsigned int j = i + 1; j < m_humans.size(); j++)
			m_humans[i]->collideWithAgent(m_humans[j]);
	}
}

void MainGame::updateBullets(const float &deltaTime)
{
	// update and collide with world
	for (unsigned int i = 0; i < m_bullets.size();)
	{
		// if true => the bullet collided with the world objects
		if (m_bullets[i].update(m_levels[m_currentLevel]->getLevelData(), deltaTime))
		{
			// remove the bullet from the vector
			m_bullets[i] = m_bullets.back();
			m_bullets.pop_back();
		}
		else i++;
	}

	bool wasBulletRemoved;

	// collide with humans and aliens
	for (int i = 0; i < m_bullets.size(); i++)
	{
		wasBulletRemoved = false;
		// loop through aliens
		for (int j = 0; j < m_aliens.size();)
		{
			// check collision
			if (m_bullets[i].collideWithAgent(m_aliens[j]))
			{
				// add blood
				addBlood(m_bullets[i].getPosition(), 5);

				// damage aliens, and kill it if its out of health
				if (m_aliens[j]->applyDamage(m_bullets[i].getDamage()))
				{
					// If the zombie died, remove him
					delete m_aliens[j];
					m_aliens[j] = m_aliens.back();
					m_aliens.pop_back();
					m_numAliensKilled++;
				}
				else j++;

				// remove the bullet
				m_bullets[i] = m_bullets.back();
				m_bullets.pop_back();
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
			for (int j = 1; j < m_humans.size();)
			{
				// check collision
				if (m_bullets[i].collideWithAgent(m_humans[j]))
				{
					// add blood
					addBlood(m_bullets[i].getPosition(), 5);

					// damage human, and kill it if its out of health
					if (m_humans[j]->applyDamage(m_bullets[i].getDamage()))
					{
						// If the human died, remove him
						delete m_humans[j];
						m_humans[j] = m_humans.back();
						m_humans.pop_back();
					}
					else j++;

					// remove the bullet
					m_bullets[i] = m_bullets.back();
					m_bullets.pop_back();
					m_numHumansKilled++;
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
	/*
	if (m_aliens.empty())
	{
		m_levels.pop_back();
		m_currentLevel++;
		std::cout << m_currentLevel << std::endl;
		initLevel(new Level("Levels/level" + std::to_string(m_currentLevel) + ".txt"), m_currentLevel);
	}
	*/
		
	// if all aliens are dead => player wins
	if (m_aliens.empty())
	{
		m_victory = true;
		//std::printf("*** YOU WIN! ***\n You killed %d humans and %d aliens. There are %d/%d civilians remaining.", 
			//m_numHumansKilled, m_numAliensKilled, m_humans.size() - 1, m_levels[m_currentLevel]->getNumHumans());

		//Pixels2D::Errors::fatalError("");


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
				m_gameState = GameState::EXIT;
                break;
            case SDL_MOUSEMOTION:
                m_inputManager.setMouseCoords(evnt.motion.x, evnt.motion.y);
                break;
            case SDL_KEYDOWN:
                m_inputManager.pressKey(evnt.key.keysym.sym);
                break;
            case SDL_KEYUP:
                m_inputManager.releaseKey(evnt.key.keysym.sym);
                break;
            case SDL_MOUSEBUTTONDOWN:
                m_inputManager.pressKey(evnt.button.button);
                break;
            case SDL_MOUSEBUTTONUP:
                m_inputManager.releaseKey(evnt.button.button);
                break;
        }
		
		// quit the game
		if (m_inputManager.isKeyDown(SDLK_ESCAPE))
			m_gameState = GameState::EXIT;

		if (m_inputManager.isKeyDown(SDL_BUTTON_LEFT))
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

	m_textureProgram.use();

	glActiveTexture(GL_TEXTURE0);

	// make sure the shader uses texture 0
	GLint textureUniform = m_textureProgram.getUniformLocation("mySampler");
	glUniform1i(textureUniform, 0);

	// get the (projection) camera matrix 
	glm::mat4 cameraMatrix = m_mainCamera.getCameraMatrix();
	GLint pUniform = m_textureProgram.getUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &cameraMatrix[0][0]);

	// draw level
	m_levels[m_currentLevel]->draw();

	m_agentSpriteBatch.begin();
	m_hudSpriteBatch.init();

	const glm::vec2 agentDims(AGENT_RADIUS * 2.0f);

	// draw humans and player(as the first human)
	for (auto &human : m_humans)
	{
		// check if all humansa are in box view
		if (m_mainCamera.isBoxInView(human->getPosition(), agentDims))
			human->draw(m_agentSpriteBatch);
	}

	// draw aliens
	for (auto &alien : m_aliens)
	{
		// check if all aliens are in box view
		if (m_mainCamera.isBoxInView(alien->getPosition(), agentDims))
			alien->draw(m_agentSpriteBatch);
	}
	
	// draw bullets
	for (auto &bullet : m_bullets)
		bullet.draw(m_agentSpriteBatch);

	// end sprite batch creation
	m_agentSpriteBatch.end();
	// render to the screen
	m_agentSpriteBatch.renderBatch();

	// render the particles
	m_particleEngine.draw(&m_agentSpriteBatch);

	// draw the HUD
	drawHUD();

	m_textureProgram.unuse();

    // swap our buffer and draw everything to the screen
    m_window.swapBuffer();
}

void MainGame::drawHUD()
{
	char buffer[256];

	glm::mat4 projectionMatrix = m_hudCamera.getCameraMatrix();
	GLint pUniform = m_textureProgram.getUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	m_hudSpriteBatch.begin();

	if (m_victory)
	{
		static unsigned int lossTimer = 0;
		lossTimer++;
		if (lossTimer < 30)
		{
			sprintf_s(buffer, "Wooo You Won!");
			m_spriteFont->draw(m_hudSpriteBatch, buffer, glm::vec2(m_screenWidth / 2.0f - 75.0f, m_screenHeight - 50.0f),
				glm::vec2(1.0f), 0.0f, Pixels2D::ColorRGBA8(255, 255, 255, 255));

			if(m_humans.size() != 1)
				sprintf_s(buffer, 256, "You got %d humans left in town!", m_humans.size());
			else
				sprintf_s(buffer, 256, "You got %d human left in town!", m_humans.size());

			m_spriteFont->draw(m_hudSpriteBatch, buffer, glm::vec2(m_screenWidth / 2.0f - 250.0f, 5.0f), glm::vec2(1.0f), 0.0f, Pixels2D::ColorRGBA8(255, 255, 255, 255));
		}
		else Quit(buffer);
	}
	else if (m_loss)
	{
		static unsigned int lossTimer = 0;
		lossTimer++;
		if (lossTimer < 30)
		{
			sprintf_s(buffer, 256, "Shit You Died!");
			m_spriteFont->draw(m_hudSpriteBatch, buffer, glm::vec2(m_screenWidth / 2.0f - 75.0f, m_screenHeight - 50.0f),
				glm::vec2(1.0f), 0.0f, Pixels2D::ColorRGBA8(255, 255, 255, 255));
		}
		else Quit(buffer);
	}
	else
	{
		// game version gui
		glm::ivec3 version(1, 1, 0);
		sprintf_s(buffer, 256, "v%d.%d.%d", version.x, version.y, version.z);
		m_spriteFont->draw(m_hudSpriteBatch, buffer, glm::vec2(m_screenWidth - 75.0f, 10.0f), glm::vec2(0.5f), 0.0f, Pixels2D::ColorRGBA8(255, 255, 255, 255));

		// player stats
		unsigned int playerHealth = m_player->m_health;
		sprintf_s(buffer, 256, "HP: %u", playerHealth);
		m_spriteFont->draw(m_hudSpriteBatch, buffer, glm::vec2(10.0f, m_screenHeight - 50.0f), glm::vec2(1.0f), 0.0f, Pixels2D::ColorRGBA8(255, 255, 255, 255));

		// humans/aliens stats gui
		sprintf_s(buffer, "Humans: %d", m_humans.size());
		m_spriteFont->draw(m_hudSpriteBatch, buffer, glm::vec2(10.0f, 5.0f), glm::vec2(1.0f), 0.0f, Pixels2D::ColorRGBA8(255, 255, 255, 255));

		sprintf_s(buffer, 256, "Aliens: %d", m_aliens.size());
		m_spriteFont->draw(m_hudSpriteBatch, buffer, glm::vec2(10.0f, 35.0f), glm::vec2(1.0f), 0.0f, Pixels2D::ColorRGBA8(255, 255, 255, 255));

		// weapons gui
		sprintf_s(buffer, "1. Revolver");
		m_spriteFont->draw(m_hudSpriteBatch, buffer, glm::vec2(m_screenWidth - 150.0f, m_screenHeight - 35.0f),
			glm::vec2(0.66f), 0.0f, Pixels2D::ColorRGBA8(255, 255, 255, 255));

		sprintf_s(buffer, 256, "2. MP5  SMG");
		m_spriteFont->draw(m_hudSpriteBatch, buffer, glm::vec2(m_screenWidth - 150.0f, m_screenHeight - 60.0f),
			glm::vec2(0.66f), 0.0f, Pixels2D::ColorRGBA8(255, 255, 255, 255));

		if (m_player->wasPickedUpShotgun)
		{
			sprintf_s(buffer, 256, "3. Shotgun");
			m_spriteFont->draw(m_hudSpriteBatch, buffer, glm::vec2(m_screenWidth - 150.0f, m_screenHeight - 85.0f),
				glm::vec2(0.66f), 0.0f, Pixels2D::ColorRGBA8(255, 255, 255, 255));
		}

		// pick-ups gui
		if (m_player->wasPickedUpShotgun)
		{
			static unsigned int pickUpTimer = 0;
			pickUpTimer++;
			if(pickUpTimer < 360)
			{
				sprintf_s(buffer, 256, "Picked up Shotgun!");
				m_spriteFont->draw(m_hudSpriteBatch, buffer, glm::vec2(m_screenWidth / 2.0f - 150.0f, m_screenHeight - 50.0f),
					glm::vec2(1.0f), 0.0f, Pixels2D::ColorRGBA8(255, 255, 255, 255));
			}
		}
	}

	m_hudSpriteBatch.end();
	m_hudSpriteBatch.renderBatch();
}

void MainGame::addBlood(const glm::vec2 &position, const int &numParticles)
{
	static std::mt19937 randEngine(time(nullptr));
	static std::uniform_real_distribution<float> randAngle(0.0f, 360.0f);

	glm::vec2 vel(2.0f, 0.0f);
	Pixels2D::ColorRGBA8 col(255, 0, 0, 255);

	for (int i = 0; i < numParticles; i++) {
		m_bloodParticleBatch->addParticle(position, glm::rotate(vel, glm::radians(randAngle(randEngine))), col, 30.0f);
	}
}

void MainGame::Quit(char *buffer)
{
	m_window.swapBuffer();

	static unsigned int quitTimer = 0;
	quitTimer++;
	static unsigned int counter = 8;

	if (quitTimer == 60)
	{
		counter--;
		std::cout << "Quitting in " << counter << std::endl;
		sprintf_s(buffer, 256, "Quitting in %d", counter);
		m_spriteFont->draw(m_hudSpriteBatch, buffer, glm::vec2(m_screenWidth / 2.0f - 230.0f, m_screenHeight / 2.0f),
			glm::vec2(2.0f), 0.0f, Pixels2D::ColorRGBA8(255, 255, 255, 255));
		m_window.swapBuffer();
		if (counter <= 0)
		{
			SDL_Quit();
			exit(69);
		}
		quitTimer = 0;
	}
}