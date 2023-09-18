#include <iostream>
#include <sstream>
#include <vector>
#include <ctime>
#include <cstdlib>

#include <irrklang/irrKlang.h>
using namespace irrklang;

#pragma comment(lib, "irrKlang.lib")

#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "bomb_object.h"
#include "missile_object.h"
#include "explosion_object.h"
#include "text_renderer.h"

SpriteRenderer* Renderer;
TextRenderer* Text;
ISoundEngine* SoundEngine = createIrrKlangDevice();
ISoundSource* explosionSound = SoundEngine->addSoundSourceFromFile("audio/explosion.wav");
std::vector<BombObject*> bombs;
std::vector<MissileObject*> missiles;
std::vector<Texture2D> explosionTextures;
std::vector<ExplosionObject> explosions;

float timer = 0.0f;
float interval = 1.0f;
glm::vec2 bomb_velocity = INITIAL_BOMB_VELOCITY;

float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (max - min));
}

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_MENU), Keys(), KeysProcessed(), Width(width), Height(height), Lives(5), Score(0)
{
    
}

Game::~Game()
{
    delete Renderer;

    for (auto& bomb : bombs) {
        delete bomb;
    }
    bombs.clear();

    for (auto& missile : missiles) {
        delete missile;
    }
    missiles.clear();
    delete Text;
    SoundEngine->drop();
}

void Game::ResetGame() {
    this->Lives = 5;
    this->Score = 0;
    this->MissileCreated = false;

    for (auto& bomb : bombs) {
        delete bomb;
    }
    bombs.clear();

    for (auto& missile : missiles) {
        delete missile;
    }
    missiles.clear();

    explosions.clear();
}

void Game::Init()
{
    // load shaders
    ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");

    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

    // set render-specific controls
    Shader spriteShader = ResourceManager::GetShader("sprite");
    Renderer = new SpriteRenderer(spriteShader);

    // load textures
    ResourceManager::LoadTexture("textures/Background_space.png", true, "background");
    ResourceManager::LoadTexture("textures/city8.png", true, "city");
    ResourceManager::LoadTexture("textures/Tank.png", true, "tank");
    ResourceManager::LoadTexture("textures/Bomb.png", true, "bomb");
    ResourceManager::LoadTexture("textures/Missile.png", true, "missile");

    // explosion textures
    ResourceManager::LoadTexture("textures/Explosion_A.png", true, "explosion1");
    ResourceManager::LoadTexture("textures/Explosion_B.png", true, "explosion2");
    ResourceManager::LoadTexture("textures/Explosion_C.png", true, "explosion3");
    ResourceManager::LoadTexture("textures/Explosion_D.png", true, "explosion4");
    ResourceManager::LoadTexture("textures/Explosion_E.png", true, "explosion5");
    ResourceManager::LoadTexture("textures/Explosion_F.png", true, "explosion6");
    ResourceManager::LoadTexture("textures/Explosion_G.png", true, "explosion7");
    ResourceManager::LoadTexture("textures/Explosion_H.png", true, "explosion8");

    explosionTextures.push_back(ResourceManager::GetTexture("explosion1"));
    explosionTextures.push_back(ResourceManager::GetTexture("explosion2"));
    explosionTextures.push_back(ResourceManager::GetTexture("explosion3"));
    explosionTextures.push_back(ResourceManager::GetTexture("explosion4"));
    explosionTextures.push_back(ResourceManager::GetTexture("explosion5"));
    explosionTextures.push_back(ResourceManager::GetTexture("explosion6"));
    explosionTextures.push_back(ResourceManager::GetTexture("explosion7"));
    explosionTextures.push_back(ResourceManager::GetTexture("explosion8"));

    // load text
    Text = new TextRenderer(this->Width, this->Height);
    Text->Load("fonts/ocraext.TTF", 24);

    // load audio
    SoundEngine->play2D("audio/missile_command.mp3", true);
    explosionSound->setDefaultVolume(0.1f);
}

