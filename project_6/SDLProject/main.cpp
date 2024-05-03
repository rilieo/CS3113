#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_LEFT_EDGE 5.0f

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
Mix_Music *g_bgm;
Mix_Chunk *g_bark_sfx;
Effects *g_effects;
Entity* curr_player;
Entity* message;

int g_num_lives = 3;
GLuint g_font_texture_id,
       g_lose_texture_id,
       g_win1_texture_id;
        
SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

void switch_to_scene(Scene *scene, Entity* player)
{
    g_current_scene = scene;
    g_current_scene->initialise(player);
}

Entity* create_player() {
    Entity* player = new Entity();
    player->set_entity_type(PLAYER);
    player->set_position(glm::vec3(0.0f, 0.0f, 0.0f));
    player->set_movement(glm::vec3(0.0f));
    player->set_speed(2.0f);
    player->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    player->m_texture_id = Utility::load_texture("assets/images/dog_spritesheet.png");
    
    // Walking
    player->m_animations[player->IDLE_RIGHT] = new int[6] {20, 21, 22, 23, 23, 23};
    
    // Attacking
    player->m_animations[player->ATTACK_RIGHT]  = new int[6] {47, 47, 47, 47, 47, 47};
    player->m_animations[player->ATTACK_LEFT]   = new int[6] {35, 35, 35, 35, 35, 35};
    
    // Hit
    player->m_animations[player->HURT_LEFT]  = new int[6] {0, 0, 0, 0, 0, 0};
    player->m_animations[player->HURT_RIGHT]   = new int[6] {11, 11, 11, 11, 11, 11};
    
    player->m_animation_indices = player->m_animations[player->IDLE_RIGHT];
    player->m_animation_frames = 6;
    player->m_animation_index  = 0;
    player->m_animation_time   = 0.0f;
    player->m_animation_cols   = 10;
    player->m_animation_rows   = 5;
    player->set_height(0.8f);
    player->set_width(0.8f);
    
    // WEAPON
    player->weapon = new Entity();
    player->weapon->set_entity_type(WEAPON);
    player->weapon->set_movement(glm::vec3(0.0f));
    player->weapon->set_speed(1.5f);
    player->weapon->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    player->weapon->m_texture_id = Utility::load_texture("assets/images/stick_spritesheet.png");
    
    player->weapon->m_animations[player->weapon->ATTACK_RIGHT] = new int[4] {0, 0, 0, 0};
    player->weapon->m_animation_indices = player->m_animations[player->ATTACK_RIGHT];
    player->weapon->m_animation_frames = 4;
    player->weapon->m_animation_index  = 0;
    player->weapon->m_animation_time   = 0.0f;
    player->weapon->m_animation_cols   = 2;
    player->weapon->m_animation_rows   = 2;
    player->weapon->set_height(0.8f);
    player->weapon->set_width(0.5f);
    
    return player;
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
    g_lose_texture_id = Utility::load_texture("assets/images/lose/lose.png");
    g_win1_texture_id = Utility::load_texture("assets/images/win/win_1.png");
    
    // ————— Player SETUP ————— //
    curr_player = create_player();
    
    // ————— LEVEL SETUP ————— //
    g_menu_screen = new MenuScreen();
    g_level_a = new LevelA();
    g_level_b = new LevelB();
    g_level_c = new LevelC();
    switch_to_scene(g_menu_screen, curr_player);
    
    /* BGM and SFX */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    g_bgm = Mix_LoadMUS("assets/audio/k-k-slider.mp3");
    Mix_PlayMusic(g_bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 8.0f);
    
    g_bark_sfx = Mix_LoadWAV("assets/audio/single-husky-bark.wav");
    
    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    curr_player->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
    
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
                    case SDLK_f: {
                        if (curr_player->get_position().y < -1.67f) {
                            curr_player->m_is_planted = true;
                        }
                        break;
                    }
                    case SDLK_v: {
                        Entity* new_player = create_player();
                        g_current_scene->m_state.players.push_back(new_player);
                        curr_player = new_player;
                        break;
                    }    
                    case SDLK_RETURN: {
                        if (g_current_scene == g_menu_screen) {
                            switch_to_scene(g_level_a, curr_player);
                        } else {
                            if (g_current_scene->m_state.next_dialogue < g_current_scene->m_state.dialogue_count - 1)
                                Mix_PlayChannel(-1, g_bark_sfx, 0);
                            g_current_scene->m_state.next_dialogue += 1;
                        }
                        break;
                    }
            default:
                break;
            }

        default:
            break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);
    
    if (key_state[SDL_SCANCODE_LEFT] || key_state[SDL_SCANCODE_A])
    {
        curr_player->move_left();
    }
    else if (key_state[SDL_SCANCODE_RIGHT]|| key_state[SDL_SCANCODE_D])
    {
        curr_player->move_right();
    }
                                                                            
    if (key_state[SDL_SCANCODE_UP]|| key_state[SDL_SCANCODE_W])
    {
        curr_player->move_up();
    }
    else if (key_state[SDL_SCANCODE_DOWN]|| key_state[SDL_SCANCODE_S])
    {
        curr_player->move_down();
    }
                                                                            
    // This makes sure that the player can't "cheat" their way into moving
    // faster
    if (glm::length(curr_player->get_movement()) > 1.0f)
    {
        curr_player->set_movement(glm::normalize(curr_player->get_movement()));
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
        
//        if (g_current_scene != g_menu_screen) g_effects->update(FIXED_TIMESTEP);
        g_current_scene->update(FIXED_TIMESTEP);
        
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;
    
    // ————— PLAYER CAMERA ————— //
    g_view_matrix = glm::mat4(1.0f);
    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-4.5f, 3.75f, 0.0f));

}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    // ————— RENDERING THE SCENE (i.e. map, character, enemies...) ————— //
//    g_effects->render();
    g_current_scene->render(&g_shader_program);
    
    if (g_current_scene != g_menu_screen && (g_current_scene->m_number_of_killed_players == g_current_scene->m_state.players.size() || g_current_scene->m_enemy_crossed)) {
        // switch to lose scene
        glm::mat4 model_matrix = glm::mat4(1.0f);
        Utility::draw_object(&g_shader_program, model_matrix, g_lose_texture_id, glm::vec3(10.0f, 8.0f, 0.0f), glm::vec3(4.5f, -3.5f, 0.0f));
        g_current_scene->reset();
    }
    
    
    if (g_current_scene != g_menu_screen && g_current_scene->m_number_of_killed_enemies == g_current_scene->m_number_of_enemies) {
//         switch to next scene
        curr_player = create_player();
        if (g_current_scene == g_level_a) {
            switch_to_scene(g_level_b, curr_player);
        } else if (g_current_scene == g_level_b) {
            switch_to_scene(g_level_c, curr_player);
        }
        else if (g_current_scene == g_level_c) {
            glm::mat4 win_model_matrix = glm::mat4(1.0f);
            Utility::draw_object(&g_shader_program, win_model_matrix, g_win1_texture_id, glm::vec3(10.0f, 8.0f, 0.0f), glm::vec3(4.5f, -3.5f, 0.0f));
            g_current_scene->reset();
        }
        
    }

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();
    
    // ————— DELETING LEVEL DATA (i.e. map, character, enemies...) ————— //
    delete g_level_a;
    delete g_level_b;
    delete g_menu_screen;
//    delete g_effects;
//    Mix_FreeChunk(g_bark_sfx);
//    Mix_FreeMusic(g_bgm);
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
