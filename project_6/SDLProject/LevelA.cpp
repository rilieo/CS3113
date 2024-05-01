/*
* Author: Riley Dou
* Assignment: Platformer
* Date due: 2024-04-13, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
*/

#include "LevelA.h"
#include "Utility.h"
#include <time.h>       /* time */

#define LEVEL_WIDTH 16
#define LEVEL_HEIGHT 8
#define ENEMY_COUNT 3

float ys[] = {1.87f, -2.87f, -3.87f, -4.87f, -5.87f, -6.876};

signed int LEVELA_DATA[] =
{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

LevelA::~LevelA()
{
    delete    m_state.map;
}

Entity* LevelA::create_enemy(int y) {
    Entity* enemy = new Entity();
    GLuint enemy_texture_id = Utility::load_texture("assets/images/enemy_bee_left.png");
    enemy->m_animations[enemy->ATTACK_LEFT] = new int[5] {0, 1, 2, 4, 5};
    enemy->m_animation_indices = enemy->m_animations[enemy->ATTACK_LEFT];
    enemy->m_animation_frames  = 5;
    enemy->m_animation_index   = 0;
    enemy->m_animation_time    = 0.0f;
    enemy->m_animation_cols    = 3;
    enemy->m_animation_rows    = 2;
    enemy->set_height(0.9f);
    enemy->set_width(0.9f);
    enemy->set_entity_type(ENEMY);
    enemy->set_ai_type(ZOMBIE);
    enemy->set_ai_state(WALKING);
    enemy->m_texture_id = enemy_texture_id;
    enemy->set_position(glm::vec3(11.0f, ys[y], 0.0f));
    enemy->set_movement(glm::vec3(0.0f));
    enemy->set_speed(0.5f);
    enemy->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    return enemy;
}

void LevelA::initialise(Entity* player)
{
    m_state.players.push_back(player);
    GLuint map_texture_id = Utility::load_texture("assets/images/Grass.png");

    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELA_DATA, map_texture_id, 1.0f, 3, 1);
    
    int random = rand() % 5;
    Entity* new_enemy = create_enemy(random);
    m_state.enemies.push_back(new_enemy);
    
}

void LevelA::update(float delta_time)
{
    for (int i = 0; i < m_state.players.size(); i++) {
       
        if (i < m_state.enemies.size()) {
            
            m_state.players[i]->update(delta_time, m_state.players[i], m_state.enemies[i], m_state.enemies.size(), m_state.map, m_state.planted_players);
            m_state.players[i]->weapon->update(delta_time, m_state.players[i], m_state.enemies[i], m_state.enemies.size(), m_state.map, m_state.planted_players);
            
        }
        else {
            m_state.players[i]->update(delta_time, m_state.players[i], NULL, 0, m_state.map, m_state.planted_players);
            m_state.players[i]->weapon->update(delta_time, m_state.players[i], NULL, 0, m_state.map, m_state.planted_players);
        }
    }
    
    for (int i = 0; i < m_state.enemies.size(); i++) {
        if (i < m_state.players.size())
            
            m_state.enemies[i]->update(delta_time, m_state.players[i], m_state.enemies[i], 0, m_state.map, m_state.planted_players);
        
        else
            m_state.enemies[i]->update(delta_time, NULL, m_state.enemies[i], 0, m_state.map, m_state.planted_players);
    }
    
    if (m_state.countdown < 0.0f && m_state.enemies.size() != ENEMY_COUNT) {
        int random = rand() % 5;
        Entity* new_enemy = create_enemy(random);
        m_state.enemies.push_back(new_enemy);
        m_state.countdown = 10.0f;
    }
    
    m_state.countdown -= (0.75f * delta_time);

}


void LevelA::render(ShaderProgram *program)
{
    m_state.map->render(program);
    
    for (size_t i=0; i < m_state.enemies.size(); ++i) {
        m_state.enemies[i]->render(program);
    }
    
    for (size_t i=0; i < m_state.players.size(); ++i) {
        m_state.players[i]->render(program);
        
        if (m_state.players[i]->m_is_planted)
            m_state.players[i]->weapon->render(program);
    }
    
    
    
    
    
}
