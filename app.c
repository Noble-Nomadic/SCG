#include "SCG.h"
#include <math.h>
#include <SDL2/SDL.h>

// Window dimensions
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// Cube object
Cube mainCube;


void mainLoop() {

    SCG_CubeRotate(&mainCube, (vec3){0.25, 0.5, 0.75});

    // Draw cube
    SCG_DrawCube(&mainCube, WINDOW_WIDTH, WINDOW_HEIGHT);
}

int main() {
    if (!SCG_Init("SCG", WINDOW_WIDTH, WINDOW_HEIGHT)) {
        return 1;
    }

    SCG_FOV = 45.0f;
    SCG_NearPlane = 0.1f;
    SCG_FarPlane = 1000.0f;

    // Initialize cube
    mainCube = SCG_NewCube((vec3){0, 0, 10});
    SCG_DrawCube(&mainCube, WINDOW_WIDTH, WINDOW_HEIGHT);
    SCG_Present();
    SDL_Delay(500);

    // Start main loop
    SCG_AppLoop(mainLoop);

    SCG_Quit();
    return 0;
}
