#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#include <SDL3/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

#include <memory>

#include "ImGuiDataContext.h"


using namespace std;
// Main code
int main()
{
    // Setup SDL
    Uint32 SDL_flags = SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMEPAD;
	Uint32 window_flags = SDL_WINDOW_OPENGL | /*SDL_WINDOW_RESIZABLE |*/ SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    Uint32 renderer_flags = SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED;
    ImGuiConfigFlags io_config_flags = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad;
    int width = 1280;
    int height = 720;

    const auto app_context = make_unique<ImGuiDataContext>("RISC-V 32-bit Emulator", SDL_flags, window_flags, renderer_flags, io_config_flags, width, height);


    // Main loop
    bool done = false;

    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                done = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == app_context->GetWindowID())
                done = true;
            //if (event.type == SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED)
            //    app_context->UpdateDPI();
        }

        app_context->NewFrame();
        app_context->Update();
        app_context->Render();        
    }

    return EXIT_SUCCESS;
}