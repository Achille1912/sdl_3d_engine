#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// This is our struct to represent a point in a 3D world.
typedef struct p3
{
    float x, y, z;
} p3;

// This is our struct to represent a point in a 2D world.
typedef struct p2
{
    int x, y;
} p2;
// Here we define two arrays of 3D-Points of NUMPOINTS length.
#define NUMPOINTS 10000
p3 model[NUMPOINTS];
p3 rotated[NUMPOINTS];
p2 model2d[NUMPOINTS];
#define ROTATING 1

// In this function we take the SDL Frame Buffer and set the memory at 0.
void clear(SDL_Surface *surface)
{
    int height = surface->h;
    int width = surface->w;
    int pitch = surface->pitch; // how many bits per pixel
    uint8_t *fb = (uint8_t *)surface->pixels;
    memset(fb, 0, pitch * height); // (width * 4) * height
}

// In this function we set all the cells of the model.
// In other words: here we set the figure to show up on the FB.
// * An important thing is that the array adjacity is lost
// * in the figure. I mean, two adjacent cells in model
// * maybe could be far away in the 3D figure.
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

// In this function we have the pointer of the surface, the (x,y) 2-dimensional
// coord of the pixel and the color (r,g,b encoded).
// And with this stuff we can set the color at the exact rappresentation's position
// in the Frame Buffer.
// ! The SDL_Surface uses the BGR encoding.
void pixel(SDL_Surface *surface, int x, int y, int r, int g, int b)
{
    int height = surface->h;
    int width = surface->w;
    // ======== Security check ==========
    if (x < 0 || x > width)
        return;
    if (y < 0 || y > height)
        return;
    // ==================================

    int pitch = surface->pitch; // how many bits per pixel
    uint8_t *fb = (uint8_t *)surface->pixels;

    /*
     * The formula that converts the (x,y) window point
     * in the Frame Buffer array coordinate is:
     *   j = (y * pitch) + (x * 4) + channel_code
     * The '4' refers to the 4 channel that we have.
     */
    fb[y * pitch + x * 4 + 0] = b;
    fb[y * pitch + x * 4 + 1] = g;
    fb[y * pitch + x * 4 + 2] = r;
    fb[y * pitch + x * 4 + 3] = 255;
}

p3 *pixel_neighbors(p3 *p)
{

    // TODO: neighbors function
}

void line(SDL_Surface *surface, p3 *pa, p3 *pb, int r, int g, int b)
{
    int height = surface->h;
    int width = surface->w;

    int cx = width / 2;
    int cy = height / 2;

    float dx = pb->x - pa->x;
    float dy = pb->y - pa->y;

    int steps = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);
    if (steps == 0)
        steps = 1;

    float x_increment = dx / steps;
    float y_increment = dy / steps;

    float x = pa->x;
    float y = pb->y;
    for (int i = 0; i < steps; i++)
    {
        pixel(surface, (int)x + cx, (int)y + cy, r, g, b);
        x += x_increment;
        y += y_increment;
    }
}

// This function performs the Rotate Transformation on the model.
// There is the time parameter too, in order to do some cool stuff.
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

void update(float time)
{
    if (ROTATING == 1)
        rotate(time);
    return;
}

/* This function, for all the points in the 3D model, calculates
the convertion from the 3D coordinates to the 2D FB world.

1. We calculate a zfactor, that is a dynamic coefficient that
    squeeze the x and y values when the z is far away from the viewer
    and does the opposite when the z value is near the viewer.
2. Sending the (x,y) coordinates with the z dependence, we perform a traslation
    to the center in addition.
*/
void draw(SDL_Surface *surface)
{
    int height = surface->h;
    int width = surface->w;

    int cx = width / 2;
    int cy = height / 2;

    p3 *draw_model;
    if (ROTATING)
        draw_model = rotated;
    else
        draw_model = model;

    clear(surface);

    for (int j = 0; j < NUMPOINTS; j++)
    {
        float zfactor = 1 + (draw_model[j].z / 300);
        model2d[j].x = cx + draw_model[j].x / zfactor;
        model2d[j].y = cy + draw_model[j].y / zfactor;

        pixel(surface, model2d[j].x, model2d[j].y, 255, 255, 255);
    }

    line(surface, &draw_model[10], &draw_model[11], 255, 0, 0);
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
        update(time);
        draw(surface);
        SDL_UpdateWindowSurface(window);
        time = time + 1;
        SDL_Delay(16);
    }

    // Cleanup
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}