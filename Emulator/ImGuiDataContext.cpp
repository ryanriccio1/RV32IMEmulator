#include "ImGuiDataContext.h"

using namespace std;
ImGuiDataContext::ImGuiDataContext(const string& window_name, const Uint32 SDL_flags, const Uint32 window_flags, const Uint32 renderer_flags,
    const ImGuiConfigFlags io_config_flags, const int base_width, const int base_height, const shared_ptr<RV32IM::Core>& core) :
	io((ImGui::CreateContext(), ImGui::GetIO())),  // NOLINT(clang-diagnostic-comma)
	style{ImGui::GetStyle()},
	base_width{base_width},
	base_height{base_height},
	core{core},
	core_clock_running(true)
{
    // Setup SDL
    if (SDL_Init(SDL_flags) != 0)
    {
        SDL_Log("Error: SDL_Init(): %s\n", SDL_GetError());
        throw SDLContextInitializationError();
    }

    // Create SDL window
    window = SDL_CreateWindow(window_name.c_str(), base_width, base_height, window_flags);
    if (window == nullptr)
    {
        SDL_Log("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        throw SDLContextInitializationError();
    }

    // Create SDL renderer
    renderer = SDL_CreateRenderer(window, nullptr, renderer_flags);
    if (renderer == nullptr)
    {
        SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
        throw SDLContextInitializationError();
    }


    // setup ImGui context
    IMGUI_CHECKVERSION();
	io.ConfigFlags |= io_config_flags;
    io.IniFilename = nullptr;   // no ImGui logs
    io.LogFilename = nullptr;

    // default window flags
    im_window_flags = 0;
    im_window_flags |= ImGuiWindowFlags_NoMove;
    im_window_flags |= ImGuiWindowFlags_NoResize;
    im_window_flags |= ImGuiWindowFlags_NoCollapse;
    im_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

    // adjust for HiDPI
    last_scale = 1.0f;
    update_dpi();

    // keep track of current selected window
    current_window = CurrentWindow::None;

    // setup default styles
    ImGui::StyleColorsDark();

    // connect backend/renderer to ImGui
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
    SDL_SetRenderDrawColor(renderer, 114, 140, 153, 255);

    // create emulator window texture
    emulator_screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                        core->get_video_width(), core->get_video_height());
    SDL_SetTextureScaleMode(emulator_screen, SDL_SCALEMODE_NEAREST);    // do not perform texture interpolation

    // show window newly created window
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);
}

