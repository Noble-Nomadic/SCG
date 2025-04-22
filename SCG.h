#ifndef SCG_H
#define SCG_H
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>

// Structure for representing a 3D vector
typedef struct {
    float x, y, z;
} vec3;

// Cube struct with vertices and edges data
typedef struct {
    vec3 vertices[8];  // 8 vertices of a cube
    int edges[12][2];  // 12 edges (pairs of vertices)
    vec3 position;     // Current position
    vec3 rotation;     // Current rotation (in degrees)
} Cube;

// Global perspective settings
extern float SCG_FOV;        // Field of View
extern float SCG_NearPlane;  // Near plane distance
extern float SCG_FarPlane;   // Far plane distance

// Function to create a new cube
Cube SCG_NewCube(vec3 position);

// Function to rotate a cube
void SCG_CubeRotate(Cube* cube, vec3 rotationDelta);

// Function to move a cube
void SCG_CubeMove(Cube* cube, vec3 moveDelta);

// Function to project a 3D point to 2D (screen space) with perspective
vec3 SCG_Project(vec3 point, int screenWidth, int screenHeight);

// Function to draw a cube (using the cube's edges and projected vertices) with perspective
void SCG_DrawCube(Cube* cube, int screenWidth, int screenHeight);

// Function to initialize SDL and the renderer
int SCG_Init(const char* title, int width, int height);

// Function to handle SDL cleanup
void SCG_Quit();

// Function to clear the screen
void SCG_Clear();

// Function to display the rendered frame
void SCG_Present();

// Function to handle events and keep the window open
void SCG_PollEvents();

// Function to check if the app is still running
int SCG_IsRunning();

// Loop for the app to use for all execution of functions
void SCG_AppLoop(void (*AppLoopFunction)(void));

#endif