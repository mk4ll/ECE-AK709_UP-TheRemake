// Include C++ headers
#include <iostream>
#include <string>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Shader loading utilities and other
#include <common/shader.h>
#include <common/util.h>
#include <common/camera.h>
#include <common/model.h>
#include <common/texture.h>
#include <common/light.h> 

#include <terrain/terrain.h>
#include <terrain/river.h>
#include <balloons/balloonMesh.h>
#include <balloons/rope.h>
#include <balloons/balloon.h>
#include <balloons/balloonTypes.h>
#include <balloons/ropeInstance.h>

#include <physics/rigidBody.h>
#include <physics/collision.h>
#include <particles/particleSystem.h>
#include <house/house.h>

using namespace std;
using namespace glm;

// Function prototypes
void initialize();
void createContext();
void mainLoop();
void free();

#define W_WIDTH 1024
#define W_HEIGHT 768
#define TITLE "UP - The Remake"

#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024



// Creating a structure to store the material parameters of an object
struct Material {
	vec4 Ka;
	vec4 Kd;
	vec4 Ks;
	float Ns;
};


// Global Variables
GLFWwindow* window;
Camera* camera;
Light* light;
GLuint shaderProgram, depthProgram, miniMapProgram;
GLuint depthFBO, depthTexture;
Drawable* quad;
//
// task 1
Drawable* house;
GLuint houseDiffuseTexture, houseSpecularTexture;
House* housePhysics;
Drawable* mountainTerrain;
Drawable* river;
GLuint waterDiffuseTexture, waterSpecularTexture;
GLuint waterDuDvTexture;
//

//
// task2: balloons
Drawable* balloon;
BalloonMesh balloonMesh;
Drawable* rope;
Drawable* bananaModel;

vector<Balloon*> balloons;
vector<RopeInstance*> ropeInstances;
const int NUM_BALLOONS = 15;					// AMOUNT OF BALLOONS

ParticleSystem* popParticles = nullptr;

//

// locations for shaderProgram
GLuint viewMatrixLocation;
GLuint projectionMatrixLocation;
GLuint modelMatrixLocation;
GLuint KaLocation, KdLocation, KsLocation, NsLocation;
GLuint LaLocation, LdLocation, LsLocation;
GLuint lightPositionLocation;
GLuint lightPowerLocation;
GLuint diffuseColorSampler;
GLuint specularColorSampler;
GLuint useTextureLocation;
GLuint depthMapSampler;
GLuint lightVPLocation;

GLuint timeLocation;

GLuint dudvSampler;



// locations for depthProgram
GLuint shadowViewProjectionLocation;
GLuint shadowModelLocation;

// locations for miniMapProgram
GLuint quadTextureSamplerLocation;

// Terrain material
const Material martianTerrain{
	vec4{0.15f, 0.05f, 0.02f, 1.0f},   // Ka: Dark rust/brown ambient
	vec4{0.65f, 0.25f, 0.12f, 1.0f},   // Kd: Primary rusty orange diffuse
	vec4{0.10f, 0.08f, 0.05f, 1.0f},   // Ks: Very weak, warm specular (dusty)
	5.0f                               // Ns: Low shininess for a matte surface
};

const Material ropeMaterial{
	glm::vec4(0.15f, 0.12f, 0.08f, 1.0f),  // Ka - brown ambient
	glm::vec4(0.45f, 0.38f, 0.25f, 1.0f),  // Kd - natural diffuse
	glm::vec4(0.05f, 0.05f, 0.05f, 1.0f),  // Ks - ~0
	4.0f                                   // Ns - low shininess
};

// transparent balloon inner Obj: banana
const Material bananaSkinMaterial{
	vec4(0.2f, 0.2f, 0.2f, 1.0f),      // Ka - Standard low glow
	vec4(0.58f, 0.54f, 0.02f, 1.0f),   // Kd - YELLOW color from file
	vec4(0.1f, 0.1f, 0.1f, 1.0f),      // Ks - Low shine
	10.0f                              // Ns
};


// NOTE: Since the Light and Material struct are used in the shader programs as well 
//		 it is recommended to create a function that will update all the parameters 
//       of an object.
// 
// Creating a function to upload (make uniform) the light parameters to the shader program
void uploadLight(const Light& light) {
	glUniform4f(LaLocation, light.La.r, light.La.g, light.La.b, light.La.a);
	glUniform4f(LdLocation, light.Ld.r, light.Ld.g, light.Ld.b, light.Ld.a);
	glUniform4f(LsLocation, light.Ls.r, light.Ls.g, light.Ls.b, light.Ls.a);
	glUniform3f(lightPositionLocation, light.lightPosition_worldspace.x,
		light.lightPosition_worldspace.y, light.lightPosition_worldspace.z);
}


