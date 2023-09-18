#include "bomb_object.h"

BombObject::BombObject()
	: GameObject(), Radius(12.5f) { }

BombObject::BombObject(glm::vec2 pos, float radius, float direction, glm::vec2 velocity, Texture2D sprite)
	: GameObject(pos, glm::vec2(radius * 2.0f, radius * 2.0f), sprite, glm::vec3(1.0f), velocity), Radius(radius), Direction(direction) { }

glm::vec2 BombObject::Move(float dt, unsigned int window_width)
{
	//move the bomb
	this->Position.x += this->Velocity.x * dt * Direction;
	this->Position.y += this->Velocity.y * dt;

	// check if outside window bounds, reverse velocity and restore correct position
	if (this->Position.x <= 0.0f)
	{
		this->Velocity.x = -this->Velocity.x;
		this->Position.x = 0.0f;
	}
	else if (this->Position.x + this->Size.x >= window_width)
	{
		this->Velocity.x = -this->Velocity.x;
		this->Position.x = window_width - this->Size.x;
	}

	if (this->Position.y <= 0.0f)
	{
		this->Velocity.y = -this->Velocity.y;
		this->Position.y = 0.0f;
	}
	return this->Position;
}

void BombObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
	this->Position = position;
	this->Velocity = velocity;
}