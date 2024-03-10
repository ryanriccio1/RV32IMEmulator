#pragma once

#include <exception>
#include <string>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#include <SDL3/SDL.h>

#include "ConsolaTTF.h"

using namespace std;

class ImGuiDataContext
{
public:
	ImGuiDataContext(const string& window_name, Uint32 SDL_flags, Uint32 window_flags, Uint32 renderer_flags, ImGuiConfigFlags io_config_flags, int base_width, int base_height);
	virtual ~ImGuiDataContext();

	virtual void NewFrame();
	virtual void Update();
	virtual void Render();
	void UpdateDPI();
	SDL_WindowID GetWindowID() const;

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

	void SetWindowSize(const float& width, const float& height, const int pos_x, const int pos_y) const;
	static void ShowExampleMenuFile();

};

class SDLContextInitializationError : public exception
{
	const char* what() const noexcept override
	{
		return "Error initializing data context!";
	}
};