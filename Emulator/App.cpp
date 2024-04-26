#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <SDL3/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

#include <iostream>
#include <memory>

#include "ImGuiDataContext.h"

using namespace std;

unordered_map<char, char> shifted_key_map = {
    {'`', '~'},
    {'1', '!'},
    {'2', '@'},
	{'3', '#'},
	{'4', '$'},
	{'5', '%'},
	{'6', '^'},
	{'7', '&'},
	{'8', '*'},
	{'9', '('},
	{'0', ')'},
	{'-', '_'},
	{'=', '+'},
	{'[', '{'},
	{']', '}'},
	{'\\', '|'},
	{';', ':'},
	{'\'', '"'},
	{',', '<'},
	{'.', '>'},
	{'/', '?'}
};

enum CoreOperation
{
	UP = 128, DOWN, LEFT, RIGHT, START, STEP, STOP, RESET
};

unsigned char calculate_character(const SDL_Event &event)
{
    static bool shift_pressed;
    static bool ctrl_pressed;
    static bool alt_pressed;
    
    shift_pressed = (SDL_GetModState() & SDL_KMOD_SHIFT) != 0;
    ctrl_pressed = (SDL_GetModState() & SDL_KMOD_CTRL) != 0;
    alt_pressed = (SDL_GetModState() & SDL_KMOD_ALT) != 0;


    const auto test_keycode = event.key.keysym.sym;
    
    if (test_keycode <= 127)
    {
        if (shift_pressed)
        {
            if (test_keycode >= 'a' && test_keycode <= 'z')
                return static_cast<unsigned char>(toupper(test_keycode));

            if (test_keycode >= '!' && test_keycode <= '@' ||
                test_keycode >= '[' && test_keycode <= '`' ||
                test_keycode >= '{' && test_keycode <= '~')
                return shifted_key_map[test_keycode];
        }
        if (ctrl_pressed)
        {
            if (test_keycode == 'k')
                return START;
            if (test_keycode == 'l')
                return STEP;
            if (test_keycode == 'j')
                return STOP;
            if (test_keycode == 'r')
                return RESET;
        }
        if (test_keycode == '\r')
            return  '\n';

    	return test_keycode;
    }

    switch (event.key.keysym.scancode)
    {
    case SDL_SCANCODE_UP:
        return UP;
    case SDL_SCANCODE_DOWN:
        return DOWN;
    case SDL_SCANCODE_LEFT:
        return LEFT;
    case SDL_SCANCODE_RIGHT:
        return RIGHT;
    default:
        return 255;
    }
}
// Main code
int main()
{
	constexpr size_t window_width = 1280;
	constexpr size_t window_height = 720;

    constexpr int time_per_clock = 0;
    constexpr size_t emulator_screen_width = 320;
    constexpr size_t emulator_screen_height = 240;

    auto core = make_shared<RV32IM::Core>(time_per_clock, emulator_screen_width, emulator_screen_height);
   
	// Setup SDL
    Uint32 SDL_flags = SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMEPAD;
	Uint32 window_flags = SDL_WINDOW_OPENGL | /*SDL_WINDOW_RESIZABLE |*/ SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    Uint32 renderer_flags = SDL_RENDERER_PRESENTVSYNC;
    ImGuiConfigFlags io_config_flags = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad;

    const auto app_context = make_unique<ImGuiDataContext>("RISC-V 32-bit Emulator", SDL_flags, window_flags, renderer_flags, io_config_flags, window_width, window_height, core);

    // Main loop
    bool done = false;
    core->start_clock();

    while (!done)
    {

        // when a key is pressed, if console is in focus, forward it there.
        // when a key is pressed, if emulator is in focus, forward it there. 
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                done = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == app_context->get_window_id())
                done = true;
            if (event.type == SDL_EVENT_KEY_DOWN)
            {
	            const auto input_char = calculate_character(event);
                switch (input_char)
                {
                case START:
                    if (!core->is_clock_running())
                        core->start_clock();
                    break;
                case STEP:
                    if (!core->is_clock_running())
                        core->step_clock();
                    break;
                case STOP:
                    if (core->is_clock_running())
                        core->stop_clock();
                    break;
                case RESET:
                    app_context->reset_to_file();
                    break;
                default:
                    break;
                }
                if (input_char <= RIGHT)
                {
                    switch (app_context->
                        get_current_window())
                    {
                    case CurrentWindow::Emulator:
                        core->notify_keypress(input_char);
                        break;
                    case CurrentWindow::Console:
                        core->notify_uart_keypress(input_char);
                        break;
                    default:
                        break;
                    }
                }
            }
            //if (event.type == SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED)
            //    app_context->UpdateDPI();
        }
        app_context->new_frame();
        app_context->update();
        app_context->render();        
    }

    return EXIT_SUCCESS;
}
