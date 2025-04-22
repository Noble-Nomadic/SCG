#include "SCG.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Initialize global perspective settings
float SCG_FOV = 60.0f;        // Default FOV
float SCG_NearPlane = 0.01f;   // Default near plane
float SCG_FarPlane = 100.0f;  // Default far plane

// Internal SDL variables
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static int running = 1;

// Global variables to store window dimensions
static int windowWidth = 800;
static int windowHeight = 600;

// Convert degrees to radians
static float degToRad(float degrees) {
    return degrees * (M_PI / 180.0f);
}

// Rotate a point around the X axis
static vec3 rotateX(vec3 point, float angle) {
    float rad = degToRad(angle);
    vec3 result;
    result.x = point.x;
    result.y = point.y * cos(rad) - point.z * sin(rad);
    result.z = point.y * sin(rad) + point.z * cos(rad);
    return result;
}

// Rotate a point around the Y axis
static vec3 rotateY(vec3 point, float angle) {
    float rad = degToRad(angle);
    vec3 result;
    result.x = point.x * cos(rad) + point.z * sin(rad);
    result.y = point.y;
    result.z = -point.x * sin(rad) + point.z * cos(rad);
    return result;
}

// Rotate a point around the Z axis
static vec3 rotateZ(vec3 point, float angle) {
    float rad = degToRad(angle);
    vec3 result;
    result.x = point.x * cos(rad) - point.y * sin(rad);
    result.y = point.x * sin(rad) + point.y * cos(rad);
    result.z = point.z;
    return result;
}

// Function to create a new cube at a given position
Cube SCG_NewCube(vec3 position) {
    Cube cube = {
        .vertices = {
            {-1, -1, -1}, {1, -1, -1},
            {1, 1, -1}, {-1, 1, -1},
            {-1, -1, 1}, {1, -1, 1},
            {1, 1, 1}, {-1, 1, 1}
        },
        .edges = {
            {0, 1}, {1, 2}, {2, 3}, {3, 0},
            {4, 5}, {5, 6}, {6, 7}, {7, 4},
            {0, 4}, {1, 5}, {2, 6}, {3, 7}
        },
        .position = position,
        .rotation = {0, 0, 0}
    };
    return cube;
}

// Function to rotate a cube by a given vector
void SCG_CubeRotate(Cube* cube, vec3 rotationDelta) {
    cube->rotation.x += rotationDelta.x;
    cube->rotation.y += rotationDelta.y;
    cube->rotation.z += rotationDelta.z;
}

// Function to move a cube by a given vector
void SCG_CubeMove(Cube* cube, vec3 moveDelta) {
    cube->position.x += moveDelta.x;
    cube->position.y += moveDelta.y;
    cube->position.z += moveDelta.z;
}

// Function to project a 3D point to 2D (screen space) with perspective
vec3 SCG_Project(vec3 point, int screenWidth, int screenHeight) {
    // Make sure we're not dividing by zero or negative values
    if (point.z <= SCG_NearPlane) point.z = SCG_NearPlane;
    
    // Calculate aspect ratio
    float aspectRatio = (float)screenWidth / (float)screenHeight;
    
    // Calculate projection factors
    float fovRad = degToRad(SCG_FOV);
    float fovFactor = 1.0f / tanf(fovRad / 2.0f);
    
    // Apply perspective projection
    float xProjected = point.x * fovFactor;
    float yProjected = point.y * fovFactor;
    
    // Apply perspective division (z-division happens AFTER applying FOV)
    xProjected /= point.z;
    yProjected /= point.z;
    
    // Apply aspect ratio correction only to the x-axis
    xProjected *= aspectRatio;
    
    // Scale to screen dimensions and center
    int x = (int)((xProjected * screenHeight / 2) + screenWidth / 2);
    int y = (int)((-yProjected * screenHeight / 2) + screenHeight / 2); // Flip y-axis
    
    return (vec3){x, y, point.z};
}


// Function to draw a cube (using the cube's edges and projected vertices) with perspective
void SCG_DrawCube(Cube* cube, int screenWidth, int screenHeight) {
    // If dimensions are 0, use stored window dimensions
    if (screenWidth == 0 || screenHeight == 0) {
        screenWidth = windowWidth;
        screenHeight = windowHeight;
    }
    
    // Set line color to white for visibility
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    // Create an array for transformed vertices
    vec3 transformedVertices[8];
    
    // Apply transformations to all vertices
    for (int i = 0; i < 8; i++) {
        // Start with the original vertex
        vec3 v = cube->vertices[i];
        
        // Apply rotations
        v = rotateX(v, cube->rotation.x);
        v = rotateY(v, cube->rotation.y);
        v = rotateZ(v, cube->rotation.z);
        
        // Apply translation
        v.x += cube->position.x;
        v.y += cube->position.y;
        v.z += cube->position.z;
        
        // Store the transformed vertex
        transformedVertices[i] = v;
    }
    
    // Draw each edge
    for (int i = 0; i < 12; i++) {
        vec3 start = SCG_Project(transformedVertices[cube->edges[i][0]], screenWidth, screenHeight);
        vec3 end = SCG_Project(transformedVertices[cube->edges[i][1]], screenWidth, screenHeight);
        
        // Draw line between start and end points
        // Only draw edges if both vertices are in front of the near plane
		if (start.z > SCG_NearPlane && end.z > SCG_NearPlane) {
		    SDL_RenderDrawLine(renderer, (int)start.x, (int)start.y, (int)end.x, (int)end.y);
		}
    }
}

// Initialize SDL, window, and renderer
int SCG_Init(const char* title, int width, int height) {
    // Store the window dimensions globally
    windowWidth = width;
    windowHeight = height;
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL Init Error: %s", SDL_GetError());
        return 0;
    }
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    if (!window) return 0;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return 0;
    return 1;
}

// Cleanup SDL resources
void SCG_Quit() {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

// Clear the screen
void SCG_Clear() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

// Present the current frame to the window
void SCG_Present() {
    SDL_RenderPresent(renderer);
}

// Poll for SDL events (like closing the window)
void SCG_PollEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) running = 0;
    }
}

// Check if the app is still running
int SCG_IsRunning() {
    return running;
}

// Main loop function that clears the screen and allows the user app to execute functions
void SCG_AppLoop(void (*AppLoopFunction)(void)) {
    while (SCG_IsRunning()) {
        SCG_PollEvents();     // Handle input/quit
        SCG_Clear();          // Clear screen

        AppLoopFunction();    // Call user's render/update function

        SCG_Present();        // Render to screen

        SDL_Delay(16);        // Cap to ~60 FPS
    }
}