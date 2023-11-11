#pragma once
#include "Camera.h"

class Object;

class Application
{
public:
	Application();
	~Application();

	bool run();

private:
	bool initSDL();
	void shutdownSDL();

	void processEvent(const SDL_Event &e);
	void setupScene();
	void update();
	void render();

	const int c_windowWidth = 800;
	const int c_windowHeight = 700;

	SDL_Window* m_window = nullptr;
	SDL_Renderer* m_renderer = nullptr;

	bool m_quit = false;

	std::vector<Object*> m_objects;
	Camera m_camera;
};
