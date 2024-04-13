#include "Map.h"

enum EntityType { PLATFORM, PLAYER, ENEMY, OBJECT   };
enum AIType     { PATROLLER, GUARD, JUMPER  };
enum AIState    { WALKING, IDLE, ATTACKING  };

class Entity
{
private:
    bool m_is_active = true;

    // ––––– ANIMATION ––––– //
    int* m_animation_walk_right = NULL, // move to the right
       * m_animation_walk_left = NULL, // move to the left
       * m_animation_attack_right = NULL, // attack to the right
       * m_animation_attack_left = NULL, // attack to the left
       * m_animation_hurt_right = NULL, // attack to the left
       * m_animation_hurt_left = NULL, // attack to the left
       * m_animation_chest = NULL;
    
    // ––––– PHYSICS (GRAVITY) ––––– //
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;

    // ————— TRANSFORMATIONS ————— //
    float     m_speed;
    glm::vec3 m_movement;
    glm::mat4 m_model_matrix;


    // ————— ENEMY AI ————— //
    EntityType m_entity_type;
    AIType     m_ai_type;
    AIState    m_ai_state;

    float m_width = 1;
    float m_height = 1;


public:
    // ————— STATIC VARIABLES ————— //
    static const int    SECONDS_PER_FRAME = 6;
    static const int    WALK_LEFT    = 0,
                        WALK_RIGHT   = 1,
                        ATTACK_LEFT  = 2,
                        ATTACK_RIGHT = 3,
                        HURT_LEFT    = 4,
                        HURT_RIGHT   = 5,
                        CHEST = 6;

    // ————— ANIMATION ————— //
    int** m_animations = new int* [7]
        {
            m_animation_walk_left,
            m_animation_walk_right,
            m_animation_attack_left,
            m_animation_attack_right,
            m_animation_hurt_left,
            m_animation_hurt_right,
            m_animation_chest,
        };

    int m_animation_frames  = 0,
        m_animation_index   = 0,
        m_animation_cols    = 0,
        m_animation_rows    = 0;

    int*    m_animation_indices = NULL;
    float   m_animation_time = 0.0f;

    // ––––– PHYSICS (JUMPING) ––––– //
    bool  m_is_jumping = false;
    float m_jumping_power = 0;

    // ––––– PHYSICS (COLLISIONS) ––––– //
    bool m_collided_top = false;
    bool m_collided_bottom = false;
    bool m_collided_left = false;
    bool m_collided_right = false;
    
    bool m_hit = false;
    bool m_got_flag = false;
    
    float m_countdown = 0.7f;

    GLuint    m_texture_id;

    // ————— METHODS ————— //
    Entity();
    ~Entity();

    void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
    void update(float delta_time, Entity* player, Entity* objects, int object_count, Map* map, bool frozen); // Now, update should check for both objects in the game AND the map
    void render(ShaderProgram* program);

    bool const check_collision(Entity* other, const float extra) const;
    void const check_collision_y(Entity* collidable_entities, int collidable_entity_count);
    void const check_collision_x(Entity* collidable_entities, int collidable_entity_count);

    // Overloading our methods to check for only the map
    void const check_collision_y(Map* map);
    void const check_collision_x(Map* map);

    void move_left()    { m_movement.x = -1.0f; };
    void move_right()   { m_movement.x = 1.0f; };
    void move_up()      { m_movement.y = 1.0f; };
    void move_down()    { m_movement.y = -1.0f; };

    void ai_activate(Entity* player);
    void ai_patrol();
    void ai_guard(Entity* player);
    void ai_jump();

    void activate() { m_is_active = true; };
    void deactivate() { m_is_active = false; };

    // ————— GETTERS ————— //
    EntityType const get_entity_type()    const { return m_entity_type;     };
    AIType     const get_ai_type()        const { return m_ai_type;         };
    AIState    const get_ai_state()       const { return m_ai_state;        };
    glm::vec3  const get_position()       const { return m_position;        };
    glm::vec3  const get_movement()       const { return m_movement;        };
    glm::vec3  const get_velocity()       const { return m_velocity;        };
    glm::vec3  const get_acceleration()   const { return m_acceleration;    };
    float      const get_jumping_power()  const { return m_jumping_power;   };
    float      const get_speed()          const { return m_speed;           };
    int        const get_width()          const { return m_width;           };
    int        const get_height()         const { return m_height;          };

    // ————— SETTERS ————— //
    void const set_entity_type(EntityType new_entity_type)  { m_entity_type = new_entity_type;      };
    void const set_ai_type(AIType new_ai_type)              { m_ai_type = new_ai_type;              };
    void const set_ai_state(AIState new_state)              { m_ai_state = new_state;               };
    void const set_position(glm::vec3 new_position)         { m_position = new_position;            };
    void const set_movement(glm::vec3 new_movement)         { m_movement = new_movement;            };
    void const set_velocity(glm::vec3 new_velocity)         { m_velocity = new_velocity;            };
    void const set_speed(float new_speed)                   { m_speed = new_speed;                  };
    void const set_jumping_power(float new_jumping_power)   { m_jumping_power = new_jumping_power;  };
    void const set_acceleration(glm::vec3 new_acceleration) { m_acceleration = new_acceleration;    };
    void const set_width(float new_width)                   { m_width = new_width;                  };
    void const set_height(float new_height)                 { m_height = new_height;                };
    void const set_scale(glm::vec3 new_scale)               { m_model_matrix = glm::scale(m_model_matrix, new_scale);                };
};
