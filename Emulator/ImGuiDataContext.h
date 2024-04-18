#pragma once

#include <exception>
#include <string>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#include <SDL3/SDL.h>

#include <ImGuiFileDialog.h>

#include "ConsolaTTF.h"
#include "../Core/core.h"

using namespace std;

enum class CurrentWindow { Emulator, Console, Registers, Memory, None };

class ImGuiDataContext
{
public:
	ImGuiDataContext(const string& window_name, Uint32 SDL_flags, Uint32 window_flags, Uint32 renderer_flags, ImGuiConfigFlags io_config_flags, int base_width, int base_height, const shared_ptr<RV32IM::Core>& core);
	virtual ~ImGuiDataContext();

	virtual void new_frame();
	virtual void update();
	virtual void render();
	void update_dpi();
	SDL_WindowID get_window_id() const;

private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	ImGuiIO& io;
	ImGuiStyle& style;
	ImGuiWindowFlags im_window_flags;
	int base_width;
	int base_height;
	float dpi_scale;
	float last_scale;
	CurrentWindow current_window; 

	SDL_Texture* emulator_screen;
	shared_ptr<RV32IM::Core> core;
	bool core_clock_running;

	void read_file_to_core(const string& filePathName) const;
	void set_next_window_size(float width, float height, float pos_x, float pos_y) const;

	void show_window_emulator();
	void show_window_console();
	void show_window_registers();
	void show_window_memory();

	void show_menu_bar();
	void show_menu_bar_file();
	void show_menu_bar_options() const;

	void show_dialog_open_file();
};

class SDLContextInitializationError : public exception
{
	const char* what() const noexcept override
	{
		return "Error initializing data context!";
	}
};

