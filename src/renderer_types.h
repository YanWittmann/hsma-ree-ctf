#ifndef RENDERER_TYPES_H
#define RENDERER_TYPES_H

typedef struct {
    float x, y, z;
} Vertex;

typedef struct {
    int a, b, c;
} Face;

typedef struct {
    Vertex *vertices;
    Face *faces;
    int vertex_count;
    int face_count;
} Model;

typedef struct {
    int x, y;
} Vec2;

typedef struct {
    int width, height;
    float scale_global;
    float scale_x;
    float scale_y;
    Vertex camera_position;
    Vertex light_direction;
} RenderConfig;

#endif //RENDERER_TYPES_H
