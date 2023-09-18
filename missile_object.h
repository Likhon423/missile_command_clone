#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "game_object.h"
#include "texture.h"

class MissileObject : public GameObject
{
public:
	bool ReachedDestination;
	float ExplosionTimer;
	float Speed = 800.0f;
	static constexpr float ExplosionDuration = 0.5f;
	glm::vec2 Destination;

	MissileObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec2 destination);

	bool Move(float dt);
};