#include "ImGuiDataContext.h"

using namespace std;
ImGuiDataContext::ImGuiDataContext(const string& window_name, Uint32 SDL_flags, Uint32 window_flags, Uint32 renderer_flags,
                                   ImGuiConfigFlags io_config_flags, int base_width, int base_height, shared_ptr<Computer::Computer>& computer) :
	io{(ImGui::CreateContext(), ImGui::GetIO())},
	style{ImGui::GetStyle()},
	base_width{base_width},
	base_height{base_height},
	computer{computer}
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
    im_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

    last_scale = 1.0f;

    UpdateDPI();
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
    SDL_SetRenderDrawColor(renderer, 114, 140, 153, 255);

    emulator_screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 320, 240);
    SDL_SetTextureScaleMode(emulator_screen, SDL_SCALEMODE_NEAREST);

    current_window = CurrentWindow::None;
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
    // Rendering
    ImGui::Render();
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

void ImGuiDataContext::ReadFileToComputer(const string& filePathName) const
{
    auto file_contents = shared_ptr<uint8_t[]>(new uint8_t[computer->get_memory_size()]);

	ifstream file(filePathName, std::ios::binary);
    file.seekg(0, std::ios::end);
    const streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    memset(file_contents.get(), 0, computer->get_memory_size());
    file.read(reinterpret_cast<char*>(file_contents.get()), min(size, static_cast<streamsize>(computer->get_memory_size())));
    file.close();

    computer->load_memory_contents(file_contents);
}

void ImGuiDataContext::UpdateVideoBufferPointer()
{
    video_buffer = computer->get_video_memory();
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
    computer->video_interface->update_memory();
    if (ImGui::BeginMenu("File"))
    {
        ShowMenuFile();
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Options"))
    {
        ShowMenuOptions();
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();

    // Emulator
    SetWindowSize(75,75,0,0);
    ImGui::Begin("Emulator", nullptr, im_window_flags);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    if (ImGui::IsWindowFocused())
    {
        current_window = CurrentWindow::Emulator;
    }
	SDL_UpdateTexture(emulator_screen, nullptr, computer->get_video_memory().get(), computer->get_video_width() * 4);

    ImGui::Image(emulator_screen, ImVec2(computer->get_video_width() * 2 * dpi_scale,computer->get_video_height() * 2 * dpi_scale));
    ImGui::End();

    // Console
    SetWindowSize(75, 25, 0, 75);
    ImGui::Begin("Console", nullptr, im_window_flags);
    if (ImGui::IsWindowFocused())
        current_window = CurrentWindow::Console;
    ImGui::End();

    // Registers
    SetWindowSize(25, 50, 75, 0);
    ImGui::Begin("Registers", nullptr, im_window_flags);
    if (ImGui::IsWindowFocused())
        current_window = CurrentWindow::Registers;
    ImGui::End();

    // Memory
    SetWindowSize(25, 50, 75, 50);
    ImGui::Begin("Memory", nullptr, im_window_flags);
    if (ImGui::IsWindowFocused())
        current_window = CurrentWindow::Memory;
    ImGui::End();

    // File Dialog
    SetWindowSize(80, 80, 10, 10);
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
            const string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            const string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            ReadFileToComputer(filePath + IGFD::Utils::GetPathSeparator() + filePathName);
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

void ImGuiDataContext::ShowMenuFile()
{
    ImGui::MenuItem("Emulator Options", nullptr, false, false);
    if (ImGui::MenuItem("Open", "Ctrl+O"))
    {
        IGFD::FileDialogConfig config;
        config.path = ".";
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".bin", config);
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Quit", "Alt+F4")) { exit(0); }
}

void ImGuiDataContext::ShowMenuOptions()
{
    ImGui::MenuItem("Actions", nullptr, false, false);
    if (ImGui::MenuItem("Toggle Clock", "Ctrl+K"))
    {
        // toggle clock
    }
    if (ImGui::MenuItem("Pulse Clock", "Ctrl+J"))
    {
	    // pulse clock
    }
}
