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
                core->notify_keypress('A');
            //if (event.type == SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED)
            //    app_context->UpdateDPI();
        }
        app_context->new_frame();
        app_context->update();
        app_context->render();        
    }

    return EXIT_SUCCESS;
}
