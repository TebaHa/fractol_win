#pragma once
#include "SDL.h"
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include "cl.h"
#include <iostream>
#include <fstream>
#undef main

#define KERNEL_PROGRAM_FILE "CLKernel.cl"
#define WIDTH 1920
#define HEIGHT 1080

struct FractalData
{
	int64_t	I;
	int64_t	Mi;
	double	Cx;
	double	Cy;
	double	Zx;
	double	Zy;
	double	XTemp;
	int		Type;
	int		Iteration;
	int		ColorType;
	int		Juldes;
	double	Left;
	double	Top;
	double	Xside;
	double	Yside;
	double	Scale;
	double	JulesX;
	double	JulesY;
	int*	Palette;

	FractalData();

	int* GeneratePalette(int Start, int End, int Iteration)
	{
		int		i;
		int*	palette;
		int		rgb[3];

		Iteration++;
		End -= 0xFF;
		palette = new int[Iteration];
		i = 0;
		while (i < Iteration - 1)
		{
			RgbCalculation(rgb, Start, End, Iteration, i);
			palette[i] = rgb[0] << 24 | rgb[1] << 16 | rgb[2] << 8 | 0x000000FF;
			i++;
		}
		End += 0xFF;
		RgbCalculation(rgb, Start, End, Iteration, i);
		palette[i] = 0xFFFFFFFF;
		return (palette);
	}

	void	RgbCalculation(int Rgb[3], int Start, int End, int Iters, int Index)
	{
		Rgb[0] = ((Start & 0xFF0000) >> 16) + (int)((((End & 0xFF0000) >> 16)
			- ((Start & 0xFF0000) >> 16)) * Index / Iters);
		Rgb[1] = ((Start & 0x00FF00) >> 8) + (int)((((End & 0x00FF00) >> 8)
			- ((Start & 0x00FF00) >> 8)) * Index / Iters);
		Rgb[2] = (Start & 0x0000FF) + (int)((End
			& 0x0000FF - Start & 0x0000FF) * Index / Iters);
	}
};

struct ClData
{
	cl_device_id		DeviceId;
	cl_context			Context;
	cl_program			Program;
	cl_kernel			Kernel;
	cl_command_queue	Queue;
	cl_mem				IntMem;
	cl_mem				DoubleMem;
	cl_mem				ImgMem;
	cl_mem				Palette;
	int					Return;

	ClData();

	void InitBuffers();
};

struct ClRunner
{
	ClData			ClData;
	FractalData		FractalData;
	SDL_Surface*	Surface;

	ClRunner();

	void Run();
	void SetData(int* IBuffer, double* DBuffer);
};

struct Fractol
{
	ClRunner		Runner;
	SDL_Renderer*	Renderer;
	SDL_Window*		Window;
	bool			IsRunning;
	bool			Fullscreen;

	Fractol();

	~Fractol()
	{
		SDL_DestroyRenderer(Renderer);
		SDL_DestroyWindow(Window);
		SDL_Quit();
	}

	void HandleEvents();

	void Update();

	void Render();

	void HandleZoom(SDL_Event& Event)
	{
		int x, y;

		SDL_GetMouseState(&x, &y);
		if (Event.wheel.y < 0)
		{
			Runner.FractalData.Scale += Runner.FractalData.Scale * 0.05;
			Runner.FractalData.Top -= Runner.FractalData.Scale * y *0.05;
			Runner.FractalData.Left -= Runner.FractalData.Scale * x * 0.05;
		}
		else if (Event.wheel.y > 0)
		{
			Runner.FractalData.Scale -= Runner.FractalData.Scale * 0.05;
			Runner.FractalData.Top += Runner.FractalData.Scale * y * 0.05;
			Runner.FractalData.Left += Runner.FractalData.Scale * x * 0.05;
		}
	}
};

