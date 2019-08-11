/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "ParticleSystem.h"
#include<iostream>
ParticleSystem::ParticleSystem(Shader shader, Texture2D texture, GLuint amount,glm::vec3 pos)
	: shader(shader), texture(texture), amount(amount), pos(pos)
{
	this->init();
}

   


void ParticleSystem::Update(GLfloat dt, GLuint newParticles)
{
	std::cout << particles[1].Life << std::endl;
	// Add new particles 
	/*for (GLuint i = 0; i < newParticles; ++i)
	{
		int unusedParticle = this->firstUnusedParticle();
		this->respawnParticle(this->particles[unusedParticle], offset);
	}*/
	// Update all particles
	//for (GLuint i = 0; i < particles.; ++i)
	//{
	//	Particle &p = this->particles[i];
	//	p.Life -= dt; // reduce life
	//	if (p.Life > 0.0f)
	//	{	// particle is alive, thus update
	//		p.Position -= p.Velocity * dt;
	//		p.Color.a -= dt * 2.5;
	//	}
	//	else {
	//		respawnParticle(this->particles[i], offset);
	//	}
	//}

	for (std::vector<Particle>::iterator iter = particles.begin(); iter != particles.end();)
	{
		iter->Life -= dt;
		if (iter->Life < 0)
		{
			iter = particles.erase(iter);
		}
		else
		{
			iter->Position -= iter->Velocity * dt;
			++iter;
		}
	}

	timer += dt;
	if (timer>0.8f) {
		timer = 0;
		float areaLength = 1.0f;
		float fallHeight = 0.2f;
		for (GLuint i = 0; i < this->amount; ++i) {
			this->particles.push_back(Particle(
				glm::vec3(
				(2.0f*float(rand()) / float(RAND_MAX) - 1.0f)*areaLength,
					fallHeight + float(rand())*0.05 / float(RAND_MAX),
					(2.0f*float(rand()) / float(RAND_MAX) - 1.0f)*areaLength
				)));
		}
	}
}

// Render all particles
void ParticleSystem::Draw(glm::mat4 m, glm::mat4 v, glm::mat4 p)
{
	// Use additive blending to give it a 'glow' effect
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	this->shader.Use();
	for (Particle particle : this->particles)
	{
		if (particle.Life > 0.0f)
		{
			this->shader.SetMatrix4("model", m);
			this->shader.SetMatrix4("view", v);
			this->shader.SetMatrix4("projection", p);
			this->shader.SetVector3f("offset",particle.Position);
			this->shader.SetVector4f("color", particle.Color);
			this->texture.Bind();
			//glScalef(0.1f, 0.1f, 0.1f);
			glBindVertexArray(this->VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}
	// Don't forget to reset to default blending mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleSystem::init()
{
	// Set up mesh and attribute properties
	GLuint VBO;
	GLfloat particle_quad[] = {
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f
	};
	float scale = 0.01f;
	for (int i = 0; i < 6; i++) {
		particle_quad[i * 5]*= scale;
		particle_quad[i * 5 + 1] *= scale;
		particle_quad[i * 5 + 2] *= scale;
	}
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(this->VAO);
	// Fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	// Set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	//glBindVertexArray(0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
	glBindVertexArray(0);
	float areaLength = 1.0f;
	float fallHeight = 0.2f;
	// Create this->amount default particle instances
	for (GLuint i = 0; i < this->amount; ++i) {
		this->particles.push_back(Particle(
			glm::vec3(
			(2.0f*float(rand()) / float(RAND_MAX) - 1.0f)*areaLength,
			fallHeight + float(rand())*0.05 / float(RAND_MAX),
			(2.0f*float(rand()) / float(RAND_MAX) - 1.0f)*areaLength
			)));
	}
	
	
		
}

// Stores the index of the last particle used (for quick access to next dead particle)
GLuint lastUsedParticle = 0;

GLuint ParticleSystem::firstUnusedParticle()
{
	// First search from last used particle, this will usually return almost instantly
	for (GLuint i = lastUsedParticle; i < this->amount; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// Otherwise, do a linear search
	for (GLuint i = 0; i < lastUsedParticle; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
	lastUsedParticle = 0;
	return 0;
}

void ParticleSystem::respawnParticle(Particle &particle, glm::vec3 offset)
{
	
	GLfloat random = ((rand() % 100) - 50) / 10.0f;
	GLfloat rColor = 0.5 + ((rand() % 100) / 100.0f);
	particle.Position = pos + random + offset;
	particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
	particle.Life = 1.0f;
	particle.Velocity = glm::vec3(0,0,0);
}