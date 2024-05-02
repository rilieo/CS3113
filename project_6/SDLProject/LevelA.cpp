#include "LevelA.h"
#include "Dog.h"
#include "Utility.h"

#define LEVEL_WIDTH 16
#define LEVEL_HEIGHT 8
#define ENEMY_COUNT 4

float ys[] = {-1.87f, -2.87f, -3.87f, -4.87f, -5.87f, -6.876};
float sizes[] = {0.3f, 0.7f, 0.9f};

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
    
    for (size_t i = 0; i < m_state.players.size(); i++) {
        delete m_state.players[i];
    }
    
    for (size_t i = 0; i < m_state.enemies.size(); i++) {
        delete m_state.enemies[i];
    }
}

Entity* LevelA::create_enemy() {
    int random = rand() % 5;
    int other_random = rand() % 3;
    Entity* enemy = new Entity();
    GLuint enemy_texture_id = Utility::load_texture("assets/images/enemy_bee_left.png");
    enemy->m_animations[enemy->ATTACK_LEFT] = new int[5] {0, 1, 2, 4, 5};
    enemy->m_animation_indices = enemy->m_animations[enemy->ATTACK_LEFT];
    enemy->m_animation_frames  = 5;
    enemy->m_animation_index   = 0;
    enemy->m_animation_time    = 0.0f;
    enemy->m_animation_cols    = 3;
    enemy->m_animation_rows    = 2;
    enemy->set_height(sizes[other_random]);
    enemy->set_width(sizes[other_random]);
    enemy->set_entity_type(ENEMY);
    enemy->set_ai_type(ZOMBIE);
    enemy->set_ai_state(WALKING);
    enemy->m_texture_id = enemy_texture_id;
    enemy->set_position(glm::vec3(11.0f, ys[random], 0.0f));
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
    
    Entity* new_enemy = create_enemy();
    m_state.enemies.push_back(new_enemy);
    
}

void LevelA::update(float delta_time)
{
    for (int i = 0; i < m_state.players.size(); i++) {
        if (i < m_state.enemies.size()) {
            m_state.players[i]->update(delta_time, m_state.players[i], m_state.enemies[i], m_state.enemies.size(), m_state.map);
            m_state.players[i]->weapon->update(delta_time, m_state.players[i], m_state.enemies[i], m_state.enemies.size(), m_state.map);
            deactivate(m_state.enemies, m_state.players, i, WEAPON);
        }
        else {
            m_state.players[i]->update(delta_time, m_state.players[i], NULL, 0, m_state.map);
            m_state.players[i]->weapon->update(delta_time, m_state.players[i], NULL, 0, m_state.map);
        }
    }
    
    for (int i = 0; i < m_state.enemies.size(); i++) {
        if (i < m_state.players.size()) {
            m_state.enemies[i]->update(delta_time, m_state.players[i], m_state.enemies[i], 0, m_state.map);
            deactivate(m_state.enemies, m_state.players, i, PLAYER);
        }
        else
            m_state.enemies[i]->update(delta_time, NULL, m_state.enemies[i], 0, m_state.map);
        
        if (m_state.enemies[i]->m_crossed) m_enemy_crossed = true;
    }
    
    if (m_state.countdown < 0.0f && m_state.enemies.size() != ENEMY_COUNT) {
        Entity* new_enemy = create_enemy();
        m_state.enemies.push_back(new_enemy);
        m_state.countdown = 10.0f;
    }
    
    m_state.countdown -= (0.5f * delta_time);

}


void LevelA::render(ShaderProgram *program)
{
    m_state.map->render(program);
    m_state.first_screen->render(program);
    
    for (size_t i=0; i < m_state.enemies.size(); ++i) {
        m_state.enemies[i]->render(program);
    }
    
    for (size_t i=0; i < m_state.players.size(); ++i) {
        m_state.players[i]->render(program);
        
        if (m_state.players[i]->m_is_planted)
            m_state.players[i]->weapon->render(program);
    }
}

void LevelA::deactivate(std::vector<Entity*> enemies, std::vector<Entity*> players, int index, EntityType entity_collide) {
    
    if (entity_collide == WEAPON)
        for (int i = 0; i < enemies.size(); i++) {
            if (enemies[i]->check_collision(players[index]->weapon, -0.5)) {
                
                if (enemies[i]->m_num_hits <= 0) {
                    enemies[i]->deactivate();
                    break;
                }
                
                enemies[i]->m_num_hits -= 1;
                players[index]->weapon->deactivate();
                glm::vec3 new_pos = players[index]->get_position();
                new_pos.x += 1;
                players[index]->weapon->set_position(new_pos);
                players[index]->weapon->activate();
                
            }
        }
    
    if (entity_collide == PLAYER)
        for (int i = 0; i < players.size(); i++) {
            if (players[i]->check_collision(enemies[index], 1)) {
                m_number_of_killed_players += 1;
                players[i]->deactivate();
            }
        }

}
