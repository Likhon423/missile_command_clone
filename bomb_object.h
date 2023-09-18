#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "game_object.h"
#include "texture.h"

class BombObject : public GameObject
{
public:
	float Radius;
	float Direction;

	BombObject();
	BombObject(glm::vec2 pos, float radius, float direction, glm::vec2 velocity, Texture2D sprite);

	// moves the bomb
	glm::vec2 Move(float dt, unsigned int window_width);

	// Reset ball to original state
	void Reset(glm::vec2 position, glm::vec2 velocity);
};