#include "explosion_object.h"

ExplosionObject::ExplosionObject(glm::vec2 position, std::vector<Texture2D> textures, float frameDuration)
	: Position(position), AnimationTextures(textures), CurrentFrame(0), FrameDuration(frameDuration), FrameTimer(0.0f) {}

void ExplosionObject::Update(float dt) {
	FrameTimer += dt;
	if (FrameTimer >= FrameDuration) {
		CurrentFrame++;
		FrameTimer = 0.0f;
	}
}

bool ExplosionObject::IsFinished() {
	return CurrentFrame >= 8;
}

void ExplosionObject::Draw(SpriteRenderer& renderer)
{
	if (!this->IsFinished())
	{
		renderer.DrawSprite(this->AnimationTextures[CurrentFrame], this->Position, glm::vec2(150.0f, 150.0f), this->Rotation);
	}
}
