#ifndef MAINGAME_HGUARD
#define MAINGAME_HGUARD

#include <Engine/Pixels2D/Window.h>
#include <Engine/Pixels2D/GLSLProgram.h>
#include <Engine/Pixels2D/Camera2D.h>
#include <Engine/Pixels2D/InputManager.h>
#include <Engine/Pixels2D/ResourceManager.h>
#include <Engine/Pixels2D/SpriteBatch.h>

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

private:
	// the game window
	Pixels2D::Window _window;

	// the shader program
	Pixels2D::GLSLProgram _textureProgram;

	// handles input
	Pixels2D::InputManager _inputManager;

	// main camera
	Pixels2D::Camera2D _camera;

	// draws all agents
	Pixels2D::SpriteBatch _agentSpriteBatch;

	// a vector of all levels
	std::vector<Level*> _levels;

	// a vector of all humans
	std::vector<Human*> _humans;
	
	// a vector of all aliens
	std::vector<Alien*> _aliens;

	std::vector<Bullet> _bullets;

	Player *_player;

	// window dimensions
	unsigned int _screenWidth, _screenHeight;

	// Game state
	GameState _gameState;

	float _fps;
	const float _maxFPS;

	unsigned int _currentLevel;

	// humans killed by player
	unsigned int _numHumansKilled;
	
	// aliens killed by player
	unsigned int _numAliensKilled;
};

#endif // !MAINGAME_HGUARD