#include "SCG.h"
#include <math.h>
#include <SDL2/SDL.h>

// Window dimensions
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// Number of cubes to render
#define NUM_CUBES 3

// Movement speed
#define MOVE_SPEED 0.2f
#define ROTATE_SPEED 1.0f

// Cube objects
Cube cubes[NUM_CUBES];

// Time tracking for animation
Uint32 lastTime = 0;

// Function to handle keyboard input
void handleInput() {
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    
    // Move camera/cube with WASD and arrow keys
    if (keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_UP]) {
        for (int i = 0; i < NUM_CUBES; i++) {
            // Move forward (decrease Z)
            SCG_CubeMove(&cubes[i], (vec3){0, 0, -MOVE_SPEED});
        }
    }
    if (keystate[SDL_SCANCODE_S] || keystate[SDL_SCANCODE_DOWN]) {
        for (int i = 0; i < NUM_CUBES; i++) {
            // Move backward (increase Z)
            SCG_CubeMove(&cubes[i], (vec3){0, 0, MOVE_SPEED});
        }
    }
    if (keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT]) {
        for (int i = 0; i < NUM_CUBES; i++) {
            // Move left (decrease X)
            SCG_CubeMove(&cubes[i], (vec3){-MOVE_SPEED, 0, 0});
        }
    }
    if (keystate[SDL_SCANCODE_D] || keystate[SDL_SCANCODE_RIGHT]) {
        for (int i = 0; i < NUM_CUBES; i++) {
            // Move right (increase X)
            SCG_CubeMove(&cubes[i], (vec3){MOVE_SPEED, 0, 0});
        }
    }
    
    // Adjust height with Q and E
    if (keystate[SDL_SCANCODE_Q]) {
        for (int i = 0; i < NUM_CUBES; i++) {
            // Move up (increase Y)
            SCG_CubeMove(&cubes[i], (vec3){0, MOVE_SPEED, 0});
        }
    }
    if (keystate[SDL_SCANCODE_E]) {
        for (int i = 0; i < NUM_CUBES; i++) {
            // Move down (decrease Y)
            SCG_CubeMove(&cubes[i], (vec3){0, -MOVE_SPEED, 0});
        }
    }
    
    // Add rotation controls with IJKL keys
    if (keystate[SDL_SCANCODE_I]) {
        for (int i = 0; i < NUM_CUBES; i++) {
            // Rotate forward (around X axis)
            SCG_CubeRotate(&cubes[i], (vec3){ROTATE_SPEED, 0, 0});
        }
    }
    if (keystate[SDL_SCANCODE_K]) {
        for (int i = 0; i < NUM_CUBES; i++) {
            // Rotate backward (around X axis)
            SCG_CubeRotate(&cubes[i], (vec3){-ROTATE_SPEED, 0, 0});
        }
    }
    if (keystate[SDL_SCANCODE_J]) {
        for (int i = 0; i < NUM_CUBES; i++) {
            // Rotate left (around Y axis)
            SCG_CubeRotate(&cubes[i], (vec3){0, ROTATE_SPEED, 0});
        }
    }
    if (keystate[SDL_SCANCODE_L]) {
        for (int i = 0; i < NUM_CUBES; i++) {
            // Rotate right (around Y axis)
            SCG_CubeRotate(&cubes[i], (vec3){0, -ROTATE_SPEED, 0});
        }
    }
    
    // Reset cube positions with R key
    if (keystate[SDL_SCANCODE_R]) {
        // Initialize cubes at default positions
        cubes[0] = SCG_NewCube((vec3){0, 0, 5});
        cubes[1] = SCG_NewCube((vec3){-2.5, 0, 7.5});
        cubes[2] = SCG_NewCube((vec3){2.5, 0, 10});
    }
    
    // Safety check - don't let cubes get too close to the camera
    for (int i = 0; i < NUM_CUBES; i++) {
        if (cubes[i].position.z < SCG_NearPlane + 1.0f) {
            cubes[i].position.z = SCG_NearPlane + 1.0f;
        }
    }
}

// Main loop function
void mainLoop() {
    // Handle keyboard input
    handleInput();
    
    // Calculate delta time for smooth animation
    Uint32 currentTime = SDL_GetTicks();
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;
    
    // Rotate cubes with different speeds
    for (int i = 0; i < NUM_CUBES; i++) {
        float rotX = (i + 1) * 10.0f * deltaTime;  // Reduced rotation speed
        float rotY = (i + 1) * 15.0f * deltaTime;
        float rotZ = (i + 1) * 5.0f * deltaTime;
        
        // Use consistent rotation speeds but different for each cube
        SCG_CubeRotate(&cubes[i], (vec3){rotX, rotY, rotZ});
    }
    
    // Draw all cubes with depth sorting
    SCG_DrawCubes(cubes, NUM_CUBES, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    // Display controls on screen
    // (not implemented in this basic version - would require text rendering)
}

int main() {
    if (!SCG_Init("SCG Cube Demo - WASD to move, IJKL to rotate, R to reset", 
                  WINDOW_WIDTH, WINDOW_HEIGHT)) {
        return 1;
    }
    
    // Set perspective parameters
    SCG_FOV = 80.0f;          // Normal field of view
    SCG_NearPlane = 0.01f;     // Increased near plane to avoid extreme deformation
    SCG_FarPlane = 100.0f;    // Far plane
    
    // Initialize cubes at different positions
    // Position cubes further from the camera to start
    cubes[0] = SCG_NewCube((vec3){0, 0, 5});
    cubes[1] = SCG_NewCube((vec3){-2.5, 0, 7.5});
    cubes[2] = SCG_NewCube((vec3){2.5, 0, 10});
    
    // Initialize time
    lastTime = SDL_GetTicks();
    
    // Start main loop
    SCG_AppLoop(mainLoop);
    
    // Clean up
    SCG_Quit();
    return 0;
}