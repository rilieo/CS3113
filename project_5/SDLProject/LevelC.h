#include "Scene.h"

class LevelC : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    int ENEMY_COUNT = 1;
    
    // ————— CONSTRUCTOR ————— //
    ~LevelC();
    
    // ————— METHODS ————— //
    void initialise(Entity* player) override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};
