#include "Scene.h"

class MenuScreen : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    int texture_id;
    
    // ————— CONSTRUCTOR ————— //
    ~MenuScreen();
    
    // ————— METHODS ————— //
    void initialise(Entity* player) override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};
