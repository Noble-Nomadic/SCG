#ifndef SCG_H
#define SCG_H
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>

// Structure for representing a 3D vector (x, y, z)
typedef struct {
    float x, y, z;
} vec3;

// Cube structure with vertices, edges, position, rotation, and velocity
typedef struct {
    vec3 vertices[8];  // 8 vertices of a cube
    int edges[12][2];  // 12 edges (pairs of vertices)
    vec3 position;     // Current position of the cube
    vec3 rotation;     // Current rotation (in degrees)
    vec3 velocity;     // Current velocity of the cube
} Cube;

// Global perspective settings
extern float SCG_FOV;        // Field of View (in degrees)
extern float SCG_NearPlane;  // Near plane distance
extern float SCG_FarPlane;   // Far plane distance

// Function prototypes:
// Creates a new cube at the specified position
Cube SCG_NewCube(vec3 position);

// Rotates the cube by a given rotation delta (in degrees)
void SCG_CubeRotate(Cube* cube, vec3 rotationDelta);

// Moves the cube by a given movement delta
void SCG_CubeMove(Cube* cube, vec3 moveDelta);

// Projects a 3D point to 2D (screen space) with perspective
vec3 SCG_Project(vec3 point, int screenWidth, int screenHeight);

// Draws a cube (using its edges and projected vertices)
void SCG_DrawCube(Cube* cube, int screenWidth, int screenHeight);

// Draws multiple cubes, handling depth sorting automatically
void SCG_DrawCubes(Cube* cubes, int numCubes, int screenWidth, int screenHeight);

// Initializes SDL, window, and renderer
int SCG_Init(const char* title, int width, int height);

// Cleans up SDL resources
void SCG_Quit();

// Clears the screen (sets it to black)
void SCG_Clear();

// Presents the current rendered frame to the window
void SCG_Present();

// Polls and handles SDL events (like window close events)
void SCG_PollEvents();

// Checks if the app is still running (returns 1 if running, 0 if not)
int SCG_IsRunning();

// Main application loop function, calls the provided function on each iteration
void SCG_AppLoop(void (*AppLoopFunction)(void));

// Updates physics (collision, gravity, etc.) for a given cube
void SCG_UpdatePhysics(Cube* cube);

// Sorts cubes by depth (z-values) to render the closest first
void SCG_SortCubesByDepth(Cube* cubes, int numCubes, int screenWidth, int screenHeight);

#endif