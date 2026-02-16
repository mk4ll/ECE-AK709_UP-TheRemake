#ifndef USERNAV_H
#define USERNAV_H

#include <glm/glm.hpp>
#include <vector>

// Forward declarations
class House;
class Camera;
struct GLFWwindow;

class UserNav {
public:
	UserNav();
	~UserNav();

	// Split update into two phases to allow Physics Update in between
	void handleInput(House* house, Camera* camera, float dt, GLFWwindow* window);
	void updateCamera(House* house, Camera* camera, float dt);

private:
	float m_distBehind;
	float m_distAbove;
	float m_controlStrength;
};

#endif
