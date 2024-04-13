#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_LEFT_EDGE 5.0f
#define LEVEL1_RIGHT_EDGE 16.0f
#define LEVEL2_RIGHT_EDGE 10.0f
#define LEVEL2_LEFT_EDGE 7.0f
#define LEVEL_BOTTOM_EDGE -7.2f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Utility.h"
#include "Scene.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "MenuScreen.h"
#include "Effects.h"

// ————— CONSTANTS ————— //
const int WINDOW_WIDTH  = 640,
          WINDOW_HEIGHT = 480;

const float BG_RED     = 0.1922f,
            BG_BLUE    = 0.549f,
            BG_GREEN   = 0.9059f,
            BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const float MILLISECONDS_IN_SECOND = 1000.0;

// ————— GLOBAL VARIABLES ————— //
Scene *g_current_scene;
MenuScreen *g_menu_screen;
LevelA *g_level_a;
LevelB *g_level_b;
LevelC *g_level_c;
Entity *g_player;
Mix_Music *g_bgm;
Mix_Chunk *g_bark_sfx;
Effects *g_effects;

int g_num_lives = 3;
GLuint g_font_texture_id,
       g_bg_texture_id;
        

SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

bool g_pressed = false;

void switch_to_scene(Scene *scene)
{
    g_current_scene = scene;
    g_current_scene->initialise(g_player);
}

void initialise()
{
    // ————— VIDEO ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("WOo",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    // ————— GENERAL ————— //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    g_font_texture_id = Utility::load_texture("assets/fonts/font1.png");
//    g_bg_texture_id = Utility::load_texture("assets/swamp-tileset/2 Background/Background.png");
    
    g_effects = new Effects(g_projection_matrix, g_view_matrix);
    g_effects->start(FADEIN, 1.0f);
    
    // ————— Player SETUP ————— //
    g_player = new Entity();
    g_player->set_entity_type(PLAYER);
    g_player->set_position(glm::vec3(0.0f, 0.0f, 0.0f));
    g_player->set_movement(glm::vec3(0.0f));
    g_player->set_speed(2.5f);
    g_player->set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    g_player->m_texture_id = Utility::load_texture("assets/images/dog_spritesheet.png");
    
    // Walking
    g_player->m_animations[g_player->WALK_LEFT]  = new int[6] {24, 25, 26, 27, 28, 29};
    g_player->m_animations[g_player->WALK_RIGHT] = new int[6] {14, 15, 16, 17, 18, 19};
    
    // Attacking
    g_player->m_animations[g_player->ATTACK_RIGHT]  = new int[6] {47, 47, 47, 47, 47, 47};
    g_player->m_animations[g_player->ATTACK_LEFT]   = new int[6] {35, 35, 35, 35, 35, 35};
    
    // Hit
    g_player->m_animations[g_player->HURT_LEFT]  = new int[6] {0, 0, 0, 0, 0, 0};
    g_player->m_animations[g_player->HURT_RIGHT]   = new int[6] {11, 11, 11, 11, 11, 11};
    
    g_player->m_animation_indices = g_player->m_animations[g_player->WALK_RIGHT];
    g_player->m_animation_frames = 6;
    g_player->m_animation_index  = 0;
    g_player->m_animation_time   = 0.0f;
    g_player->m_animation_cols   = 10;
    g_player->m_animation_rows   = 5;
    g_player->set_height(0.8f);
    g_player->set_width(0.8f);
    
    // Jumping
    g_player->m_jumping_power = 5.0f;
    
    // ————— LEVEL SETUP ————— //
    g_menu_screen = new MenuScreen();
    g_level_a = new LevelA();
    g_level_b = new LevelB();
    g_level_c = new LevelC();
    switch_to_scene(g_menu_screen);
    
    /* BGM and SFX */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    g_bgm = Mix_LoadMUS("assets/audio/k-k-slider.mp3");
    Mix_PlayMusic(g_bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 16.0f);
    
    g_bark_sfx = Mix_LoadWAV("assets/audio/single-husky-bark.wav");
    
    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    if (g_current_scene != g_menu_screen)
        g_current_scene->m_state.player->set_movement(glm::vec3(0.0f));
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_game_is_running  = false;
            break;

        case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                        // Quit the game with a keystroke
                        g_game_is_running  = false;
                        break;
                        
                    case SDLK_SPACE:
                        // Jump
                        if (!g_current_scene->m_state.is_frozen && g_current_scene != g_menu_screen && g_current_scene->m_state.player->m_collided_bottom)
                            g_current_scene->m_state.player->m_is_jumping = true;
                        break;
                    case SDLK_e: {
                        if (!g_current_scene->m_state.is_frozen) {
                            Mix_PlayChannel(-1, g_bark_sfx, 0);
                            if (g_current_scene != g_menu_screen &&
                                g_current_scene->m_state.player->m_animation_indices == g_current_scene->m_state.player->m_animations[g_current_scene->m_state.player->WALK_LEFT]) {
                                g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_animations[g_current_scene->m_state.player->ATTACK_LEFT];
                            }
                            else if (g_current_scene != g_menu_screen &&
                                     g_current_scene->m_state.player->m_animation_indices == g_current_scene->m_state.player->m_animations[g_current_scene->m_state.player->WALK_RIGHT]) {
                                g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_animations[g_current_scene->m_state.player->ATTACK_RIGHT];
                            }
                        }
                        break;
                    }
                    case SDLK_RETURN: {
                        if (g_current_scene == g_menu_screen)
                            switch_to_scene(g_level_a);
                        break;
                    }
                    case SDLK_f: {
                        if (!g_pressed && g_current_scene != g_menu_screen) {
                            g_current_scene->m_state.is_frozen = true;
                            g_pressed = true;
                        } else if (g_pressed && g_current_scene != g_menu_screen) {
                            g_current_scene->m_state.is_frozen = false;
                            g_pressed = false;
                        }
                    }
            default:
                break;
            }

        default:
            break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);
    
    if (!g_current_scene->m_state.is_frozen) {
        if (g_current_scene != g_menu_screen) {
            if (key_state[SDL_SCANCODE_LEFT])
            {
                g_current_scene->m_state.player->move_left();
                g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_animations[g_current_scene->m_state.player->WALK_LEFT];
            }
            else if (key_state[SDL_SCANCODE_RIGHT])
            {
                g_current_scene->m_state.player->move_right();
                g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_animations[g_current_scene->m_state.player->WALK_RIGHT];
            }
            else if (key_state[SDL_SCANCODE_DOWN])
            {
                g_current_scene->m_state.player->move_down();
            }
            else if (key_state[SDL_SCANCODE_UP])
            {
                g_current_scene->m_state.player->move_up();
            }
            
            // This makes sure that the player can't move faster diagonally
            if (glm::length(g_current_scene->m_state.player->get_movement()) > 1.0f)
            {
                g_current_scene->m_state.player->set_movement(glm::normalize(g_current_scene->m_state.player->get_movement()));
            }
        }
    }
}