ImGuiDataContext::~ImGuiDataContext()
{
    // cleanup backend
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    // shutdown event
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void ImGuiDataContext::new_frame()
{
    // start new frame draw list
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void ImGuiDataContext::update()
{
    core_clock_running = core->is_clock_running();
    // menu bar
    show_menu_bar();

    // emulator
    set_next_window_size(70, 75, 0, 0);
    show_window_emulator();

    // console
    set_next_window_size(70, 25, 0, 75, true);
    show_window_console();

    // registers
    set_next_window_size(30, 45, 70, 0);
    show_window_registers();

    // memory
    set_next_window_size(30, 55, 70, 45, true);
    show_window_memory();

    // file dialog
    show_dialog_open_file();
}

void ImGuiDataContext::render()
{
    // render data from ImGui to SDL
    ImGui::Render();
    SDL_RenderClear(renderer);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(renderer);
}

void ImGuiDataContext::update_dpi()
{
    // get scale
    dpi_scale = SDL_GetWindowDisplayScale(window);

    // load font and scale font atlas
	io.Fonts->AddFontFromMemoryCompressedBase85TTF(ConsolaTTF_compressed_data_base85,13 * dpi_scale);

    // scale default styles
	style.ScaleAllSizes(sqrt(dpi_scale / last_scale));

    // scale window based on dpi;
    SDL_SetWindowSize(window, static_cast<int>(static_cast<float>(base_width) * dpi_scale), 
							  static_cast<int>(static_cast<float>(base_height) * dpi_scale));

    // store last scale to dictate scaling change during runtime
	last_scale = dpi_scale;
}

SDL_WindowID ImGuiDataContext::get_window_id() const
{
    return SDL_GetWindowID(window);
}

CurrentWindow ImGuiDataContext::get_current_window() const
{
    return current_window;
}

void ImGuiDataContext::read_file_to_core(const string& file_path_name)
{
    last_file_path = file_path_name;
    // create memory for file contents
	const auto file_contents = shared_ptr<uint8_t[]>(new uint8_t[core->get_memory_size()]);

	ifstream file(file_path_name, std::ios::binary);
    file.seekg(0, std::ios::end);
    const streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // read file contents into memory
    memset(file_contents.get(), 0, core->get_memory_size());
    file.read(reinterpret_cast<char*>(file_contents.get()), min(size, static_cast<streamsize>(core->get_memory_size())));
    file.close();

    // load to risc core
    core->load_memory_contents(file_contents);
}

void ImGuiDataContext::set_next_window_size(const float width, const float height, const float pos_x, const float pos_y, const bool end) const
{
    // set values for ImGui internal window based on percentages and DPI
    ImVec2 window_size;
    if (end)
    {
        window_size = ImVec2(static_cast<float>(base_width) * (width / 100.0f) * dpi_scale,
							 static_cast<float>(base_height) * (height / 100.0f) * dpi_scale - ImGui::GetFrameHeight());
    }
    else
    {
        window_size = ImVec2(static_cast<float>(base_width) * (width / 100.0f) * dpi_scale,
							 static_cast<float>(base_height) * (height / 100.0f) * dpi_scale);
    }
    

    ImGui::SetNextWindowSize(window_size);
    ImGui::SetNextWindowPos(ImVec2(pos_x / 100.0f * dpi_scale * static_cast<float>(base_width),
								   pos_y / 100.0f * dpi_scale * static_cast<float>(base_height) + ImGui::GetFrameHeight()));
}

void ImGuiDataContext::show_window_emulator()
{
    ImGui::Begin("Emulator", nullptr, im_window_flags);
    if (ImGui::IsWindowFocused())
        current_window = CurrentWindow::Emulator;

       
    SDL_UpdateTexture(emulator_screen, nullptr, core->get_video_memory().get(), core->get_video_width() * 4);

    const auto image_size = ImVec2(static_cast<float>(core->get_video_width() * 2) * dpi_scale, 
                                   static_cast<float>(core->get_video_height() * 2) * dpi_scale);
    
	ImGui::SetCursorPosX((ImGui::GetWindowSize().x - image_size.x) * 0.5f);
    ImGui::Image(emulator_screen, image_size);

    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - image_size.x) * 0.5f);
    ImGui::BeginChild("stuff");
	if (core_clock_running) ImGui::BeginDisabled();
    if(ImGui::Button("Start"))
    {
        core->start_clock();
    }
    ImGui::SameLine();
    if (ImGui::Button("Step"))
    {
        core->step_clock();
    }
    ImGui::SameLine();
    if (core_clock_running) ImGui::EndDisabled();

	if (!core_clock_running) ImGui::BeginDisabled();
	if (ImGui::Button("Stop"))
    {
        core->stop_clock();
    }
    if (!core_clock_running) ImGui::EndDisabled();

    ImGui::SameLine();
    if (ImGui::Button("Reset"))
        reset_to_file();

    ImGui::SameLine();
    ImGui::SetNextItemWidth(125 * dpi_scale);

    static string clock_delay;
    if (ImGui::InputTextWithHint("Desired Clock Length", "delay (ns)", &clock_delay, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        try
        {
            core->set_desired_clock_time(stoi(clock_delay));
        }
        catch (std::invalid_argument const& ex)
        {
        }
        catch (std::out_of_range const& ex)
        {
        }
    }

    ImGui::EndChild();
    ImGui::End();
}

void ImGuiDataContext::show_window_console()
{
    ImGui::Begin("Console", nullptr, im_window_flags);
    if (ImGui::IsWindowFocused())
        current_window = CurrentWindow::Console;

	ImGui::Text("%.1f FPS (%.2f ms/frame) | %d (%d) ns/clock (%.1f MHz)", io.Framerate, 1000.0f / io.Framerate, core->get_average_clock_time(), core->get_average_processing_time(), 1000.0f / static_cast<float>(core->get_average_clock_time()));
    ImGui::Separator();

	/*static const float footerHeightToReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollRegion##", ImVec2(0, -footerHeightToReserve), false, 0);*/
    ImGui::PushTextWrapPos();

	ImGui::TextUnformatted(core->get_uart_data().c_str());
   
    ImGui::PopTextWrapPos();
    //ImGui::EndChild();

    // Auto-scroll logs.
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    //ImGui::Separator();
    //static string uart_input;

    //ImGui::PushItemWidth(-1);
    ///*if(*/ImGui::InputText("##", &uart_input);
    /*{
        for (const auto character : uart_input)
        {
            core->notify_uart_keypress(character);
        }
        strcpy_s(uart_input, 1024, "");
    }
    ImGui::PopItemWidth();
    */
    ImGui::End();
}

void ImGuiDataContext::show_window_registers()
{
    ImGui::Begin("Registers", nullptr, im_window_flags);
    if (ImGui::IsWindowFocused())
        current_window = CurrentWindow::Registers;

    const char* format_modes[] = { "Hexadecimal", "Signed", "Unsigned", "IEEE-754" };
    static const char* current_item = "Hexadecimal";
    ImGui::SetNextItemWidth(-1);
    if (ImGui::BeginCombo("##combo", current_item)) // The second parameter is the label previewed before opening the combo.
    {
        for (auto& format_mode : format_modes)
        {
	        const bool is_selected = (current_item == format_mode); // You can store your selection however you want, outside or inside your objects

            if (ImGui::Selectable(format_mode, is_selected))
            {
	            current_item = format_mode;
            }

	        if (is_selected)
	        {
		        ImGui::SetItemDefaultFocus(); // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
	        }
        }
        ImGui::EndCombo();
    }

    ImGui::Text(format("PC: {:#010x} | IRQ: {}", core->get_current_address(), core->get_irq()).c_str());  // NOLINT(clang-diagnostic-format-security)

	auto registers = core->get_registers();
	ImGuiTableFlags table_flags = 0;
    table_flags |= ImGuiTableFlags_Borders;
    table_flags |= ImGuiTableFlags_RowBg;

    if (ImGui::BeginTable("table1", 2, table_flags))
    {
        // setup table columns/headers
        constexpr ImGuiTableColumnFlags column_flags = ImGuiTableColumnFlags_WidthFixed;
        ImGui::TableSetupColumn("Register", column_flags, 70.0f * dpi_scale);
        ImGui::TableSetupColumn("Value", column_flags, 300.0f * dpi_scale);
        ImGui::TableHeadersRow();

    	if (core_clock_running) ImGui::BeginDisabled();
        for (int row = 0; row < 32; row++)
        {
            ImGui::TableNextRow();

            ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(1, 0.5f));
            ImGui::TableSetColumnIndex(0);
            ImGui::Selectable(RV32IM::register_name_to_string[static_cast<RV32IM::RegisterName>(row)].c_str());  // NOLINT(clang-diagnostic-format-security)

            ImGui::TableSetColumnIndex(1);
            char data[] = "0x00000000";
            if (strcmp(current_item, "Hexadecimal") == 0)
                strcpy_s(data, format("{:#010x}", registers[static_cast<RV32IM::RegisterName>(row)]).c_str());
            if (strcmp(current_item, "Signed") == 0)
                strcpy_s(data, format("{}", static_cast<int32_t>(registers[static_cast<RV32IM::RegisterName>(row)])).c_str());
            if (strcmp(current_item, "Unsigned") == 0)
                strcpy_s(data, format("{}", registers[static_cast<RV32IM::RegisterName>(row)]).c_str());
            if (strcmp(current_item, "IEEE-754") == 0)
                strcpy_s(data, format("{:.4e}", bit_cast<float>(registers[static_cast<RV32IM::RegisterName>(row)])).c_str());
            if (row == 0)
                strcpy_s(data, "0x00000000");
            ImGui::Selectable(data);  // NOLINT(clang-diagnostic-format-security)
            ImGui::PopStyleVar();
            
        }
        if (core_clock_running) ImGui::EndDisabled();
        ImGui::EndTable();
    }
    ImGui::End();
}