// Creating a function to upload the material parameters of a model to the shader program
void uploadMaterial(const Material& mtl) {
	glUniform4f(KaLocation, mtl.Ka.r, mtl.Ka.g, mtl.Ka.b, mtl.Ka.a);
	glUniform4f(KdLocation, mtl.Kd.r, mtl.Kd.g, mtl.Kd.b, mtl.Kd.a);
	glUniform4f(KsLocation, mtl.Ks.r, mtl.Ks.g, mtl.Ks.b, mtl.Ks.a);
	glUniform1f(NsLocation, mtl.Ns);
}




void createContext() {
	// Create and compile our GLSL program from the shader
	shaderProgram = loadShaders("../shaders/ShadowMapping.vertexshader", "../shaders/ShadowMapping.fragmentshader");

	// Task 3.1 
	// Create and load the shader program for the depth buffer construction
	// You need to load and use the Depth.vertexshader, Depth.fragmentshader
	depthProgram = loadShaders("../shaders/Depth.vertexshader", "../shaders/Depth.fragmentshader");


	// Task 2.1
	// Use the MiniMap.vertexshader, "MiniMap.fragmentshader"
	miniMapProgram = loadShaders("../shaders/MiniMap.vertexshader", "../shaders/MiniMap.fragmentshader");


	// NOTE: Don't forget to delete the shader programs on the free() function


	// Get pointers to uniforms
	// --- shaderProgram ---
	projectionMatrixLocation = glGetUniformLocation(shaderProgram, "P");
	viewMatrixLocation = glGetUniformLocation(shaderProgram, "V");
	modelMatrixLocation = glGetUniformLocation(shaderProgram, "M");
	// for phong lighting
	KaLocation = glGetUniformLocation(shaderProgram, "mtl.Ka");
	KdLocation = glGetUniformLocation(shaderProgram, "mtl.Kd");
	KsLocation = glGetUniformLocation(shaderProgram, "mtl.Ks");
	NsLocation = glGetUniformLocation(shaderProgram, "mtl.Ns");
	LaLocation = glGetUniformLocation(shaderProgram, "light.La");
	LdLocation = glGetUniformLocation(shaderProgram, "light.Ld");
	LsLocation = glGetUniformLocation(shaderProgram, "light.Ls");
	lightPositionLocation = glGetUniformLocation(shaderProgram, "light.lightPosition_worldspace");
	diffuseColorSampler = glGetUniformLocation(shaderProgram, "diffuseColorSampler");
	specularColorSampler = glGetUniformLocation(shaderProgram, "specularColorSampler");

	dudvSampler = glGetUniformLocation(shaderProgram, "dudvSampler");
	//
	// for time in shader
	timeLocation = glGetUniformLocation(shaderProgram, "time");
	//

	// Task 1.4
	useTextureLocation = glGetUniformLocation(shaderProgram, "useTexture");

	// locations for shadow rendering
	depthMapSampler = glGetUniformLocation(shaderProgram, "shadowMapSampler");
	lightVPLocation = glGetUniformLocation(shaderProgram, "lightVP");


	// --- depthProgram ---
	shadowViewProjectionLocation = glGetUniformLocation(depthProgram, "VP");
	shadowModelLocation = glGetUniformLocation(depthProgram, "M");

	// --- miniMapProgram ---
	quadTextureSamplerLocation = glGetUniformLocation(miniMapProgram, "textureSampler");

	// Loading a model
	// loading a diffuse and a specular texture
	houseDiffuseTexture = loadSOIL("../assets/textures/house_diffuse.bmp");
	houseSpecularTexture = loadSOIL("../assets/textures/house_specular.bmp");

	waterDiffuseTexture = loadSOIL("../terrain/water_diffuse.bmp");
	waterSpecularTexture = loadSOIL("../terrain/water_specular.bmp");

	waterDuDvTexture = loadSOIL("../terrain/water_dudv.png");

	// house 
	house = new Drawable("../assets/models/houseUP.obj");

	vec3 peak = Terrain::get_terrain_peak();
	housePhysics = new House(house, peak);

	// terrain
	float size = 100.0f;
	float res = 200.0f;
	float maxHeight = 15.0f;

	mountainTerrain = Terrain::generate(size, res, maxHeight);

	float waterLevel = -2.0f;
	river = River::createFloodedCanyon(size, res, waterLevel, maxHeight);

	// banana obj for transparent balloon
	bananaModel = new Drawable(std::string("../assets/models/banana.obj"));

	// balloons
	/*/
	// ONE BALLOON:
	ropeInstance = new RopeInstance(Rope::DEFAULT_LENGTH);

	vec3 peak = Terrain::get_terrain_peak();
	vec3 chimneyOffset = vec3(-0.18f, 5.0f, -2.0f);
	vec3 chimneyPos = peak + chimneyOffset;

	// balloon
	balloon = new Drawable(balloonMesh.positions, balloonMesh.uvs);
	balloonObj = new Balloon(balloon);

	// anchor = chimney
	balloonObj->setAnchor(chimneyPos);
	balloonObj->attach(Rope::DEFAULT_LENGTH);
	/*/

	//
	// multiple balloons
	vec3 chimneyOffset = vec3(-0.18f, 5.0f, -2.0f);
	vec3 chimneyPos = peak + chimneyOffset;

	balloon = new Drawable(balloonMesh.positions, balloonMesh.uvs);

	// multiple balloons around the center of the chimney
	for (int i = 0; i < NUM_BALLOONS; ++i) {
		BalloonType type = getBalloonTypeByIndex(i);
		// If transparent, add banana inside, else draw other type of balloon
		Balloon* newBalloon = (type == BalloonType::TRANSPARENT)
			? new Balloon(balloon, type, bananaModel)
			: new Balloon(balloon, type);

		// put them in a circle
		float angle = (float)i / NUM_BALLOONS * 2.0f * 3.14159f;
		float radius = 0.01f; // radius around chimneyPos
		vec3 offset = vec3(cos(angle) * radius, 0.0f, sin(angle) * radius);

		newBalloon->setAnchor(chimneyPos + offset);
		newBalloon->attach(Rope::DEFAULT_LENGTH);

		balloons.push_back(newBalloon);

		// create corresponding rope 
		RopeInstance* newRope = new RopeInstance(Rope::DEFAULT_LENGTH);
		ropeInstances.push_back(newRope);
		// debug info
		printf("Created balloon %d: %s\n", i, getBalloonTypeName(type));
	}

	// ---------------------------------------------------------------------------- //
	// -  Task 3.2 Create a depth framebuffer and a texture to store the depthmap - //
	// ---------------------------------------------------------------------------- //
	//*/
	// Tell opengl to generate a framebuffer
	glGenFramebuffers(1, &depthFBO);
	// Binding the framebuffer, all changes bellow will affect the binded framebuffer
	// **Don't forget to bind the default framebuffer at the end of initialization
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);


	// We need a texture to store the depth image
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	// Telling opengl the required information about the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0,
		GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);							// Task 4.5 Texture wrapping methods
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);							// GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
	//*/

	// Task 4.5 Don't shadow area out of light's viewport
	//*/
	// Step 1 : (Don't forget to comment out the respective lines above
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// Set color to set out of border 
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// Next go to fragment shader and add an if statement, so if the distance in the z-buffer is equal to 1, 
	// meaning that the fragment is out of the texture border (or further than the far clip plane) 
	// then the shadow value is 0.
	//*/

	//*/ Task 3.2 Continue
	// Attaching the texture to the framebuffer, so that it will monitor the depth component
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);


	// Since the depth buffer is only for the generation of the depth texture, 
	// there is no need to have a color output
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);


	// Finally, we have to always check that our frame buffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		glfwTerminate();
		throw runtime_error("Frame buffer not initialized correctly");
	}

	// Binding the default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//*/

}


