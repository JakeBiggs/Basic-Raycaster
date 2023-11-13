#include "stdafx.h"
#include "Application.h"
#include "Object.h"

// Constructor -- initialise application-specific data here
Application::Application()
{
}

Application::~Application()
{
	for (auto obj : m_objects)
	{
		if (obj != nullptr)
			delete obj;
	}
}

// Run the application
// Return true if the application finishes successfully, or false if an error occurs
bool Application::run()
{
	// Initialisation
	if (!initSDL())
		return false;

	setupScene();

	// Main loop
	m_quit = false;
	while (!m_quit)
	{
		// Process events
		SDL_Event ev;
		while (SDL_PollEvent(&ev))
		{
			processEvent(ev);
		}

		// Update objects' positions
		update();

		// Render
		render();
		SDL_RenderPresent(m_renderer);
	}

	// Shutdown
	shutdownSDL();
	return true;
}

// Initialise the required parts of the SDL library
// Return true if initialisation is successful, or false if initialisation fails
bool Application::initSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return false;
	}

	m_window = SDL_CreateWindow("COMP270", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, c_windowWidth, c_windowHeight, SDL_WINDOW_SHOWN);
	if (m_window == nullptr)
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		return false;
	}

	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (m_renderer == nullptr)
	{
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		return false;
	}

	return true;
}

// Shutdown the SDL library
void Application::shutdownSDL()
{
	if (m_renderer)
	{
		SDL_DestroyRenderer(m_renderer);
		m_renderer = nullptr;
	}

	if (m_window)
	{
		SDL_DestroyWindow(m_window);
		m_window = nullptr;
	}

	SDL_Quit();
}

// Process a single event
void Application::processEvent(const SDL_Event &ev)
{
	switch (ev.type)
	{
	case SDL_QUIT:
		m_quit = true;
		break;

	case SDL_KEYDOWN:
	{
		bool shiftMod = SDL_GetModState() & KMOD_SHIFT;
		if (ev.key.keysym.sym == SDLK_ESCAPE)
			m_quit = true;
		else if (ev.key.keysym.sym == SDLK_a)
			shiftMod ? m_camera.rotateY(0.1f) : m_camera.translateX(-1.0f);
		else if (ev.key.keysym.sym == SDLK_d)
			shiftMod ? m_camera.rotateY(-0.1f) : m_camera.translateX(1.0f);
		else if (ev.key.keysym.sym == SDLK_s)
			shiftMod ? m_camera.rotateZ(-0.1f) : m_camera.translateY(-1.0f);
		else if (ev.key.keysym.sym == SDLK_w)
			shiftMod ? m_camera.rotateZ(0.1f) : m_camera.translateY(1.0f);
		else if (ev.key.keysym.sym == SDLK_q)
			shiftMod ? m_camera.rotateX(-0.1f) : m_camera.translateZ(-1.0f);
		else if (ev.key.keysym.sym == SDLK_e)
			shiftMod ? m_camera.rotateX(0.1f) : m_camera.translateZ(1.0f);
		else if (ev.key.keysym.sym == SDLK_UP)
			m_camera.zoom(0.1f);
		else if (ev.key.keysym.sym == SDLK_DOWN)
			m_camera.zoom(-0.1f);
		break;
	}
	default:
		break;
	}
}

// Add the camera and renderable objects to the scene
void Application::setupScene()
{
	m_camera.init(Point3D(0.0f, 0.0f, 7.5f));
	/*
	m_objects.push_back(new Plane(Point3D(), Vector3D(0.0f, 0.0f, 1.0f), Vector3D(0.0f, 1.0f, 0.0f), 10.0f, 7.5f));
	m_objects[0]->m_colour = Colour(245, 121, 58);
	*/
	
	
	m_objects.push_back(new Plane(Point3D(),
		Vector3D(0.5f, 0.5f, 1.0f),
		Vector3D(-0.5f, 1.0f, -0.25f),
		10.0f, 7.5f));
	m_objects[0]->m_colour = Colour(50, 255, 50);
	m_objects.push_back(new Sphere(Point3D(0.0f, 0.0f, 3.0f)));
	m_objects[1]->m_colour = Colour(169, 90, 161);
	m_objects[1]->m_isDynamic = true;

	m_objects.push_back(new Sphere(Point3D(1.0f, 1.0f, 1.0f), 0.75f));
	m_objects[2]->m_colour = Colour(133, 192, 249);
	m_objects[2]->m_isDynamic = true;

	m_objects.push_back(new Light(Point3D(0.0f, 0.0f, 0.0f), 0.5f));
	m_objects[3]->m_colour = Colour(255, 255, 255);
	m_objects[3]->m_isDynamic = true;

}

// Apply an update to the (dynamic) objects on each frame
void Application::update()
{
	for (auto object : m_objects)
	{
		if (object->m_isDynamic)
		{
			// TODO: apply a transformation to the object.

		}
	}
}

// Render the scene (via the camera)
void Application::render()
{
	// Convert the image created by the camera to an SDL_Texture
	// that can be rendered directly to the window.
	if (m_camera.updatePixelBuffer(m_objects))
	{
		// Copy the data from the camera image to the screen texture,
		// accounting for the resolution and flipped y-axis
		static const unsigned viewPlaneWidth = m_camera.getViewPlaneResolutionX(),
							viewPlaneHeight = m_camera.getViewPlaneResolutionY();
		static const float x_step = static_cast<float>(c_windowWidth) / static_cast<float>(viewPlaneWidth);
		static const float y_step = static_cast<float>(c_windowHeight) / static_cast<float>(viewPlaneHeight);
		static SDL_FRect rect;
		rect.x = 0.0f;
		rect.w = x_step;
		rect.h = y_step;

		static const int iEnd = viewPlaneWidth, jStart = viewPlaneHeight - 1;
		for (int i = 0; i < iEnd; ++i)
		{
			rect.y = 0.0f;
			for (int j = jStart; j >= 0; --j)
			{
				const Colour col = m_camera.getColourAtPixel(i, j, m_objects);
				SDL_SetRenderDrawColor(m_renderer, col.r, col.g, col.b, col.a);
				SDL_RenderFillRectF(m_renderer, &rect);
				rect.y += y_step;
			}
			rect.x += x_step;
		}
	}
}

// Application entry point
int main(int, char**)
{
	Application application;
	if (application.run())
		return 0;
	else
		return 1;
}
