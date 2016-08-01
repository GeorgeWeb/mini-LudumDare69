#pragma once

#ifndef MAINGAME_HGUARD
#define MAINGAME_HGUARD

#include <Engine/Pixels2D/Window.h>
#include <Engine/Pixels2D/GLSLProgram.h>
#include <Engine/Pixels2D/Camera2D.h>
#include <Engine/Pixels2D/InputManager.h>
#include <Engine/Pixels2D/ResourceManager.h>
#include <Engine/Pixels2D/SpriteBatch.h>
#include <Engine/Pixels2D/SpriteFont.h>
#include <Engine/Pixels2D/AudioEngine.h>
#include <Engine/Pixels2D/ParticleEngine2D.h>
#include <Engine/Pixels2D/ParticleEngine2D.h>

#include "Level.h"
#include "Player.h"
#include "Alien.h"

enum class GameState
{
	PLAY,
	EXIT
};

class MainGame
{
public:
	MainGame();
	~MainGame();

	// runs the game
	void run();

private:
	// initializes the core systems
	void initSystems();

	// initializes the level
	void initLevel();

	// initializes the shaders
	void initShaders();

	// main game loop for the program
	void gameLoop();

	// update all agents (player, humans, aliens)
	void updateAgents(const float &deltaTime);

	// update all bullets
	void updateBullets(const float &deltaTime);

	// checks the win condition
	void checkVictory();

	// handles input processing
	void processInput();

	// renders the game
	void drawGame();

	// draw the HUD
	void drawHUD();

	// spawns blood particles on collision with bullets
	void addBlood(const glm::vec2 &position, const int &numParticles);

private:
	// the game window
	Pixels2D::Window m_window;

	// the shader program
	Pixels2D::GLSLProgram m_textureProgram;

	// handles input
	Pixels2D::InputManager m_inputManager;

	// main camera
	Pixels2D::Camera2D m_mainCamera;
	// HUD camera
	Pixels2D::Camera2D m_hudCamera;

	// draws all agents
	Pixels2D::SpriteBatch m_agentSpriteBatch;
	// draws UI
	Pixels2D::SpriteBatch m_hudSpriteBatch;
	
	Pixels2D::ParticleEngine2D m_particleEngine;
	Pixels2D::ParticleBatch2D* m_bloodParticleBatch;

	// loads the font
	Pixels2D::SpriteFont *m_spriteFont;

	// manages all of the game audio
	Pixels2D::AudioEngine m_audioEngine;

	// a vector of all levels
	std::vector<Level*> m_levels;

	// a vector of all humans
	std::vector<Human*> m_humans;
	
	// a vector of all aliens
	std::vector<Alien*> m_aliens;

	std::vector<Bullet> m_bullets;

	Player *m_player;

	// window dimensions
	unsigned int m_screenWidth, m_screenHeight;

	// Game state
	GameState m_gameState;

	float m_fps;
	const float m_maxFPS;

	unsigned int m_currentLevel;

	// humans killed by player
	unsigned int m_numHumansKilled;
	
	// aliens killed by player
	unsigned int m_numAliensKilled;
};

#endif // !MAINGAME_HGUARD