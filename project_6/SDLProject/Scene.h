#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_opengl.h>

#include "Entity.h"
#include "Map.h"
#include "ShaderProgram.h"
#include "Utility.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/mat4x4.hpp"
#include <vector>

struct Menu {
    glm::mat4 model_matrix;
    GLuint texture_id;
};

struct GameState {
    // ————— GAME OBJECTS ————— //
    Map* map;
    Map* first_screen;
    std::vector<Entity*> players;
    std::vector<Entity*> enemies;
    Entity* objects;
    std::vector<Entity*> weapons;
    
    float countdown = 7.0f;
    int next_dialogue = 0;
    bool is_dialogue = true;
    
};

class Scene {
   public:
    // ————— ATTRIBUTES ————— //
    int m_number_of_enemies = 1;
    int m_number_of_killed_enemies = 0;
    int m_number_of_killed_players = 0;
    bool m_enemy_crossed = false;

    GameState m_state;
    Menu m_menu;
    Mix_Chunk *g_bark_sfx;

    // ————— METHODS ————— //
    virtual void initialise(Entity* player)                                = 0;
    virtual void update(float delta_time)                                  = 0;
    virtual void render(ShaderProgram *program)                            = 0;
    virtual Entity* create_enemy()                                         = 0;
    virtual void reset()                                                   = 0;

    // ————— GETTERS ————— //
    GameState const get_state() const { return m_state; }
    int const       get_number_of_enemies() const { return m_number_of_enemies; }
};
