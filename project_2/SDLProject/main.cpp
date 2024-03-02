/*
* Author: Riley Dou
* Assignment: Pong Clone
* Date due: 2024-03-02, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
*/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"

#define LOG(argument) std::cout << argument << '\n'

const int WINDOW_WIDTH = 640,
          WINDOW_HEIGHT = 480;

const float BG_RED = 0.1922f,
            BG_BLUE = 0.549f,
            BG_GREEN = 0.9059f,
            BG_OPACITY = 0.75f;

const float MILLISECONDS_IN_SECOND = 1000.0;
const float MINIMUM_COLLISION_DISTANCE = 0.85f;

const int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl",
           LEFT_TOTORO_FILEPATH[] = "left-totoro.png",
           RIGHT_TOTORO_FILEPATH[] = "right-totoro.png",
           BALL1_FILEPATH[] = "soot.png",
           BALL2_FILEPATH[] = "soot.png",
           BALL3_FILEPATH[] = "soot.png",
           RIGHT_WIN_MSG_FILEPATH[] = "right-win.png",
           LEFT_WIN_MSG_FILEPATH[] = "left-win.png";

const int NUMBER_OF_TEXTURES = 1; // to be generated, that is
const GLint LEVEL_OF_DETAIL = 0;  // base image level; Level n is the nth mipmap reduction image
const GLint TEXTURE_BORDER = 0;   // this value MUST be zero

SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_shader_program;
glm::mat4   g_view_matrix,
            g_right_totoro_model_matrix,
            g_left_totoro_model_matrix,
            g_ball1_model_matrix,
            g_ball2_model_matrix,
            g_ball3_model_matrix,
            g_left_win_msg_model_matrix,
            g_right_win_msg_model_matrix,
            g_projection_matrix,
            g_trans_matrix;

GLuint  g_right_totoro_texture_id,
        g_left_totoro_texture_id,
        g_ball1_texture_id,
        g_ball2_texture_id,
        g_ball3_texture_id,
        g_left_win_msg_texture_id,
        g_right_win_msg_texture_id;

bool g_right_totoro_turn1,
     g_right_totoro_turn2 = false,
     g_right_totoro_turn3 = true;

// Req 1: Two totoros on both sides
glm::vec3 g_right_totoro_position = glm::vec3(4.0f, 0.0f, 0.0f);
glm::vec3 g_right_totoro_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_left_totoro_position = glm::vec3(-4.0f, 0.0f, 0.0f);
glm::vec3 g_left_totoro_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_ball1_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball2_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball3_position = glm::vec3(1.0f, 0.0f, 0.0f);

glm::vec3 g_ball1_movement = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball2_movement = glm::vec3(-1.0f, -1.0f, 0.0f);
glm::vec3 g_ball3_movement = glm::vec3(1.0f, 1.0f, 0.0f);

const glm::vec3 scale = glm::vec3(2.0f, 2.0f, 1.0f);
float g_speed = 2.0f;

bool g_is_bot = false;
glm::vec3 g_right_bot_movement = glm::vec3(0.0f, 1.0f, 0.0f);

float g_previous_ticks = 0.0f;

bool g_right_totoro_win = false,
     g_left_totoro_win = false;

int g_number_of_balls = 1;

GLuint load_texture(const char* filepath) {
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);
    
    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        LOG(filepath);
        assert(false);
    }
    
    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);
    
    return textureID;
}

