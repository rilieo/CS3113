#include "LevelC.h"
#include "Utility.h"

#define LEVEL_WIDTH 16
#define LEVEL_HEIGHT 8
#define ENEMY_COUNT 1

signed int LEVELC_DATA[] =
{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    0, 39, 19, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    21, 21, 21, 22, -1, -1, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, 20, 21, 22, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, 31, 32, 33, -1, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2,
    -1, -1, -1, -1, -1, -1, 0, 1, 2, -1, 53, -1, -1, 10, 11, 12,
};

LevelC::~LevelC()
{
    delete [] m_state.enemies;
    delete    m_state.map;
    delete[]  m_state.objects;
}

void LevelC::initialise(Entity* player)
{
    GLuint map_texture_id = Utility::load_texture("assets/images/swamp-tileset/1 Tiles/Tileset.png");

    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELC_DATA, map_texture_id, 1.0f, 10, 6);
    m_state.player = player;
    m_state.player->set_position(glm::vec3(1.0f, 0.0f, 0.0f));
    
    /* Enemies' stuff */
    m_state.enemies = new Entity[ENEMY_COUNT];
    
    GLuint enemy_texture_id = Utility::load_texture("assets/images/enemy_bee_right.png");
    
    m_state.enemies->m_animations[m_state.enemies->ATTACK_RIGHT] = new int[5] {0, 1, 2, 3, 4};
    m_state.enemies[0].m_animation_indices = m_state.enemies->m_animations[m_state.enemies->ATTACK_RIGHT];
    m_state.enemies[0].m_animation_frames  = 5;
    m_state.enemies[0].m_animation_index   = 0;
    m_state.enemies[0].m_animation_time    = 0.0f;
    m_state.enemies[0].m_animation_cols    = 3;
    m_state.enemies[0].m_animation_rows    = 2;
    m_state.enemies[0].set_height(0.9f);
    m_state.enemies[0].set_width(0.9f);
    m_state.enemies[0].set_entity_type(ENEMY);
    m_state.enemies[0].set_ai_type(GUARD);
    m_state.enemies[0].set_ai_state(IDLE);
    m_state.enemies[0].m_texture_id = enemy_texture_id;
    m_state.enemies[0].set_position(glm::vec3(6.0f, 0.0f, 0.0f));
    m_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_state.enemies[0].set_speed(0.5f);
    m_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    
    m_state.objects = new Entity[1];
    m_state.objects->m_texture_id = Utility::load_texture("assets/images/swamp-tileset/4 Animated objects/Chest.png");
    m_state.objects->m_animations[ m_state.objects->CHEST] = new int[4] {0, 1, 2, 3};
    m_state.objects->m_animation_indices =  m_state.objects->m_animations[m_state.objects->CHEST];
    m_state.objects[0].m_animation_frames  = 4;
    m_state.objects[0].m_animation_index   = 0;
    m_state.objects[0].m_animation_time    = 0.0f;
    m_state.objects[0].m_animation_cols    = 4;
    m_state.objects[0].m_animation_rows    = 1;
    m_state.objects[0].set_entity_type(OBJECT);
    m_state.objects[0].set_position(glm::vec3(15.0f, 0.0f, 0.0f));
    m_state.objects[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    
}

void LevelC::update(float delta_time)
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
    
    m_state.objects[0].update(delta_time, m_state.player, NULL, 0, m_state.map, false);
    
    if (m_state.player->check_collision(&m_state.objects[0], 0)) {
        m_state.player->m_got_flag = true;
    }
}


void LevelC::render(ShaderProgram *program)
{
    m_state.map->render(program);
    m_state.objects->render(program);
    m_state.player->render(program);
    
    for (size_t i=0; i < ENEMY_COUNT; ++i) {
        m_state.enemies[i].render(program);
    }
    
}
