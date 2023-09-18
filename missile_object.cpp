#include "missile_object.h"

MissileObject::MissileObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec2 destination)
	: GameObject(pos, size, sprite), Destination(destination) { }

bool MissileObject::Move(float dt)
{
    glm::vec2 direction = glm::normalize(this->Destination - this->Position);
    float distance = glm::distance(this->Position, this->Destination);

    this->Rotation = glm::degrees(glm::atan(direction.y, direction.x)) + 90.0f;

    float movementDistance = this->Speed * dt;

    // check if reached destination
    if (movementDistance >= distance)
    {
        this->Position = this->Destination;
        return true;
    }
    else
    {
        glm::vec2 movement = direction * movementDistance;
        this->Position += movement;
        return false;
    }

}