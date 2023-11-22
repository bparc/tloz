#define _CRT_SECURE_NO_WARNINGS
#include <inttypes.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>

#include <SDL_opengl.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>

#undef RGB
#define RGB(R,G,B) ((((R)|(G<<8))|(B<<16))|(0xff<<24))

#ifndef TRUE
#define TRUE 1
#endif

#define KB(Bytes) (Bytes * 1024)
#define MB(Bytes) (Bytes * 1024 * 1024)
#define GB(Bytes) (Bytes *1024 *1024*1024)
#define ArrayCount(Array) (sizeof(Array)/sizeof(*Array))

typedef struct
{
	int32_t Size;
	void* Data;
} file_contents_t;

static file_contents_t LoadFileContents(const char* path)
{
	file_contents_t result;
	memset(&result, 0, sizeof(result));
	FILE* file = fopen(path, "rb");
	if (file)
	{
		fseek(file, 0, SEEK_END);
		int32_t size = (int32_t)ftell(file);
		result.Size = size;
		fseek(file, 0, SEEK_SET);
		result.Data = malloc(result.Size);
		assert(result.Data);
		if (fread(result.Data, result.Size, 1, file) != 1)
		{
			free(result.Data);
			memset(&result, 0, sizeof(result));
		}
		fclose(file);
	}
	return result;
}

#include "math.c"
#include "framebuffer.h"
#include "framebuffer.c"
#include "chr.h"
#include "chr.c"
#include "map.c"
#include "host.c"
static float dT;
static double PrevTime;
static SDL_Window* Window;
static SDL_GLContext* Context;
static int32_t Running = TRUE;
static GLuint Texture;
int main(void)
{
	int32_t result = EXIT_SUCCESS;
	if ((SDL_Init(SDL_INIT_EVERYTHING) == 0))
	{
		Window = SDL_CreateWindow("Project1.exe", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			256 * 2, 240 * 2, SDL_WINDOW_OPENGL);
		if (Window)
		{
			SDL_SetWindowResizable(Window, SDL_TRUE);
			Context = SDL_GL_CreateContext(Window);
			if (Context)
			{
				SDL_GL_MakeCurrent(Window, Context);
			
				chr_ram_t ROM = LoadROMFromFile(".nes");
				if (ROM.Memory)
				{
					glGenTextures(1, &Texture);
					glBindTexture(GL_TEXTURE_2D, Texture);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#define X 256
#define Y 240
					static uint32_t pixels[Y][X];
					framebuffer_t buffer;
					buffer.Pixels = &pixels[0][0];
					buffer.x = X;
					buffer.y = Y;
#undef X
#undef Y
					int32_t requestedMemory = MB(2);
					void* memory = SDL_malloc(requestedMemory);
					while (Running)
					{
						SDL_Event event;
						while (SDL_PollEvent(&event))
						{
							if (event.type == SDL_QUIT)
							{
								Running = 0;
							}
							if (event.type == SDL_KEYDOWN)
							{
							}
						}

						controller_t controllers[1];
						memset(controllers, 0, sizeof(*controllers));
						int32_t numKeys = 0;
						const Uint8 *keys = SDL_GetKeyboardState(&numKeys);
						if (keys)
						{
							if (keys[SDL_SCANCODE_D])
							{
								controllers->DPad.x = +1;
							}
							else
							if (keys[SDL_SCANCODE_A])
							{
								controllers->DPad.x = -1;
							}
							else
							if (keys[SDL_SCANCODE_W])
							{
								controllers->DPad.y = -1;
							}
							else
							if (keys[SDL_SCANCODE_S])
							{
								controllers->DPad.y = +1;
							}
							controllers->A = keys[SDL_SCANCODE_PERIOD] > 0;
						}
						Host(&buffer, &ROM, memory, requestedMemory, controllers,
							(double)SDL_GetPerformanceCounter() / (double)SDL_GetPerformanceFrequency());
						// Blit framebuffer bits

						int32_t view[2];
						SDL_GetWindowSize(Window, &view[0], &view[1]);
						glViewport(0, 0, view[0], view[1]);
						glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
						glClear(GL_COLOR_BUFFER_BIT);

						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, Texture);
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, buffer.x, buffer.y,
							0, GL_RGBA, GL_UNSIGNED_BYTE, buffer.Pixels);

						glBegin(GL_QUADS);
						glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
						glTexCoord2f(+0.0f, +0.0f);  glVertex2f(-1.0f, -1.0f);
						glTexCoord2f(+1.0f, +0.0f);  glVertex2f(+1.0f, -1.0f);
						glTexCoord2f(+1.0f, -1.0f);  glVertex2f(+1.0f, 1.0f);
						glTexCoord2f(+0.0f, -1.0f);  glVertex2f(-1.0f, +1.0f);
						glEnd();

						SDL_GL_SwapWindow(Window);
					} // while (Running)
				}// if (ROM.Memory)
				else
				{
					assert(0);
				}
			}
		}
	}
	return result;
}