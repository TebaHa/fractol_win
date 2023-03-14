
#include "Fractol.h"


//please don't put all your code in main like I did.
int main()
{
	Fractol fractol;

	while (fractol.IsRunning) {
		fractol.HandleEvents();
		fractol.Update();
		fractol.Render();
	}

	return 0;
}

Fractol::Fractol()
	: Runner()
{
	Fullscreen = false;
	int flags = 0;
	flags = SDL_WINDOW_RESIZABLE;

	if (Fullscreen)
	{
		flags = flags | SDL_WINDOW_FULLSCREEN;
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		std::cout << "Subsystems Initialized!\n";

		Window = SDL_CreateWindow("fractol", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, flags);
		if (Window)
		{
			std::cout << "Window Created!\n";
			SDL_SetWindowMinimumSize(Window, WIDTH, HEIGHT);
		}

		Renderer = SDL_CreateRenderer(Window, -1, 0);
		if (Renderer)
		{
			std::cout << "Renderer created!\n";
			//SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
			IsRunning = true;
		}

		Runner.Surface = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, (unsigned int)0xff000000, (unsigned int)0x00ff0000, (unsigned int)0x0000ff00, (unsigned int)0x000000ff);
	}
}

void Fractol::HandleEvents()
{
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type)
	{
	case SDL_QUIT:
		IsRunning = false;
	case SDL_MOUSEWHEEL:
		HandleZoom(event);
		break;
	case SDL_KEYDOWN:
	if (event.key.keysym.sym == SDLK_ESCAPE)
	{
		IsRunning = false;
	}
	else if (event.key.keysym.sym == SDLK_1)
	{
		delete Runner.FractalData.Palette;
		Runner.FractalData.Palette = Runner.FractalData.GeneratePalette(0xE69800, 0xFCFFB2, Runner.FractalData.Iteration);
	}
	else if (event.key.keysym.sym == SDLK_2)
	{
		delete Runner.FractalData.Palette;
		Runner.FractalData.Palette = Runner.FractalData.GeneratePalette(0x66B2FF, 0xFFE5FF, Runner.FractalData.Iteration);
	}
	else if (event.key.keysym.sym == SDLK_3)
	{
		delete Runner.FractalData.Palette;
		Runner.FractalData.Palette = Runner.FractalData.GeneratePalette(0xFF3939, 0xDAFFC8, Runner.FractalData.Iteration);
	}
	break;
	default:
		break;
	}
}

void Fractol::Render()
{
	SDL_Texture* texture;

	SDL_LockSurface(Runner.Surface);
	texture = SDL_CreateTextureFromSurface(Renderer, Runner.Surface);
	SDL_RenderCopy(Renderer, texture, NULL, NULL);
	SDL_DestroyTexture(texture);
	SDL_RenderPresent(Renderer);
	SDL_RenderClear(Renderer);
	SDL_UnlockSurface(Runner.Surface);
}

//simple update function
void Fractol::Update() {
	Runner.Run();
}

FractalData::FractalData()
{
	Xside = 8;
	Yside = 5;
	Scale = 0.005;
	Top = -3;
	Left = -5;
	Iteration = 100;
	ColorType = 1;
	Cx = 0.0;
	Cy = 0.0;
	Zx = 0.0;
	Zy = 0.0;
	I = 0;
	Mi = Iteration;
	Palette = GeneratePalette(0xE69800, 0xFCFFB2, Iteration);
}