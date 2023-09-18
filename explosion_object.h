#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "game_object.h"
#include "texture.h"
#include <vector>

class ExplosionObject : public GameObject {
public:
	glm::vec2 Position;
	std::vector<Texture2D> AnimationTextures;
	int CurrentFrame;
	float FrameDuration;
	float FrameTimer;

	ExplosionObject(glm::vec2 position, std::vector<Texture2D> animationTextures, float frameDuration);

	void Update(float dt);
	bool IsFinished();
	void Draw(SpriteRenderer& renderer);
};