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

Entity* MenuScreen::create_enemy() { return NULL; }

void MenuScreen::initialise(Entity* player)
{
    GLuint id = Utility::load_texture("assets/images/first.png");
    texture_id = id;
}

void MenuScreen::update(float delta_time) {}


void MenuScreen::render(ShaderProgram *program)
{
    Utility::render(program, texture_id);
    
}
