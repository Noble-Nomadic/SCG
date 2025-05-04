#include "SCG.h"
#include <math.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

// Internal renderer
static SDL_Renderer* renderer = NULL;
static SDL_Window* window = NULL;

float SCG_FOV = 90.0f;
float SCG_NearPlane = 0.1f;
float SCG_FarPlane = 100.0f;

// Projection function
vec3 SCG_Project(vec3 p, int screenW, int screenH) {
    float aspectRatio = (float)screenW / (float)screenH;
    float fovRad = 1.0f / tanf((SCG_FOV * 0.5f) * (M_PI / 180.0f));

    // Avoid flipping inside-out
    if (p.z <= 0.01f) p.z = 0.01f;

    return (vec3){
        (p.x * fovRad * aspectRatio) / p.z * screenW * 0.5f + screenW * 0.5f,
        (-p.y * fovRad) / p.z * screenH * 0.5f + screenH * 0.5f,
        p.z
    };
}

// Face normal calculation
vec3 faceNormal(vec3 a, vec3 b, vec3 c) {
    vec3 u = (vec3){b.x - a.x, b.y - a.y, b.z - a.z};
    vec3 v = (vec3){c.x - a.x, c.y - a.y, c.z - a.z};
    return (vec3){
        u.y * v.z - u.z * v.y,
        u.z * v.x - u.x * v.z,
        u.x * v.y - u.y * v.x
    };
}

// Normalize vector
vec3 normalize(vec3 v) {
    float mag = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    return (vec3){v.x / mag, v.y / mag, v.z / mag};
}

// Dot product
float dot(vec3 a, vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Cube face indices
int faceIndices[6][4] = {
    {0, 1, 2, 3},
    {4, 5, 6, 7},
    {0, 1, 5, 4},
    {2, 3, 7, 6},
    {0, 3, 7, 4},
    {1, 2, 6, 5}
};

Cube SCG_NewCube(vec3 pos) {
    Cube c = {.position = pos, .rotation = {0, 0, 0}};
    float s = 1.0f;
    c.vertices[0] = (vec3){-s, -s, -s};
    c.vertices[1] = (vec3){ s, -s, -s};
    c.vertices[2] = (vec3){ s,  s, -s};
    c.vertices[3] = (vec3){-s,  s, -s};
    c.vertices[4] = (vec3){-s, -s,  s};
    c.vertices[5] = (vec3){ s, -s,  s};
    c.vertices[6] = (vec3){ s,  s,  s};
    c.vertices[7] = (vec3){-s,  s,  s};
    return c;
}

// Apply rotation
vec3 rotate(vec3 p, vec3 r) {
    float cosX = cosf(r.x), sinX = sinf(r.x);
    float cosY = cosf(r.y), sinY = sinf(r.y);
    float cosZ = cosf(r.z), sinZ = sinf(r.z);

    vec3 q = p;
    q = (vec3){q.x, cosX * q.y - sinX * q.z, sinX * q.y + cosX * q.z};
    q = (vec3){cosY * q.x + sinY * q.z, q.y, -sinY * q.x + cosY * q.z};
    q = (vec3){cosZ * q.x - sinZ * q.y, sinZ * q.x + cosZ * q.y, q.z};
    return q;
}

void SCG_CubeRotate(Cube* c, vec3 rot) {
    c->rotation.x += rot.x;
    c->rotation.y += rot.y;
    c->rotation.z += rot.z;
}

void SCG_CubeMove(Cube* c, vec3 delta) {
    c->position.x += delta.x;
    c->position.y += delta.y;
    c->position.z += delta.z;
}

void SCG_DrawCube(Cube* cube, int screenW, int screenH) {
    vec3 transformed[8];
    for (int i = 0; i < 8; i++) {
        transformed[i] = rotate(cube->vertices[i], cube->rotation);
        transformed[i].x += cube->position.x;
        transformed[i].y += cube->position.y;
        transformed[i].z += cube->position.z;
    }

    for (int f = 0; f < 6; f++) {
        vec3 a = transformed[faceIndices[f][0]];
        vec3 b = transformed[faceIndices[f][1]];
        vec3 c = transformed[faceIndices[f][2]];
        vec3 normal = normalize(faceNormal(a, b, c));
        vec3 camRay = normalize((vec3){a.x, a.y, a.z});
        if (dot(normal, camRay) < 0.0f) {
            for (int e = 0; e < 4; e++) {
                vec3 p1 = transformed[faceIndices[f][e]];
                vec3 p2 = transformed[faceIndices[f][(e + 1) % 4]];
                vec3 s1 = SCG_Project(p1, screenW, screenH);
                vec3 s2 = SCG_Project(p2, screenW, screenH);
                SDL_RenderDrawLine(renderer, (int)s1.x, (int)s1.y, (int)s2.x, (int)s2.y);
            }
        }
    }
}

void SCG_DrawCubes(Cube* cubes, int count, int screenW, int screenH) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int i = 0; i < count; i++) {
        SCG_DrawCube(&cubes[i], screenW, screenH);
    }

    SDL_RenderPresent(renderer);
}

int SCG_Init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 0;
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    if (!window) return 0;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    return renderer != NULL;
}

void SCG_AppLoop(void (*callback)()) {
    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;
        }
        callback();
    }
}

void SCG_Quit() {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}