void free() {
	// Delete balloons
	for (auto* b : balloons) {
		delete b;
	}
	balloons.clear();
	// Delete ropes
	for (auto* r : ropeInstances) {
		delete r;
	}
	ropeInstances.clear();
	// del house physics
	if (housePhysics) {
		delete housePhysics;
		housePhysics = nullptr;
	}

	// Delete Shader Programs
	glDeleteProgram(shaderProgram);
	glDeleteProgram(depthProgram);
	glDeleteProgram(miniMapProgram);

	glfwTerminate();
}


void depth_pass(mat4 viewMatrix, mat4 projectionMatrix) {

	// Setting viewport to shadow map size
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

	// Binding the depth framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);

	// Cleaning the framebuffer depth information (stored from the last render)
	glClear(GL_DEPTH_BUFFER_BIT);

	// Selecting the new shader program that will output the depth component
	glUseProgram(depthProgram);

	// sending the view and projection matrix to the shader
	mat4 view_projection = projectionMatrix * viewMatrix;
	glUniformMatrix4fv(shadowViewProjectionLocation, 1, GL_FALSE, &view_projection[0][0]);


	// ---- rendering the scene ---- //
	// creating model matrix and sending to GPU
	// terrain
	mat4 terrainModelMatrix = mat4(1.0f);
	glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &terrainModelMatrix[0][0]);
	mountainTerrain->bind();
	mountainTerrain->draw();

	// river
	mat4 riverModelMatrix = mat4(1.0f);
	glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &riverModelMatrix[0][0]);
	river->bind();
	river->draw();

	// house
	mat4 houseModelMatrix = mat4(1.0f);
	houseModelMatrix = translate(houseModelMatrix, housePhysics->getPosition());
	glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &houseModelMatrix[0][0]);
	house->bind();
	house->draw();

	// balloons
	for (size_t i = 0; i < balloons.size(); ++i) {
		if (balloons[i]->isPopped()) continue;
		mat4 balloonM = translate(mat4(1.0f), balloons[i]->getPosition());
		balloonM = scale(balloonM, vec3(balloons[i]->getRadius()));
		glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &balloonM[0][0]);
		balloon->bind();
		balloon->draw();
	}

	// binding the default framebuffer again
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//*/
}

