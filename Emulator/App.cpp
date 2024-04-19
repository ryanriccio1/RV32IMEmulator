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

unordered_map<char, char> shifted_num_row = {
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

unsigned char calculate_character(const SDL_Event &event)
{
    static bool shift_pressed;
    static bool ctrl_pressed;
    static bool alt_pressed;

    shift_pressed = (event.key.keysym.mod & SDL_KMOD_SHIFT) != 0;
    ctrl_pressed = (event.key.keysym.mod & SDL_KMOD_CTRL) != 0;
    alt_pressed = (event.key.keysym.mod & SDL_KMOD_ALT) != 0;


    const auto test_keycode = static_cast<unsigned char>(SDL_SCANCODE_TO_KEYCODE(event.key.keysym.scancode));
    if (test_keycode <= 127)
    {
        if (shift_pressed)
        {
            if (test_keycode >= 'a' && test_keycode <= 'z')
                return static_cast<unsigned char>(toupper(test_keycode));

            if (test_keycode >= '!' && test_keycode <= '@' ||
                test_keycode >= '[' && test_keycode <= '`' ||
                test_keycode >= '{' && test_keycode <= '~')
                return shifted_num_row[test_keycode];
        }
    	return test_keycode;
    }
    return 255;
}
// Main code
int main()
{
	constexpr size_t window_width = 1280;
	constexpr size_t window_height = 720;

    constexpr size_t memory_size = 0x100000;
    constexpr int time_per_clock = 0;
    constexpr size_t emulator_screen_width = 320;
    constexpr size_t emulator_screen_height = 240;

    auto core = make_shared<RV32IM::Core>(memory_size, time_per_clock, emulator_screen_width, emulator_screen_height);
   
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
                if (input_char != 255)
                {
                    switch (app_context->get_current_window())
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
