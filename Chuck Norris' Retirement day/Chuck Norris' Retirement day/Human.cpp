#include "Human.h"

#include <glm/gtx/rotate_vector.hpp>

#include <random>
#include <ctime>

Human::Human() : m_frames(0.0f)
{
}

Human::~Human()
{
}

void Human::init(const glm::vec2 &position, const float &speed, const float &health)
{
	// random number generator
	static std::mt19937 randomEngine(time(nullptr));
	static std::uniform_real_distribution<float> randDir(-1.0f, 1.0f);

	m_color = Pixels2D::ColorRGBA8(255, 255, 255, 255);
	// initializations
	m_position = position;
	m_direction = glm::vec2(randDir(randomEngine), randDir(randomEngine));
	m_speed = speed;
	m_health = health;
	// make sure so that direction is not 0
	if (m_direction.length() == 0) m_direction = glm::vec2(1.0f, 0.0f);

	// normalize it => get unit length
	m_direction = glm::normalize(m_direction);
}

void Human::update(const std::vector<std::string> &levelData, std::vector<Human*> &humans, std::vector<Alien*> &aliens, const float &deltaTime)
{
	static std::mt19937 randomEngine(time(nullptr));
	static std::uniform_real_distribution<float> randRotate(-60.0f, 60.0f);

	m_position += m_direction * m_speed * deltaTime;

	// humans change direction every few frames
	if (m_frames == 60.0f)
	{
		m_direction = glm::rotate(m_direction, glm::radians(randRotate(randomEngine)));
		m_frames = 0.0f;
	}
	else m_frames += 1.0f;

	if(collideWithLevel(levelData))
		m_direction = glm::rotate(m_direction, glm::radians(randRotate(randomEngine)));
}