void lighting_pass(mat4 viewMatrix, mat4 projectionMatrix) {
	// Step 1: Binding a frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, W_WIDTH, W_HEIGHT);

	// Step 2: Clearing color and depth info
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Step 3: Selecting shader program
	glUseProgram(shaderProgram);
	float t = glfwGetTime();
	glUniform1f(timeLocation, t);

	// Making view and projection matrices uniform to the shader program
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

	// uploading the light parameters to the shader program
	uploadLight(*light);

	// Task 4.1 Display shadows on the terrain
	// Sending the shadow texture to the shaderProgram
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glUniform1i(depthMapSampler, 2);

	// Sending the light View-Projection matrix to the shader program
	mat4 lightVP = light->lightVP();
	glUniformMatrix4fv(lightVPLocation, 1, GL_FALSE, &lightVP[0][0]);



	// ----------------------------------------------------------------- //
	// --------------------- Drawing scene objects --------------------- //	
	// ----------------------------------------------------------------- //

	// creating a model matrix
	mat4 modelMatrix = translate(mat4(), vec3(0.0, 0.0, -5.0));
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);

	// Setting up texture to display on shader program          //  --- Texture Pipeline ---
	glActiveTexture(GL_TEXTURE0);								// Activate texture position
	glBindTexture(GL_TEXTURE_2D, houseDiffuseTexture);			// Assign texture to position 
	glUniform1i(diffuseColorSampler, 0);						// Assign sampler to that position

	glActiveTexture(GL_TEXTURE1);								//
	glBindTexture(GL_TEXTURE_2D, houseSpecularTexture);			// Same process for specular texture
	glUniform1i(specularColorSampler, 1);						//


	// upload the material
	uploadMaterial(martianTerrain);
	glUniform1i(useTextureLocation, 0);

	//draw terrain under house
	mat4 terrainModelMatrix = mat4(1.0f);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &terrainModelMatrix[0][0]);

	mountainTerrain->bind();
	mountainTerrain->draw();

	// draw river
	glDisable(GL_CULL_FACE);

	// DuDv map
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, waterDuDvTexture);
	glUniform1i(dudvSampler, 3);

	// diffuse
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterDiffuseTexture);
	glUniform1i(diffuseColorSampler, 0);

	// specular 
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, waterSpecularTexture);
	glUniform1i(specularColorSampler, 1);


	glUniform1i(useTextureLocation, 2);

	river->bind();
	river->draw();

	glEnable(GL_CULL_FACE);

	// house
	// get terrain peak

	// Activate and bind diffuse texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, houseDiffuseTexture);
	glUniform1i(diffuseColorSampler, 0);

	// Activate and bind specular texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, houseSpecularTexture);
	glUniform1i(specularColorSampler, 1);

	glUniform1i(useTextureLocation, 1);

	housePhysics->draw(modelMatrixLocation);

	// multiple balloons
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &mat4(1.0f)[0][0]);
	uploadMaterial(ropeMaterial);
	glUniform1i(useTextureLocation, 0);

	// draw all ropes
	for (size_t i = 0; i < balloons.size(); ++i) {
		if (balloons[i]->isPopped() && balloons[i]->getVerletRope()) {
			balloons[i]->getVerletRope()->draw(modelMatrixLocation, rope);
		}
		else {
			ropeInstances[i]->draw(modelMatrixLocation);
		}
	}

	// draw all balloons
	for (size_t i = 0; i < balloons.size(); ++i) {
		BalloonType type = balloons[i]->getType();

		Material balloonMat = getBalloonMaterial(type);
		uploadMaterial(balloonMat);

		//shader flag
		int shaderFlag = getBalloonShaderFlag(type);
		glUniform1i(useTextureLocation, shaderFlag);

		// special rendering for transparent balloons
		if (type == BalloonType::TRANSPARENT) {	
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthMask(GL_FALSE);
		}

		balloons[i]->draw(modelMatrixLocation);

		// reset transparency settings
		if (type == BalloonType::TRANSPARENT) {
			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);
		}

		// draw inner obj
		if (type == BalloonType::TRANSPARENT) {
			uploadMaterial(bananaSkinMaterial);
			glUniform1i(useTextureLocation, 0);
			balloons[i]->drawContent(modelMatrixLocation);
		}
	}

	// reset for particles
	glUniform1i(useTextureLocation, 3);

	// draw particles
	if (popParticles) {
		popParticles->draw(modelMatrixLocation, balloon);
	}
}

