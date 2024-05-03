#include "LevelC.h"
#include "Utility.h"
#include <cmath>

#define LEVEL_WIDTH 16
#define LEVEL_HEIGHT 8
#define ENEMY_COUNT 1
#define DIALOGUE_COUNT 1

signed int LEVELC_DATA[] =
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

LevelC::~LevelC()
{
    delete    m_state.map;

    for (int i = 0; i < m_state.players.size(); i++) {
        delete m_state.players[i]->weapon;
        delete m_state.players[i];
        m_state.players[i]->weapon = nullptr;
        m_state.players[i] = nullptr;
    }
    
    m_state.players.clear();
    
    for (int i = 0; i < m_state.enemies.size(); i++) {
        delete m_state.enemies[i];
        m_state.enemies[i] = nullptr;
    }
    
    m_state.enemies.clear();
    
    delete[] m_state.objects;
}

Entity* LevelC::create_enemy() {
    Entity* enemy = new Entity();
    GLuint enemy_texture_id = Utility::load_texture("assets/images/enemy_bee_left.png");
    enemy->m_animations[enemy->ATTACK_LEFT] = new int[5] {0, 1, 2, 4, 5};
    enemy->m_animation_indices = enemy->m_animations[enemy->ATTACK_LEFT];
    enemy->m_animation_frames  = 5;
    enemy->m_animation_index   = 0;
    enemy->m_animation_time    = 0.0f;
    enemy->m_animation_cols    = 3;
    enemy->m_animation_rows    = 2;
    enemy->set_height(15.0f);
    enemy->set_width(4.5f);
    enemy->set_entity_type(BOSS);
    enemy->set_ai_type(ZOMBIE);
    enemy->set_ai_state(WALKING);
    enemy->m_texture_id = enemy_texture_id;
    enemy->set_position(glm::vec3(12.0f, -3.5, 0.0f));
    enemy->set_movement(glm::vec3(0.0f));
    enemy->set_speed(0.3f);
    enemy->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    enemy->set_num_hits(30);
    return enemy;
}

void LevelC::initialise(Entity* player)
{
    m_number_of_enemies = ENEMY_COUNT;
    m_state.players.push_back(player);
    GLuint map_texture_id = Utility::load_texture("assets/images/Grass.png");

    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELC_DATA, map_texture_id, 1.0f, 3, 1);
    
    m_state.objects = new Entity[DIALOGUE_COUNT];
    m_state.objects[0].m_texture_id = Utility::load_texture("assets/images/dialogue/C/first.png");
    m_state.objects[0].set_entity_type(DIALOGUE);
    m_state.objects[0].set_position(glm::vec3(4.5f, -3.0f, 0.0f));
    m_state.objects[0].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    
    Entity* new_enemy = create_enemy();
    m_state.enemies.push_back(new_enemy);
}

void LevelC::update(float delta_time)
{
    
    if (m_state.next_dialogue > DIALOGUE_COUNT-1)
        m_state.is_dialogue = false;
    else
        m_state.objects[m_state.next_dialogue].update(delta_time, NULL, NULL, 0, m_state.map);
    
    if (!m_state.is_dialogue) {
        for (int i = 0; i < m_state.players.size(); i++) {
            
            m_state.players[i]->update(delta_time, NULL, NULL, 0, m_state.map);
            m_state.players[i]->weapon->update(delta_time, m_state.players[i], NULL, 0, m_state.map);
            deactivate(m_state.enemies, m_state.players, i, WEAPON);
            
            std::cout << "player: " << m_state.players[i]->get_position().x << "enemy: " << m_state.enemies[0]->get_position().x << std::endl;
            if (abs(m_state.players[i]->get_position().x - m_state.enemies[0]->get_position().x) < 2.5f)
                m_state.players[i]->weapon->deactivate();
        }
        
        for (int i = 0; i < m_state.enemies.size(); i++) {
            m_state.enemies[i]->update(delta_time, NULL, NULL, 0, m_state.map);
            deactivate(m_state.enemies, m_state.players, i, PLAYER);
            
            if (m_state.enemies[i]->get_active() && m_state.enemies[i]->m_crossed) {m_enemy_crossed = true; m_state.enemies[i]->deactivate();}
        }
    }
}


void LevelC::render(ShaderProgram *program)
{
    m_state.map->render(program);
    
    if (m_state.next_dialogue <= DIALOGUE_COUNT-1)
        m_state.objects[m_state.next_dialogue].render(program);
    
    if (!m_state.is_dialogue) {
        for (size_t i=0; i < m_state.enemies.size(); ++i) {
            m_state.enemies[i]->render(program);
        }
        
        for (size_t i=0; i < m_state.players.size(); ++i) {
            
            m_state.players[i]->render(program);
            
            if (m_state.players[i]->m_is_planted)
                m_state.players[i]->weapon->render(program);
        }
    }
}

void LevelC::deactivate(std::vector<Entity*>& enemies, std::vector<Entity*>& players, int index, EntityType entity_collide) {
    
    if (entity_collide == WEAPON)
        for (int i = 0; i < enemies.size(); i++) {
            
            if (enemies[i]->get_active() && enemies[i]->check_collision(players[index]->weapon, -0.1f) && players[index]->weapon->get_position().y <= enemies[i]->get_position().y + 0.5f && players[index]->weapon->get_position().y >= enemies[i]->get_position().y - 0.5f) {
                if (enemies[i]->m_num_hits <= 0) {
                    m_number_of_killed_enemies += 1;
                    enemies[i]->deactivate();
                }
                else {
                    enemies[i]->m_num_hits -= 1;
                    players[index]->weapon->deactivate();
                    glm::vec3 new_pos = players[index]->get_position();
                    new_pos.x += 1;
                    players[index]->weapon->set_position(new_pos);
                    players[index]->weapon->activate();
                }
                
            }
        }
    
    if (entity_collide == PLAYER)
        for (int i = 0; i < players.size(); i++) {
            if (players[i]->get_active() && players[i]->check_collision(enemies[index], 1.0f) && players[i]->get_position().y <= enemies[index]->get_position().y + 0.5f && players[i]->get_position().y >= enemies[index]->get_position().y - 0.5f ) {
                players[i]->m_animation_indices = players[i]->m_animations[players[i]->HURT_RIGHT];
                m_number_of_killed_players += 1;
                players[i]->deactivate();
            }
        }

}

void LevelC::reset() {
    for (int i = 0; i < m_state.players.size(); i++) {
        if (m_state.players[i]->get_active()) {
            m_state.players[i]->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
            
            m_state.players[i]->weapon->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
        }
    }
    
    for (int i = 0; i < m_state.enemies.size(); i++) {
        if (m_state.enemies[i]->get_active()) {
            m_state.enemies[i]->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
            m_state.enemies[i]->set_velocity(glm::vec3(0.0f, 0.0f, 0.0f));
        }
    }
}