void update()
{
    // ————— DELTA TIME / FIXED TIME STEP CALCULATION ————— //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_accumulator;
    
    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    
    while (delta_time >= FIXED_TIMESTEP) {
        // ————— UPDATING THE SCENE (i.e. map, character, enemies...) ————— //
        
        if (g_current_scene != g_menu_screen) g_effects->update(FIXED_TIMESTEP);
        
        g_current_scene->update(FIXED_TIMESTEP);
        
        if (g_current_scene != g_menu_screen && g_current_scene->m_state.player->m_hit) {
            g_num_lives--;
            g_current_scene->m_state.player->m_hit = false;
        }
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;
    
    g_player->m_countdown -= delta_time;
    
    // ————— PLAYER CAMERA ————— //
    g_view_matrix = glm::mat4(1.0f);
    
    if (g_current_scene != g_menu_screen && g_current_scene->m_state.player->get_position().x > LEVEL1_LEFT_EDGE) {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->m_state.player->get_position().x, 3.75, 0));
    } else {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
    }
    
    if (g_current_scene == g_level_a && g_current_scene->m_state.player->get_position().x > LEVEL1_RIGHT_EDGE && g_current_scene->m_state.player->get_position().y < LEVEL_BOTTOM_EDGE) {
        g_effects->start(FADEOUT, 1.0f);
        switch_to_scene(g_level_b);
        g_effects->start(FADEIN, 1.85f);
    }
    
    if (g_current_scene == g_level_b &&
        g_current_scene->m_state.player->get_position().x > LEVEL2_LEFT_EDGE && g_current_scene->m_state.player->get_position().x < LEVEL2_RIGHT_EDGE && g_current_scene->m_state.player->get_position().y < LEVEL_BOTTOM_EDGE){
        g_effects->start(FADEOUT, 1.0f);
        switch_to_scene(g_level_c);
        g_effects->start(FADEIN, 1.85f);
    }
    
    if (g_current_scene->m_state.is_frozen) Mix_PauseMusic();
    else if (!g_current_scene->m_state.is_frozen) Mix_ResumeMusic();
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    // ————— RENDERING THE SCENE (i.e. map, character, enemies...) ————— //
    g_effects->render();
    g_current_scene->render(&g_shader_program);
    
    if (g_num_lives <= 0 || (g_current_scene != g_menu_screen && g_current_scene->m_state.player->get_position().y < LEVEL_BOTTOM_EDGE)) {
        float x_position = g_current_scene->m_state.player->get_position().x;
        Utility::draw_text(&g_shader_program, g_font_texture_id, "You lose!", 1.5f, -0.7f, glm::vec3(x_position - 3.0f, -3.5f, 0.0f));
    }
    else if (g_num_lives > 0 && g_current_scene == g_level_c && g_current_scene->m_state.player->m_got_flag) {
        float x_position = g_current_scene->m_state.player->get_position().x;
        Utility::draw_text(&g_shader_program, g_font_texture_id, "You win!", 1.5f, -0.7f, glm::vec3(x_position - 3.0f, -3.5f, 0.0f));
    }
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();
    
    // ————— DELETING LEVEL A DATA (i.e. map, character, enemies...) ————— //
    delete g_level_a;
    delete g_level_b;
    delete g_level_c;
    delete g_menu_screen;
    delete g_effects;
    Mix_FreeChunk(g_bark_sfx);
    Mix_FreeMusic(g_bgm);
}

// ————— GAME LOOP ————— //
int main(int argc, char* argv[])
{
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