// Task 2.3: visualize the depth_map on a sub-window at the top of the screen
void renderMiniMap() {
	// using the correct shaders to visualize the depth texture on the quad
	glUseProgram(miniMapProgram);

	//enabling the texture - follow the aforementioned pipeline
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glUniform1i(quadTextureSamplerLocation, 0);

	// Drawing the quad
	quad->bind();
	quad->draw();
}


// collision detection: BALLOONS
void handleBalloonCollisions() {
	for (size_t i = 0; i < balloons.size(); ++i) {
		if (balloons[i]->isPopped()) continue;

		for (size_t j = i + 1; j < balloons.size(); ++j) {
			if (balloons[j]->isPopped()) continue;

			//sphere structs
			Sphere s1, s2;
			s1.x = balloons[i]->getPosition();
			s1.r = balloons[i]->getRadius();

			s2.x = balloons[j]->getPosition();
			s2.r = balloons[j]->getRadius();

			// check for collision
			if (checkSphereSphereCollision(s1, s2)) {
				RigidBody& rb1 = balloons[i]->getRigidBody();
				RigidBody& rb2 = balloons[j]->getRigidBody();

				// collsiion handling
				handleSphereSphereCollision(
					s1, s2,
					rb1.velocity, rb2.velocity,
					rb1.mass, rb2.mass
				);

				// update positions
				rb1.position = s1.x;
				rb2.position = s2.x;
			}
		}
	}
}


