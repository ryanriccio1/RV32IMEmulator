#include "ImGuiDataContext.h"

using namespace std;
ImGuiDataContext::ImGuiDataContext(const string& window_name, Uint32 SDL_flags, Uint32 window_flags, Uint32 renderer_flags,
                                   ImGuiConfigFlags io_config_flags, int base_width, int base_height) :
	io{(ImGui::CreateContext(), ImGui::GetIO())},
	style{ImGui::GetStyle()},
	base_width{base_width},
	base_height{base_height}
{
    // Setup SDL
    if (SDL_Init(SDL_flags) != 0)
    {
        SDL_Log("Error: SDL_Init(): %s\n", SDL_GetError());
        throw SDLContextInitializationError();
    }

    // Enable native IME.
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

    // Create window with SDL_Renderer graphics context
    window = SDL_CreateWindow(window_name.c_str(), base_width, base_height, window_flags);
    if (window == nullptr)
    {
        SDL_Log("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        throw SDLContextInitializationError();
    }

    renderer = SDL_CreateRenderer(window, nullptr, renderer_flags);
    if (renderer == nullptr)
    {
        SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
        throw SDLContextInitializationError();
    }

    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
	io.ConfigFlags |= io_config_flags;
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    im_window_flags = 0;
    //im_window_flags |= ImGuiWindowFlags_NoTitleBar;
    im_window_flags |= ImGuiWindowFlags_NoMove;
    im_window_flags |= ImGuiWindowFlags_NoResize;
    im_window_flags |= ImGuiWindowFlags_NoCollapse;

    last_scale = 1.0f;

    UpdateDPI();
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
}

ImGuiDataContext::~ImGuiDataContext()
{
    // Cleanup
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void ImGuiDataContext::NewFrame()
{
    // Start the Dear ImGui frame
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void ImGuiDataContext::Render()
{
    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Rendering
    ImGui::Render();
    //SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
    SDL_RenderClear(renderer);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(renderer);
}

void ImGuiDataContext::UpdateDPI()
{
    dpi_scale = SDL_GetWindowDisplayScale(window);
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(ConsolaTTF_compressed_data_base85,13 * dpi_scale);
    style.ScaleAllSizes(sqrt(dpi_scale / last_scale));
    SDL_SetWindowSize(window, base_width * dpi_scale, base_height * dpi_scale);
    last_scale = dpi_scale;
}

SDL_WindowID ImGuiDataContext::GetWindowID() const
{
    return SDL_GetWindowID(window);
}

void ImGuiDataContext::SetWindowSize(const float& width, const float& height, const int pos_x, const int pos_y) const
{
    const auto windowSize = ImVec2(base_width * (width / 100) * dpi_scale, base_height * (height / 100) * dpi_scale);

    ImGui::SetNextWindowSize(windowSize);
    ImGui::SetNextWindowPos(ImVec2(pos_x / 100.0 * dpi_scale * base_width,pos_y / 100.0 * dpi_scale * base_height + ImGui::GetFrameHeight()));
}

void ImGuiDataContext::Update()
{
	ImGui::BeginMainMenuBar();

    if (ImGui::BeginMenu("File"))
    {
        ShowExampleMenuFile();
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Edit"))
    {
        if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
        if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
        ImGui::Separator();
        if (ImGui::MenuItem("Cut", "CTRL+X")) {}
        if (ImGui::MenuItem("Copy", "CTRL+C")) {}
        if (ImGui::MenuItem("Paste", "CTRL+V")) {}
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
    
    SetWindowSize(75,75,0,0);
    ImGui::Begin("Emulator", nullptr, im_window_flags);
    ImGui::End();

    SetWindowSize(75, 25, 0, 75);
    ImGui::Begin("Console", nullptr, im_window_flags);
    ImGui::End();

    SetWindowSize(25, 50, 75, 0);
    ImGui::Begin("Registers", nullptr, im_window_flags);
    ImGui::End();

    SetWindowSize(25, 50, 75, 50);
    ImGui::Begin("Memory", nullptr, im_window_flags);
    ImGui::End();
}

void ImGuiDataContext::ShowExampleMenuFile()
{
    ImGui::MenuItem("(demo menu)", NULL, false, false);
    if (ImGui::MenuItem("New")) {}
    if (ImGui::MenuItem("Open", "Ctrl+O")) {}
    if (ImGui::BeginMenu("Open Recent"))
    {
        ImGui::MenuItem("fish_hat.c");
        ImGui::MenuItem("fish_hat.inl");
        ImGui::MenuItem("fish_hat.h");
        if (ImGui::BeginMenu("More.."))
        {
            ImGui::MenuItem("Hello");
            ImGui::MenuItem("Sailor");
            if (ImGui::BeginMenu("Recurse.."))
            {
                ShowExampleMenuFile();
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Save", "Ctrl+S")) {}
    if (ImGui::MenuItem("Save As..")) {}

    ImGui::Separator();
    if (ImGui::BeginMenu("Options"))
    {
        static bool enabled = true;
        ImGui::MenuItem("Enabled", "", &enabled);
        ImGui::BeginChild("child", ImVec2(0, 60), ImGuiChildFlags_Border);
        for (int i = 0; i < 10; i++)
            ImGui::Text("Scrolling Text %d", i);
        ImGui::EndChild();
        static float f = 0.5f;
        static int n = 0;
        ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
        ImGui::InputFloat("Input", &f, 0.1f);
        ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Colors"))
    {
        float sz = ImGui::GetTextLineHeight();
        for (int i = 0; i < ImGuiCol_COUNT; i++)
        {
            const char* name = ImGui::GetStyleColorName((ImGuiCol)i);
            ImVec2 p = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
            ImGui::Dummy(ImVec2(sz, sz));
            ImGui::SameLine();
            ImGui::MenuItem(name);
        }
        ImGui::EndMenu();
    }

    // Here we demonstrate appending again to the "Options" menu (which we already created above)
    // Of course in this demo it is a little bit silly that this function calls BeginMenu("Options") twice.
    // In a real code-base using it would make senses to use this feature from very different code locations.
    if (ImGui::BeginMenu("Options")) // <-- Append!
    {
        static bool b = true;
        ImGui::Checkbox("SomeOption", &b);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Disabled", false)) // Disabled
    {
        IM_ASSERT(0);
    }
    if (ImGui::MenuItem("Checked", NULL, true)) {}
    ImGui::Separator();
    if (ImGui::MenuItem("Quit", "Alt+F4")) {}
}