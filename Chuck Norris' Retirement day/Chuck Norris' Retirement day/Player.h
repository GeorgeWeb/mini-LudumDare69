#ifndef PLAYER_HGUARD
#define PLAYER_HGUARD

#include <Engine/Pixels2D/InputManager.h>
#include <Engine/Pixels2D/Camera2D.h>

#include "Human.h"
#include "Bullet.h"

class Gun;

const float PLAYER_SPEED = 5.0f;
const float PLAYER_HEALTH = 200.0f;

class Player : public Human
{
public:
	Player();
	~Player();

	void init(const glm::vec2 &position, const float &speed, const float &health, Pixels2D::InputManager *inputManager, Pixels2D::Camera2D *camera, std::vector<Bullet> *bullets);

	void addGun(Gun *gun);

	void update(const std::vector<std::string> &levelData, std::vector<Human*> &humans, std::vector<Alien*> &aliens) override;

private:
	Pixels2D::InputManager *_inputManager;
	
	std::vector<Gun*> _guns;
	int _currentGunIndex;
	Pixels2D::Camera2D *_camera;
	std::vector<Bullet> *_bullets;
};

#endif // !PLAYER_HGUARD