void mainLoop() {
	static float lastTime = 0.0f;

	light->update();
	mat4 light_proj = light->projectionMatrix;
	mat4 light_view = light->viewMatrix;

	// Task 3.3
	// Create the depth buffer
	depth_pass(light_view, light_proj);

	do {

		light->update();
		mat4 light_proj = light->projectionMatrix;
		mat4 light_view = light->viewMatrix;

		// Task 3.5
		// Create the depth buffer
		depth_pass(light_view, light_proj);

		// Getting camera information
		camera->update();
		mat4 projectionMatrix = camera->projectionMatrix;
		mat4 viewMatrix = camera->viewMatrix;
		// Balloon Functions
		float dt = glfwGetTime() - lastTime;
		lastTime = glfwGetTime();
		// static vars to store key-pressed values
		static bool keyN_wasPressed = false;
		static bool keyM_wasPressed = false;

		if (!balloons.empty()) {
			// release
			bool keyN_isPressed = (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS);
			if (keyN_isPressed && !keyN_wasPressed) {
				// first available balloon to be released
				for (size_t i = 0; i < balloons.size(); ++i) {
					if (balloons[i]->isRopeAttached() && !balloons[i]->isPopped()) {
						balloons[i]->release();
						printf("Balloon %zu was RELEASED!\n", i);
						break;
					}
				}
			}
			keyN_wasPressed = keyN_isPressed; // save state

			// pop
			bool keyM_isPressed = (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS);
			if (keyM_isPressed && !keyM_wasPressed) {
				// first available balloon to be popped
				for (size_t i = 0; i < balloons.size(); ++i) {
					if (!balloons[i]->isPopped() && balloons[i]->isRopeAttached()) {
						balloons[i]->pop();
						if (popParticles) {
							delete popParticles;
						}
						popParticles = new ParticleSystem(balloons[i]->getPosition(), balloons[i]->getColor());
						printf("Balloon %zu POPPED!\n", i);
						break;
					}
				}
			}
			keyM_wasPressed = keyM_isPressed; // save state
			
			if (popParticles) {
				popParticles->update(dt);
				if (!popParticles->isAlive()) {
					delete popParticles;
					popParticles = nullptr;
				}
			}
		}
		// update all balloons
		vec3 peak = Terrain::get_terrain_peak();

		for (size_t i = 0; i < balloons.size(); ++i) {
			balloons[i]->applyForces();
			balloons[i]->update(dt);

			vec3 houseMin = peak + vec3(-10.0f, 0.0f, -10.0f);
			vec3 houseMax = peak + vec3(10.0f, 10.0f, 10.0f);
			balloons[i]->setHouseBounds(houseMin, houseMax);
		}

		handleBalloonCollisions();

		housePhysics->applyForces(balloons);
		housePhysics->update(dt);
		//
		//debugging
		printf("House Y: %.2f, Balloons: %d, Flying: %d\n",
			housePhysics->getPosition().y,
			housePhysics->getAttachedBalloonCount(),
			housePhysics->isFlying());
		//

		// update ropes
		for (size_t i = 0; i < balloons.size(); ++i) {
			if (!balloons[i]->isPopped()) {
				float angle = (float)i / NUM_BALLOONS * 2.0f * 3.14159f;
				float radius = 0.1f;
				vec3 offset = vec3(cos(angle) * radius, 0.0f, sin(angle) * radius);
				vec3 chimneyOffset = vec3(-0.18f, 5.0f, -2.0f);
				vec3 anchorPos = housePhysics->getPosition() + chimneyOffset + offset;

				if (balloons[i]->isRopeAttached()) {
					balloons[i]->updateAnchor(anchorPos);
				}

				ropeInstances[i]->updateBezier(anchorPos,
					balloons[i]->getPosition(),
					false, dt);
			}

			ropeInstances[i]->update(balloons[i]->getRopeStart(),
				balloons[i]->getPosition());
		}

		//

		// Task 1.5
		// Rendering the scene from light's perspective when F1 is pressed
		//*/
		if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
			lighting_pass(light_view, light_proj);
		}
		else {
			// Render the scene from camera's perspective
			lighting_pass(viewMatrix, projectionMatrix);
		}
		//*/



		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

}


void initialize() {
	// Initialize GLFW
	if (!glfwInit()) {
		throw runtime_error("Failed to initialize GLFW\n");
	}


	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// Open a window and create its OpenGL context
	window = glfwCreateWindow(W_WIDTH, W_HEIGHT, TITLE, NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		throw runtime_error(string(string("Failed to open GLFW window.") +
			" If you have an Intel GPU, they are not 3.3 compatible." +
			"Try the 2.1 version.\n"));
	}
	glfwMakeContextCurrent(window);

	// Start GLEW extension handler
	glewExperimental = GL_TRUE;

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		throw runtime_error("Failed to initialize GLEW\n");
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Hide the mouse and enable unlimited movement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, W_WIDTH / 2, W_HEIGHT / 2);

	// Sky blue background color
	glClearColor(0.53f, 0.81f, 0.92f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// enable texturing and bind the depth texture
	glEnable(GL_TEXTURE_2D);

	// enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// Log
	logGLParameters();

	//initalize terrain
	mountainTerrain = Terrain::generate(100.0f, 50, 15.0f);
	// get terrain peak
	vec3 peak = Terrain::get_terrain_peak();

	// Create camera
	camera = new Camera(window);

	//initialize the camera position so that the scene is visible
	camera->position = vec3(peak.x, peak.y + 5.0f, 20.0f);

	// Task 1.1 Creating a light source
	// Creating a custom light

	light = new Light(window,
		vec4{ 1, 1, 1, 1 },
		vec4{ 1, 1, 1, 1 },
		vec4{ 1, 1, 1, 1 },
		vec3{0, peak.y + 30, peak.z + 25} // over the house and over the peak
	);

}

int main(void) {
	try {
		initialize();
		createContext();
		mainLoop();
		free();
	}
	catch (exception& ex) {
		cout << ex.what() << endl;
		getchar();
		free();
		return -1;
	}

	return 0;
}
