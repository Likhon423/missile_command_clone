#pragma once
#include <vector>
#include <tuple>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_END
};

// directions
enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

// Initial bomb velocity and radius
const glm::vec2 INITIAL_BOMB_VELOCITY(50.0f, 100.0f);
const float BOMB_RADIUS = 18.0f;

class Game
{
public:
    // game state
    GameState               State;
    bool                    Keys[1024];
    bool                    KeysProcessed[1024];
    // missile created flag
    bool                    MissileCreated = false;
    unsigned int            Width, Height;
    unsigned int            Lives;
    unsigned int            Score;
    // constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void Init();
    // game loop
    void ProcessInput(GLFWwindow* window, float dt);
    void Update(float dt);
    void Render();
    void ResetGame();
};