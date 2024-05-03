#include "LevelB.h"
#include "Utility.h"

#define LEVEL_WIDTH 16
#define LEVEL_HEIGHT 8
#define ENEMY_COUNT 2
#define DIALOGUE_COUNT 6

signed int LEVELB_DATA[] =
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

LevelB::~LevelB()
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

Entity* LevelB::create_enemy() {
    float ys[] = {-1.87f, -2.87f, -3.87f, -4.87f, -5.87f, -6.876};
    float sizes[] = {0.3f, 0.7f, 0.9f};
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
    enemy->set_speed(0.8f);
    enemy->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    return enemy;
}

void LevelB::initialise(Entity* player)
{
    m_number_of_enemies = ENEMY_COUNT;
    m_state.players.push_back(player);
    GLuint map_texture_id = Utility::load_texture("assets/images/Grass.png");

    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELB_DATA, map_texture_id, 1.0f, 3, 1);
    
    m_state.objects = new Entity[DIALOGUE_COUNT];
    m_state.objects[0].m_texture_id = Utility::load_texture("assets/images/dialogue/B/first.png");
    m_state.objects[0].set_entity_type(DIALOGUE);
    m_state.objects[0].set_position(glm::vec3(4.5f, -3.0f, 0.0f));
    m_state.objects[0].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    
    m_state.objects[1].m_texture_id = Utility::load_texture("assets/images/dialogue/B/second.png");
    m_state.objects[1].set_entity_type(DIALOGUE);
    m_state.objects[1].set_position(glm::vec3(4.5f, -3.0f, 0.0f));
    m_state.objects[1].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    
    m_state.objects[2].m_texture_id = Utility::load_texture("assets/images/dialogue/B/third.png");
    m_state.objects[2].set_entity_type(DIALOGUE);
    m_state.objects[2].set_position(glm::vec3(4.5f, -3.0f, 0.0f));
    m_state.objects[2].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    
    m_state.objects[3].m_texture_id = Utility::load_texture("assets/images/dialogue/B/fourth.png");
    m_state.objects[3].set_entity_type(DIALOGUE);
    m_state.objects[3].set_position(glm::vec3(4.5f, -3.0f, 0.0f));
    m_state.objects[3].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    
    m_state.objects[4].m_texture_id = Utility::load_texture("assets/images/dialogue/B/fifth.png");
    m_state.objects[4].set_entity_type(DIALOGUE);
    m_state.objects[4].set_position(glm::vec3(4.5f, -4.0f, 0.0f));
    m_state.objects[4].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    
    m_state.objects[5].m_texture_id = Utility::load_texture("assets/images/dialogue/B/sixth.png");
    m_state.objects[5].set_entity_type(DIALOGUE);
    m_state.objects[5].set_position(glm::vec3(4.5f, -3.0f, 0.0f));
    m_state.objects[5].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    
    Entity* new_enemy = create_enemy();
    m_state.enemies.push_back(new_enemy);
}

void LevelB::update(float delta_time)
{
    
    if (m_state.next_dialogue > 5)
        m_state.is_dialogue = false;
    else
        m_state.objects[m_state.next_dialogue].update(delta_time, NULL, NULL, 0, m_state.map);
    
    if (!m_state.is_dialogue) {
        for (int i = 0; i < m_state.players.size(); i++) {
            m_state.players[i]->update(delta_time, NULL, NULL, 0, m_state.map);
            m_state.players[i]->weapon->update(delta_time, m_state.players[i], NULL, 0, m_state.map);
            deactivate(m_state.enemies, m_state.players, i, WEAPON);
            
            for (int j = 0; j < m_state.enemies.size(); j++) {
                if (abs(m_state.players[i]->get_position().x - m_state.enemies[j]->get_position().x) < 2.0f)
                    m_state.players[i]->weapon->deactivate();
            }
        }
        
        for (int i = 0; i < m_state.enemies.size(); i++) {
            m_state.enemies[i]->update(delta_time, NULL, NULL, 0, m_state.map);
            deactivate(m_state.enemies, m_state.players, i, PLAYER);
            
            if (m_state.enemies[i]->get_active() && m_state.enemies[i]->m_crossed) m_enemy_crossed = true;
        }
        
        if (m_state.countdown < 0.0f && m_state.enemies.size() != ENEMY_COUNT) {
            Entity* new_enemy = create_enemy();
            m_state.enemies.push_back(new_enemy);
            m_state.countdown = 7.0f;
        }
        
        m_state.countdown -= (0.5f * delta_time);
    }
}


void LevelB::render(ShaderProgram *program)
{
    m_state.map->render(program);
    
    if (m_state.next_dialogue <= 5)
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

void LevelB::deactivate(std::vector<Entity*>& enemies, std::vector<Entity*>& players, int index, EntityType entity_collide) {
    
    if (entity_collide == WEAPON)
        for (int i = 0; i < enemies.size(); i++) {
            if (enemies[i]->get_active() && enemies[i]->check_collision(players[index]->weapon, -0.5) && players[index]->weapon->get_position().y <= enemies[i]->get_position().y + 0.5f && players[index]->weapon->get_position().y >= enemies[i]->get_position().y - 0.5f) {
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
            if (players[i]->get_active() && players[i]->check_collision(enemies[index], 1) && players[i]->get_position().y <= enemies[index]->get_position().y + 0.5f && players[i]->get_position().y >= enemies[index]->get_position().y - 0.5f ) {
                players[i]->m_animation_indices = players[i]->m_animations[players[i]->HURT_RIGHT];
                m_number_of_killed_players += 1;
                players[i]->deactivate();
            }
        }

}

void LevelB::reset() {
    for (int i = 0; i < m_state.players.size(); i++) {
        if (m_state.players[i]->get_active()) {
            m_state.players[i]->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
            
            m_state.players[i]->weapon->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
        }
    }
    
    for (int i = 0; i < m_state.enemies.size(); i++) {
        if (m_state.enemies[i]->get_active()) {
            m_state.enemies[i]->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
        }
    }
}
