#pragma once
#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "shader.h"
#include "Texture2D.h"

// Represents a single particle and its state
struct Particle {
	glm::vec3 Position, Velocity;
	glm::vec4 Color;
	GLfloat Life;

	Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};


// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class ParticleSystem
{
public:
	// Constructor
	ParticleSystem(Shader shader, Texture2D texture, GLuint amount, glm::vec3 pos);
	// Update all particles
	void Update(GLfloat dt, GLuint newParticles, glm::vec3 offset = glm::vec3(0.0f, 0.0f ,0.0f));
	// Render all particles
	void Draw();

private:
	// State
	std::vector<Particle> particles;
	GLfloat g = 9.8f;
	GLuint amount;
	glm::vec3 pos;
	// Render state
	Shader shader;
	Texture2D texture;
	GLuint VAO;
	// Initializes buffer and vertex attributes
	void init();
	// Returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
	GLuint firstUnusedParticle();
	// Respawns particle
	void respawnParticle(Particle &particle, glm::vec3 offset = glm::vec3(0.0f, 0.0f,0.0f));
};

#endif


