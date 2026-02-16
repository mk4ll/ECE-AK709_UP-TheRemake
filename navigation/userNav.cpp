#include "userNav.h"
#include <GL/glew.h> // Must be included before glfw3.h
#include <common/camera.h>
#include <glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <house/house.h>
#include <iostream>

using namespace glm;

UserNav::UserNav()
    : m_distBehind(30.0f), m_distAbove(10.0f), m_controlStrength(30.0f) {
}

UserNav::~UserNav() {}

void UserNav::handleInput(House* house, Camera* camera, float dt,
    GLFWwindow* window) {
    // 1. User Controls House (WASD moves House relative to Camera View)

    // Need to get camera direction for controls, even if we don't move camera yet
    vec3 camDir(cos(camera->verticalAngle) * sin(camera->horizontalAngle),
        sin(camera->verticalAngle),
        cos(camera->verticalAngle) * cos(camera->horizontalAngle));

    vec3 userForce(0.0f);

    // Get Forward/Right vectors relative to camera (projected on XZ plane for
    // horizontal movement)
    vec3 forward = glm::normalize(vec3(camDir.x, 0, camDir.z));
    vec3 right = glm::normalize(glm::cross(forward, vec3(0, 1, 0)));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        userForce += forward * m_controlStrength;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        userForce -= forward * m_controlStrength;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        userForce += right * m_controlStrength;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        userForce -= right * m_controlStrength;

    // Altitude control
    // Apply force only if moving
    if (length(userForce) > 0.001f) {
        // Task: Less balloons = Less control
        // Scale force by ratio of current balloons to threshold (e.g. 8)
        float balloonFactor = (float)house->getAttachedBalloonCount() /
            (float)House::BALLOON_THRESHOLD;

        // Clamp min to 0.1 (so you aren't totally helpless with 1 balloon) and max
        // to 1.5 (diminishing returns)
        balloonFactor = glm::clamp(balloonFactor, 0.1f, 1.5f);

        house->applyExternalForce(userForce * balloonFactor);
    }
}

void UserNav::updateCamera(House* house, Camera* camera, float dt) {
    // 2. Update Camera (3rd Person View)

    // Hack: We want to use the Camera class to handle "Mouse Look" (updating
    // horizontal/vertical Angle) BUT we don't want the WASD keys to move the
    // camera position (as they should move the House). So, we temporarily set
    // camera speed to 0, call update(), then restore it.

    float oldSpeed = camera->speed;
    camera->speed = 0.0f; // Disable camera movement keys
    camera->update();     // Process Mouse Input -> Angles
    camera->speed =
        oldSpeed; // Restore speed for when we switch back to Autopilot

    // Calculate Camera Position relative to House
    vec3 housePos = house->getPosition();

    // Get camera viewing direction from its internal angles
    vec3 camDir(cos(camera->verticalAngle) * sin(camera->horizontalAngle),
        sin(camera->verticalAngle),
        cos(camera->verticalAngle) * cos(camera->horizontalAngle));

    // Position camera behind house
    // CameraPos = HousePos - (ViewDir * distBehind) + distAbove
    camera->position =
        housePos - (camDir * m_distBehind) + vec3(0, m_distAbove, 0);

    // Re-calculate view matrix manually since we changed position and want to
    // look AT the house (offset)
    camera->viewMatrix = glm::lookAt(
        camera->position,         // Eye
        housePos + vec3(0, 5, 0), // Center (Look at slightly above house center)
        glm::vec3(0, 1, 0)        // Up
    );
}
