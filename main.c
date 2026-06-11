#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

typedef struct p3
{
    float x, y, z;
} p3;

#define NUMPOINTS 10000
p3 model[NUMPOINTS];
p3 rotated[NUMPOINTS];

void clear(SDL_Surface *surface)
{
    int height = surface->h;
    int width = surface->w;
    int pitch = surface->pitch; // how many bits per pixel
    uint8_t *fb = (uint8_t *)surface->pixels;
    memset(fb, 0, pitch * height);
}

void create_model()
{
#if 1
    for (int i = 0; i <= NUMPOINTS; i++)
    {
        model[i].x = -100 + rand() % 200;
        model[i].y = -100 + rand() % 200;
        model[i].z = -100 + rand() % 200;
    }
#else
    int j = 0;
    for (float x = -50; x < 50; x++)
    {
        for (float z = -50; z <= 50; z++)
        {
            float y = 10 + (sin(x / 100 * 3.14 * 5) * 5) + (cos(z / 100 * 3.14 * 5) * 5);
            model[j].x = x * 4;
            model[j].y = y * 4;
            model[j].z = z * 4;
            j++;
            if (j >= NUMPOINTS)
                return;
        }
    }
#endif
}

void pixel(SDL_Surface *surface, int x, int y, int r, int g, int b)
{
    int height = surface->h;
    int width = surface->w;
    if (x < 0 || x > width)
        return;
    if (y < 0 || y > height)
        return;
    int pitch = surface->pitch; // how many bits per pixel
    uint8_t *fb = (uint8_t *)surface->pixels;

    fb[y * pitch + x * 4 + 0] = b;
    fb[y * pitch + x * 4 + 1] = g;
    fb[y * pitch + x * 4 + 2] = r;
    fb[y * pitch + x * 4 + 3] = 1;
}

void rotate(float time)
{
    float alpha = 100;
    for (int j = 0; j < NUMPOINTS; j++)
    {
        /*
         *  Rotation alogn the y axis is:
         *   x' = x cos(theta) + z sin(theta)
         *   y' = y
         *   z' = -x sin(theta) + z cos(theta)
         */
        rotated[j].x = model[j].x * cos(time / alpha) + model[j].z * sin(time / alpha);
        rotated[j].y = model[j].y;
        rotated[j].z = -model[j].x * sin(time / alpha) + model[j].z * cos(time / alpha);
    }
}

void draw(SDL_Surface *surface, float time)
{
    int height = surface->h;
    int width = surface->w;

    int cx = width / 2;
    int cy = height / 2;

    rotate(time);
    clear(surface);
    for (int j = 0; j < NUMPOINTS; j++)
    {
        float zfactor = 1 + (rotated[j].z / 300);
        float x = rotated[j].x / zfactor;
        float y = rotated[j].y / zfactor;
        pixel(surface, cx + x, cy + y, 255, 255, 255);
        /*
        float alpha = (float)j / 1000;
        int x = cx + sin(alpha) * 50;
        int y = cy + cos(alpha) * 50;
        pixel(surface, x, y, 255, 255, 255);
    */
    }
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    // Create window
    SDL_Window *window = SDL_CreateWindow(
        "Pixel Framebuffer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 580, SDL_WINDOW_SHOWN);
    if (!window)
    {
        SDL_Log("SDL_CreateWIndow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Get the window's surface (the framebuffer)
    SDL_Surface *surface = SDL_GetWindowSurface(window);
    if (!surface)
    {
        SDL_Log("SDL_GetWindowSurface failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Main loop
    create_model();
    int running = 1;
    float time = 0;
    while (running)
    {

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
            }
        }

        draw(surface, time);
        SDL_UpdateWindowSurface(window);
        time = time + 1;
        SDL_Delay(16);
    }

    // CLeanup
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}