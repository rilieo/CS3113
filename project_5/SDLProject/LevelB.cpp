/*
* Author: Riley Dou
* Assignment: Platformer
* Date due: 2024-04-13, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
*/

#include "LevelB.h"
#include "Utility.h"

#define LEVEL_WIDTH 16
#define LEVEL_HEIGHT 8
#define ENEMY_COUNT 2

signed int LEVELB_DATA[] =
{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, 31, 32, 33, -1, -1, -1, -1, -1, -1, -1, -1,
    9, 19, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    11, 11, 12, -1, -1, -1, -1, -1, -1, -1, 0, 9, 19, 29, 2, -1,
    11, 11, 12, -1, 46, 1, 39, 2, -1, -1, 10, 11, 11, 11, 12, -1,
    11, 11, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

LevelB::~LevelB()
{
    delete [] m_state.enemies;
    delete    m_state.map;
}

void LevelB::initialise(Entity* player)
{
    GLuint map_texture_id = Utility::load_texture("assets/images/swamp-tileset/1 Tiles/Tileset.png");

    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELB_DATA, map_texture_id, 1.0f, 10, 6);
    
    m_state.objects = new Entity[1];
    m_state.objects[0].m_texture_id = Utility::load_texture("assets/images/swamp-tileset/3 Objects/Pointers/6.png");
    m_state.objects[0].set_scale(glm::vec3(0.5f, 0.5f, 1.0f));
    m_state.objects[0].set_position(glm::vec3(10.0f, -3.0f, 0.0f));
    m_state.objects[0].set_entity_type(OBJECT);
    m_state.objects[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));

    m_state.player = player;
    m_state.player->set_position(glm::vec3(1.0f, 0.0f, 0.0f));
    
    /* Enemies' stuff */
    m_state.enemies = new Entity[ENEMY_COUNT];
    
    GLuint enemy_texture_id = Utility::load_texture("assets/images/enemy_bee_right.png");
    
    m_state.enemies->m_animations[m_state.enemies->ATTACK_RIGHT] = new int[5] {0, 1, 2, 3, 4};
    AIType types[] = {GUARD, JUMPER};
    float positions[] = {2.5f, 7.0f};
    
    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_state.enemies[i].m_animation_indices = m_state.enemies->m_animations[m_state.enemies->ATTACK_RIGHT];
        m_state.enemies[i].m_animation_frames  = 5;
        m_state.enemies[i].m_animation_index   = 0;
        m_state.enemies[i].m_animation_time    = 0.0f;
        m_state.enemies[i].m_animation_cols    = 3;
        m_state.enemies[i].m_animation_rows    = 2;
        m_state.enemies[i].set_height(0.9f);
        m_state.enemies[i].set_width(0.9f);
        m_state.enemies[i].set_entity_type(ENEMY);
        m_state.enemies[i].set_ai_type(types[i]);
        
        if (m_state.enemies[i].get_ai_type() == GUARD)
            m_state.enemies[i].set_ai_state(IDLE);
            
        m_state.enemies[i].m_texture_id = enemy_texture_id;
        m_state.enemies[i].set_position(glm::vec3(positions[i], 0.0f, 0.0f));
        m_state.enemies[i].set_movement(glm::vec3(0.0f));
        m_state.enemies[i].set_speed(0.5f);
        m_state.enemies[i].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    }
}

void LevelB::update(float delta_time)
{
    if (m_state.is_frozen) {
        m_state.player->update(delta_time, m_state.player, NULL, 0, m_state.map, true);
        
        for (int i = 0; i < ENEMY_COUNT; i++)
        {
            m_state.enemies[i].update(delta_time, m_state.player, NULL, 0, m_state.map, true);
        }
        
        m_state.objects[0].update(delta_time, m_state.player, NULL, 0, m_state.map, true);
        
        return;
    }
    
    m_state.player->update(delta_time, m_state.player, NULL, 0, m_state.map, false);
    
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_state.enemies[i].update(delta_time, m_state.player, NULL, 0, m_state.map, false);
    }
    
    m_state.objects->update(delta_time, m_state.player, NULL, 0, m_state.map, false);
}


void LevelB::render(ShaderProgram *program)
{
    m_state.map->render(program);
    m_state.objects->render(program);
    m_state.player->render(program);
    
    for (size_t i=0; i < ENEMY_COUNT; ++i) {
        m_state.enemies[i].render(program);
    }
    
}