void initialise() {
    // Initialise video
    SDL_Init(SDL_INIT_VIDEO);
    
    g_display_window = SDL_CreateWindow("Pong",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix = glm::mat4(1.0f);  // Defines the position (location and orientation) of the camera
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);  // Defines the characteristics of your camera, such as clip planes, field of view, projection method etc.
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    // Load textures
    // EC: From My Neighbor Totoro
    g_left_totoro_texture_id = load_texture(LEFT_TOTORO_FILEPATH);
    g_right_totoro_texture_id = load_texture(RIGHT_TOTORO_FILEPATH);
    g_ball1_texture_id = load_texture(BALL1_FILEPATH);
    g_ball2_texture_id = load_texture(BALL2_FILEPATH);
    g_ball3_texture_id = load_texture(BALL3_FILEPATH);
    g_left_win_msg_texture_id = load_texture(LEFT_WIN_MSG_FILEPATH);
    g_right_win_msg_texture_id = load_texture(RIGHT_WIN_MSG_FILEPATH);
    
    // Set model matrices
    g_left_totoro_model_matrix = glm::mat4(1.0f);
    g_right_totoro_model_matrix = glm::mat4(1.0f);
    g_ball1_model_matrix = glm::mat4(1.0f);
    g_ball2_model_matrix = glm::mat4(1.0f);
    g_ball3_model_matrix = glm::mat4(1.0f);
    g_left_win_msg_model_matrix = glm::mat4(1.0f);
    g_right_win_msg_model_matrix = glm::mat4(1.0f);
    
    float x, y;
    
    // Set player who plays first as random
    g_right_totoro_turn1 = (rand() % 100 > 50) ? true : false;
    
    // Set random movement
    x = (g_right_totoro_turn1) ? 1.0f : -1.0f;
    y = (rand() % 100 > 50) ? 1.0f : -1.0f;
    g_ball1_movement = glm::vec3(x, y, 0.0f);
    
    g_left_win_msg_model_matrix = glm::scale(g_left_totoro_model_matrix, glm::vec3(5.0f, 5.0f, 1.0f));
    g_right_win_msg_model_matrix = glm::scale(g_left_totoro_model_matrix, glm::vec3(5.0f, 5.0f, 1.0f));
    
    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_game_is_running = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {   // Req 1
                    case SDLK_UP:
                        g_right_totoro_movement.y = 1.0f;
                        break;
                    case SDLK_DOWN:
                        g_right_totoro_movement.y = -1.0f;
                        break;
                    case SDLK_w:
                        g_left_totoro_movement.y = 1.0f;
                        break;
                    case SDLK_s:
                        g_left_totoro_movement.y = -1.0f;
                        break;
                    case SDLK_t:
                        g_is_bot = true;
                        break;
                    case SDLK_1:
                        g_number_of_balls = 1;
                        break;
                    case SDLK_2:
                        g_number_of_balls = 2;
                        break;
                    case SDLK_3:
                        g_number_of_balls = 3;
                        break;
                    default:
                        break;
                }
                
            default:
                break;
        }
    }
    
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    
    if (key_state[SDL_SCANCODE_DOWN]) {
        g_right_totoro_movement.y = -1.0f;
    }
    else if (key_state[SDL_SCANCODE_UP]) {
        g_right_totoro_movement.y = 1.0f;
    }
    
    if (key_state[SDL_SCANCODE_S]) {
        g_left_totoro_movement.y = -1.0f;
    }
    else if (key_state[SDL_SCANCODE_W]) {
        g_left_totoro_movement.y = 1.0f;
    }
    
    if (glm::length(g_right_totoro_movement) > 1.0f) {
            g_right_totoro_movement = glm::normalize(g_right_totoro_movement);
        }
    if (glm::length(g_left_totoro_movement) > 1.0f) {
        g_left_totoro_movement = glm::normalize(g_left_totoro_movement);
    }
}

// Checks if ball collides with one of the totoros
bool did_collide(const glm::vec3& ball_position, bool turn) {
    float x_right_distance = fabs(g_right_totoro_position.x + 0.05f - ball_position.x) - (MINIMUM_COLLISION_DISTANCE + MINIMUM_COLLISION_DISTANCE) / 2.0f;
    float y_right_distance = fabs(g_right_totoro_position.y + 0.05f - ball_position.y) - (MINIMUM_COLLISION_DISTANCE + MINIMUM_COLLISION_DISTANCE) / 2.0f;
    float x_left_distance = fabs(g_left_totoro_position.x + 0.05f - ball_position.x) - (MINIMUM_COLLISION_DISTANCE + MINIMUM_COLLISION_DISTANCE) / 2.0f;
    float y_left_distance = fabs(g_left_totoro_position.y + 0.05f  - ball_position.y) - (MINIMUM_COLLISION_DISTANCE + MINIMUM_COLLISION_DISTANCE) / 2.0f;
    
    return (turn && x_right_distance < 0 && y_right_distance < 0) || (!turn && x_left_distance < 0 && y_left_distance < 0);
}

