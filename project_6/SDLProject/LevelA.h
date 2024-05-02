#include "Scene.h"
#include <vector>

class LevelA : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    int ENEMY_COUNT = 5;
    
    // ————— CONSTRUCTOR ————— //
    ~LevelA();
    
    // ————— METHODS ————— //
//    void initialise(Entity* player) override;
    void initialise(Entity* player) override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
    Entity* create_enemy() override;
    
    void deactivate(std::vector<Entity*> enemies, std::vector<Entity*> players, int index, EntityType entity_type);
};