void Game::Update(float dt)
{
    if (this->State == GAME_ACTIVE) {
        timer += dt;

        bomb_velocity.y += 1.0f * dt;

        // generate bombs at random x position
        if (timer >= interval) {
            float randomX = randomFloat(0.0f, this->Width);
            float direction = (rand() % 2 == 0) ? 1 : -1;

            BombObject* newBomb = new BombObject(glm::vec2(randomX, 0.0f), BOMB_RADIUS, direction, bomb_velocity, ResourceManager::GetTexture("bomb"));
            bombs.push_back(newBomb);

            timer = 0.0f;
        }

        // move missiles
        for (auto it = missiles.begin(); it != missiles.end();)
        {
            MissileObject* missilePtr = *it;
            bool reachedDestination = missilePtr->Move(dt);

            if (reachedDestination)
            {
                glm::vec2 explosionPosition = missilePtr->Position - glm::vec2(75.0f, 75.0f);

                ExplosionObject newExplosion(explosionPosition, explosionTextures, 0.05f);
                explosions.push_back(newExplosion);

                

                SoundEngine->play2D(explosionSound);

                delete missilePtr;
                it = missiles.erase(it);

                break;
            }
            else
            {
                ++it;
            }
        }

        for (auto& explosion : explosions) {
            explosion.Update(dt);
        }

        // collision detection
        for (auto& bombPtr : bombs) {
            BombObject& bomb = *bombPtr;
            bomb.Move(dt, this->Width);

            bool bombHit = false;

            // Check for collision with explosions
            for (auto& explosion : explosions) {
                if (!explosion.IsFinished()) {
                    glm::vec2 bombCenter = bomb.Position + glm::vec2(bomb.Radius, bomb.Radius);
                    glm::vec2 explosionCenter = explosion.Position + glm::vec2(75.0f, 75.0f);
                    float distance = glm::distance(bombCenter, explosionCenter);

                    if (distance <= 75.0f) {
                        bombHit = true;
                        break;
                    }
                }
            }

            if (bombHit) {
                bomb.Destroyed = true;
                this->Score++;
            }

            if (bomb.Position.y >= this->Height) {
                this->Lives--;
                bomb.Destroyed = true;
                SoundEngine->play2D("audio/error.mp3", false);
            }
        }

        bombs.erase(std::remove_if(bombs.begin(), bombs.end(), [](BombObject* bombPtr) {
            return bombPtr->Destroyed;
            }), bombs.end());
    }

    if (this->Lives <= 0) {
        this->State = GAME_END;
    }
}

void Game::ProcessInput(GLFWwindow *window, float dt)
{
    if (this->State == GAME_MENU)
    {
        if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])
        {
            this->State = GAME_ACTIVE;
            this->KeysProcessed[GLFW_KEY_ENTER] = true;
        }
    }

    if (this->State == GAME_ACTIVE) {
        // create missiles if player clicks left mouse button
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !MissileCreated)
        {
            this->MissileCreated = true;

            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            glm::vec2 mouseClickPosition = glm::vec2(static_cast<float>(xpos), static_cast<float>(ypos));

            MissileObject* newMissile = new MissileObject(glm::vec2(this->Width / 2.0f - 10.0f, this->Height - 105.0f), glm::vec2(20.0f, 30.0f), ResourceManager::GetTexture("missile"), mouseClickPosition);
            missiles.push_back(newMissile);
        }
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
        {
            this->MissileCreated = false;
        }
    }

    if (this->State == GAME_END)
    {
        if (this->Keys[GLFW_KEY_ENTER])
        {
            this->KeysProcessed[GLFW_KEY_ENTER] = true;
            this->State = GAME_MENU;
            ResetGame();
        }
    }
}

void Game::Render()
{
    if (this->State == GAME_ACTIVE || this->State == GAME_MENU || this->State == GAME_END)
    {
        // render background and tank
        Texture2D backgroundTex = ResourceManager::GetTexture("background");
        Renderer->DrawSprite(backgroundTex, glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
        Texture2D cityTex = ResourceManager::GetTexture("city");
        Renderer->DrawSprite(cityTex, glm::vec2(0.0f, this->Height - 170.0f), glm::vec2(this->Width, 170.0f), 0.0f);
        Texture2D tankTex = ResourceManager::GetTexture("tank");
        Renderer->DrawSprite(tankTex, glm::vec2(this->Width / 2.0f - 30.0f, this->Height - 75.0f), glm::vec2(60.0f, 75.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    }

    if (this->State == GAME_ACTIVE) {
        std::stringstream ss; ss << this->Lives;
        Text->RenderText("Lives: " + ss.str(), 5.0f, 5.0f, 1.0f);

        std::stringstream ss2; ss2 << this->Score;
        Text->RenderText("Score: " + ss2.str(), 1050.0f, 5.0f, 1.0f);

        // render bombs
        for (auto& bombPtr : bombs) {
            BombObject& bomb = *bombPtr;
            bomb.Draw(*Renderer);
        }

        // render missiles
        for (auto& missilePtr : missiles) {
            MissileObject& missile = *missilePtr;
            missile.Draw(*Renderer);
        }

        // render explosion
        for (auto& explosion : explosions) {
            explosion.Draw(*Renderer);
        }
    }

    if (this->State == GAME_MENU)
    {
        Text->RenderText("Press ENTER to start", this->Width / 2.0f - 150.0f, this->Height / 2.0f, 1.0f);
    }

    if (this->State == GAME_END)
    {
        std::stringstream ss; ss << this->Score;
        Text->RenderText(
            "Final Score: " + ss.str(), this->Width / 2.0f - 100.0f, this->Height / 2.0f - 20.0, 1.0, glm::vec3(0.0, 1.0, 0.0)
        );
        Text->RenderText(
            "Press ENTER to retry or ESC to quit", 350.0, this->Height / 2.0f, 1.0, glm::vec3(1.0, 1.0, 0.0)
        );
    }
}