/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "shader.h"
#include "texture.h"



// Represents a single particle and its state
struct Particle {
	glm::vec3 EmitPos;
	glm::vec3 Position, Velocity;
	glm::vec4 Color;
	GLfloat Life;

	Particle(glm::vec3 pos) :EmitPos(pos), Position(pos), Velocity(glm::vec3(0,0.1f,0)), Color(1.0f), Life(5.0f) { }
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
	void Update(GLfloat dt, GLuint newParticles);
	// Render all particles
	void Draw(glm::mat4 m, glm::mat4 v, glm::mat4 p);
private:
	float timer = 0;
	// State
	std::vector<Particle> particles;
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