void ImGuiDataContext::show_window_memory()
{
    static MemoryEditor memory_editor;
    memory_editor.Cols = 8;
    

    if (core_clock_running) 
        memory_editor.ReadOnly = true;
    else 
        memory_editor.ReadOnly = false;

	if (ImGui::IsWindowFocused())
        current_window = CurrentWindow::Memory;

	memory_editor.DrawWindow("Memory", core->get_memory_ptr().get(), core->get_memory_size(), im_window_flags | ImGuiWindowFlags_NoScrollbar);
    //ImGui::End();
}

void ImGuiDataContext::show_menu_bar()
{
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("File"))
    {
        show_menu_bar_file();
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Options"))
    {
        show_menu_bar_options();
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
}

void ImGuiDataContext::show_menu_bar_file()
{
    ImGui::MenuItem("Emulator Options", nullptr, false, false);
    if (ImGui::MenuItem("Open"))
    {
        IGFD::FileDialogConfig config;
        config.path = ".";
        config.flags = ImGuiFileDialogFlags_Modal | ImGuiFileDialogFlags_CaseInsensitiveExtention;
    	ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".bin", config);
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Quit", "Alt+F4")) { quick_exit(0); }
}

void ImGuiDataContext::show_menu_bar_options()
{
    ImGui::MenuItem("Actions", nullptr, false, false);

    if (core_clock_running) ImGui::BeginDisabled();
    if (ImGui::MenuItem("Start Clock", "Ctrl+K"))
    {
        core->start_clock();
    }
    if (ImGui::MenuItem("Step Clock", "Ctrl+L"))
    {
        core->step_clock();
    }
    if (core_clock_running) ImGui::EndDisabled();

    if (!core_clock_running) ImGui::BeginDisabled();
    if (ImGui::MenuItem("Stop Clock", "Ctrl+J"))
    {
        core->stop_clock();
    }
    if (!core_clock_running) ImGui::EndDisabled();
    if (ImGui::MenuItem("Reset", "Ctrl+R"))
        reset_to_file();
}

void ImGuiDataContext::show_dialog_open_file()
{
    set_next_window_size(80, 80, 10, 10);
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey", im_window_flags ^ ImGuiWindowFlags_NoBringToFrontOnFocus)) {
        if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
            const string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            const string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            read_file_to_core(filePath + IGFD::Utils::GetPathSeparator() + filePathName);
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

void ImGuiDataContext::reset_to_file()
{
    if (!last_file_path.empty())
        read_file_to_core(last_file_path);
}