void update() {
    
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND; // get the current number of ticks
    float delta_time = ticks - g_previous_ticks; // the delta time is the difference from the last frame
    g_previous_ticks = ticks;
    
    // Req 3: If one of totoros win, stop the game
    if (g_right_totoro_win || g_left_totoro_win) {
        // Stop movement
        g_right_totoro_movement = glm::vec3(0.0f, 0.0f, 0.0f);
        g_left_totoro_movement = glm::vec3(0.0f, 0.0f, 0.0f);
        g_ball1_movement = glm::vec3(0.0f, 0.0f, 0.0f);
        g_ball2_movement = glm::vec3(0.0f, 0.0f, 0.0f);
        g_ball3_movement = glm::vec3(0.0f, 0.0f, 0.0f);
        
        // Return to original position
        g_right_totoro_position = glm::vec3(4.0f, 0.0f, 0.0f);
        g_left_totoro_position = glm::vec3(-4.0f, 0.0f, 0.0f);
    }
    
    g_left_totoro_position += g_left_totoro_movement * g_speed * delta_time;
    
    // Control ball(s) movement
    if (g_number_of_balls >= 1) {
        g_ball1_position += g_ball1_movement * delta_time;
    }
    if (g_number_of_balls >= 2) {
        g_ball2_position += g_ball2_movement * delta_time;
    }
    if (g_number_of_balls >= 3) {
        g_ball3_position += g_ball3_movement * delta_time;
    }
    
    // Control bot movement
    if (g_is_bot) {
        if (g_right_totoro_position.y >= 2.75f) {
            g_right_bot_movement.y = -1.0f;
        }
        else if (g_right_totoro_position.y <= -2.75f) {
            g_right_bot_movement.y = 1.0f;
        }
        
        g_right_totoro_position += g_right_bot_movement * g_speed * delta_time;
        
    } else {
        g_right_totoro_position += g_right_totoro_movement * g_speed * delta_time;
    }
    
    // Req 1: Make sure that totoros don't go off border
    if (g_right_totoro_position.y > 2.75f) {
        g_right_totoro_position.y = 2.75f;
    }
    else if (g_right_totoro_position.y < -2.75f) {
        g_right_totoro_position.y = -2.75f;
    }
    
    if (g_left_totoro_position.y > 2.75f) {
        g_left_totoro_position.y = 2.75f;
    }
    else if (g_left_totoro_position.y < -2.75f) {
        g_left_totoro_position.y = -2.75f;
    }
    
    // Reset model matrix
    g_left_totoro_model_matrix = glm::mat4(1.0f);
    g_right_totoro_model_matrix = glm::mat4(1.0f);
    g_ball1_model_matrix = glm::mat4(1.0f);
    g_ball2_model_matrix = glm::mat4(1.0f);
    g_ball3_model_matrix = glm::mat4(1.0f);

    // Req 1: Check if ball collides with totoros
    if (did_collide(g_ball1_position, g_right_totoro_turn1)){
        
        g_ball1_movement.x *= -1.0f;
        g_ball1_movement.y = (rand() % 100 > 50) ? -0.75f : 0.75f;
        
        g_right_totoro_turn1 = !g_right_totoro_turn1;
    }
    
    if(did_collide(g_ball2_position, g_right_totoro_turn2)) {
        g_ball2_movement.x *= -1.25f;
        g_ball2_movement.y = (rand() % 100 > 50) ? -0.5f : 0.5f;
        
        g_right_totoro_turn2 = !g_right_totoro_turn2;
    }
    
    
    if(did_collide(g_ball3_position, g_right_totoro_turn3)) {
        g_ball3_movement.x *= -1.25f;
        g_ball3_movement.y = (rand() % 100 > 50) ? -1.75f : 1.75f;
        
        g_right_totoro_turn3 = !g_right_totoro_turn3;
    }
    
    // Move totoros
    g_left_totoro_model_matrix = glm::translate(g_left_totoro_model_matrix, g_left_totoro_position);
    g_right_totoro_model_matrix = glm::translate(g_right_totoro_model_matrix, g_right_totoro_position);
    
    // Move ball(s)
    if (g_number_of_balls >= 1) {
        g_ball1_model_matrix = glm::translate(g_ball1_model_matrix, g_ball1_position);
    }
    if (g_number_of_balls >= 2) {
        g_ball2_model_matrix = glm::translate(g_ball2_model_matrix, g_ball2_position);
    }
    if (g_number_of_balls >= 3) {
        g_ball3_model_matrix = glm::translate(g_ball3_model_matrix, g_ball3_position);
    }
    
    // Make totoros bigger
    g_left_totoro_model_matrix = glm::scale(g_left_totoro_model_matrix, scale);
    g_right_totoro_model_matrix = glm::scale(g_right_totoro_model_matrix, scale);
    
    // Req 2: Make balls bounce off of border of window
    if (g_ball1_position.y > 3.45f) {
        g_ball1_position.y = 3.45f;
        g_ball1_movement.y *= -1.0f;
    } else if (g_ball1_position.y < -3.45f) {
        g_ball1_position.y = -3.45f;
        g_ball1_movement.y *= -1.0f;
    }
    
    if (g_ball2_position.y > 3.45f) {
        g_ball2_position.y = 3.45f;
        g_ball2_movement.y *= -1.0f;
    } else if (g_ball2_position.y < -3.45f) {
        g_ball2_position.y = -3.45f;
        g_ball2_movement.y *= -1.0f;
    }
    
    if (g_ball3_position.y > 3.45f) {
        g_ball3_position.y = 3.45f;
        g_ball3_movement.y *= -1.0f;
    } else if (g_ball3_position.y < -3.45f) {
        g_ball3_position.y = -3.45f;
        g_ball3_movement.y *= -1.0f;
    }
    
    // Set winners
    if (g_ball1_position.x < -5.5f || g_ball2_position.x < -5.5f || g_ball3_position.x < -5.5f) {
        g_right_totoro_win = true;
    } else if (g_ball1_position.x > 5.5f || g_ball2_position.x > 5.5f || g_ball3_position.x > 5.5f) {
        g_left_totoro_win = true;
    }
    
}


void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id) {
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Vertices
    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
    };

    // Textures
    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };
    
    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());
    
    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    // Bind texture
    draw_object(g_right_totoro_model_matrix, g_right_totoro_texture_id);
    draw_object(g_left_totoro_model_matrix, g_left_totoro_texture_id);
    
    if (g_number_of_balls >= 1) {
        draw_object(g_ball1_model_matrix, g_ball1_texture_id);
    }
    if (g_number_of_balls >= 2) {
        draw_object(g_ball2_model_matrix, g_ball2_texture_id);
    }
    if (g_number_of_balls >= 3) {
        draw_object(g_ball3_model_matrix, g_ball3_texture_id);
    }
    
    // EC: End game message
    if (g_left_totoro_win) {
        draw_object(g_left_win_msg_model_matrix, g_left_win_msg_texture_id);
    } else if (g_right_totoro_win) {
        draw_object(g_right_win_msg_model_matrix, g_right_win_msg_texture_id);
    }
    
    // We disable two attribute arrays now
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    initialise();
    
    while (g_game_is_running)
    {
        process_input();
        update();
        render();
    }
    
    shutdown();
    return 0;
}
