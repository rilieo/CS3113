#include "MenuScreen.h"
#include "Utility.h"

#define LEVEL_WIDTH 16
#define LEVEL_HEIGHT 8
#define ENEMY_COUNT 3

MenuScreen::~MenuScreen() {}

void MenuScreen::initialise(Entity* player)
{
    GLuint font_texture_id = Utility::load_texture("assets/fonts/font1.png");
    texture_id = font_texture_id;
}

void MenuScreen::update(float delta_time) {}


void MenuScreen::render(ShaderProgram *program)
{
    Utility::draw_text(program, texture_id, "Press Enter to Start", 0.8, -0.35, glm::vec3(0.7f, -4.0f, 0.0f));
    
}
