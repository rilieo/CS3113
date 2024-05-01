/*
* Author: Riley Dou
* Assignment: Platformer
* Date due: 2024-04-13, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
*/

#include "MenuScreen.h"
#include "Utility.h"

MenuScreen::~MenuScreen() {}

Entity* MenuScreen::create_enemy(int y) { return NULL; }

void MenuScreen::initialise(Entity* player)
{
    GLuint font_texture_id = Utility::load_texture("assets/fonts/font1.png");
    texture_id = font_texture_id;
}

void MenuScreen::update(float delta_time) {}


void MenuScreen::render(ShaderProgram *program)
{
    Utility::draw_text(program, texture_id, "Press Enter to Start", 0.8, -0.35, glm::vec3(0.7f, -3.8f, 0.0f));
    
}
