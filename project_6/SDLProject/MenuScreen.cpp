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

void MenuScreen::reset() {}

void MenuScreen::initialise(Entity* player)
{
    m_menu.model_matrix = glm::mat4(1.0f);
    m_menu.texture_id = Utility::load_texture("assets/images/Dogs vs Bees.png");
    
}
void MenuScreen::update(float delta_time) {
    m_menu.model_matrix = glm::mat4(1.0f);
    m_menu.model_matrix = glm::translate(m_menu.model_matrix, glm::vec3(4.5f, -3.5f, 0.0f));
    m_menu.model_matrix = glm::scale(m_menu.model_matrix, glm::vec3(10.0f, 8.0f, 0.0f));
}

void draw_object(ShaderProgram *program, glm::mat4 &object_model_matrix, GLuint &object_texture_id) {
    program->set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}

void MenuScreen::render(ShaderProgram *program)
{
    program->set_model_matrix(m_menu.model_matrix);

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D,  m_menu.texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    draw_object(program, m_menu.model_matrix, m_menu.texture_id);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

