#include "ascii_renderer.h"

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <stdlib.h>
#include <time.h>

#include "renderer_types.h"
#include "generated/hint_system.h"

// rasterization-based ASCII renderer

// ASCII intensity mapping
const char *ascii_chars = " ..::--===+++***##%%@";

char char_for_intensity(const float intensity) {
    return ascii_chars[(int) (intensity * (float) (strlen(ascii_chars) - 1))];
}

// vector operations
Vertex Vertex_sub(const Vertex v1, const Vertex v2) {
    return (Vertex){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

Vertex Vertex_cross(const Vertex v1, const Vertex v2) {
    return (Vertex){
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    };
}

float Vertex_dot(const Vertex v1, const Vertex v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vertex Vertex_normalize(const Vertex v) {
    const float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return (Vertex){v.x / length, v.y / length, v.z / length};
}

// rendering functions

Vec2 project_vertex(const Vertex vertex, const RenderConfig config) {
    // https://gabrielgambetta.com/computer-graphics-from-scratch/09-perspective-projection.html
    // http://webgl.brown37.net/09_projections/04_projections_perspective_math.html

    // moving the origin of the scene (0,0,0) to the camera by moving all vertex coordinates by the negative camera position
    const Vertex translated_vertex = {
        vertex.x - config.camera_position.x,
        vertex.y - config.camera_position.y,
        vertex.z - config.camera_position.z
    };

    // perspective projection:
    // x' = (x * near) / z
    // y' = (y * near) / z
    // to reduce the amount of calculations, we pre-compute the perspective division factor (1/z) and apply it to x and y
    // I've read to add a small value to z to avoid division by zero, it basically zooms out of the image
    const float perspective = 1.0f / (translated_vertex.z + 5.0f);
    const float x_proj = translated_vertex.x * perspective;
    const float y_proj = translated_vertex.y * perspective;

    // 2D screen space: apply scale and translation from config.
    // we use a global scale for the general scaling, and separate scales for x and y to account for terminal character aspect ratio (2:1) to de-stretch the output
    return (Vec2){
        (int) (x_proj * config.scale_global * config.scale_x + config.width / 2),
        (int) (-y_proj * config.scale_global * config.scale_y + config.height / 2)
    };
}

float edge_function(const Vec2 v0, const Vec2 v1, const Vec2 p) {
    return (float) ((p.x - v0.x) * (v1.y - v0.y) - (p.y - v0.y) * (v1.x - v0.x));
}

void rasterize_triangle(
    char **const buffer, float **const z_buffer,
    const Vec2 v0, const Vec2 v1, const Vec2 v2,
    const float z0, const float z1, const float z2,
    const char ascii_char, const RenderConfig config
) {
    const int min_x = fmax(0, fmin(v0.x, fmin(v1.x, v2.x)));
    const int max_x = fmin(config.width - 1, fmax(v0.x, fmax(v1.x, v2.x)));
    const int min_y = fmax(0, fmin(v0.y, fmin(v1.y, v2.y)));
    const int max_y = fmin(config.height - 1, fmax(v0.y, fmax(v1.y, v2.y)));

    Vec2 p;
    const float area = edge_function(v0, v1, v2);
    for (p.y = min_y; p.y <= max_y; p.y++) {
        for (p.x = min_x; p.x <= max_x; p.x++) {
            const float w0 = edge_function(v1, v2, p) / area;
            const float w1 = edge_function(v2, v0, p) / area;
            const float w2 = edge_function(v0, v1, p) / area;
            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                const float z = w0 * z0 + w1 * z1 + w2 * z2;
                if (z < z_buffer[p.y][p.x]) {
                    z_buffer[p.y][p.x] = z;
                    buffer[p.y][p.x] = ascii_char;
                }
            }
        }
    }
}

// rotation matrices, simple 3D rotation around axis

void rotate_x(Vertex *const vertex, const float angle) {
    const float cos_angle = cos(angle);
    const float sin_angle = sin(angle);
    const float y = vertex->y * cos_angle - vertex->z * sin_angle;
    const float z = vertex->y * sin_angle + vertex->z * cos_angle;
    vertex->y = y;
    vertex->z = z;
}

void rotate_y(Vertex *const vertex, const float angle) {
    const float cos_angle = cos(angle);
    const float sin_angle = sin(angle);
    const float x = vertex->x * cos_angle + vertex->z * sin_angle;
    const float z = -vertex->x * sin_angle + vertex->z * cos_angle;
    vertex->x = x;
    vertex->z = z;
}

void rotate_z(Vertex *const vertex, const float angle) {
    const float cos_angle = cos(angle);
    const float sin_angle = sin(angle);
    const float x = vertex->x * cos_angle - vertex->y * sin_angle;
    const float y = vertex->x * sin_angle + vertex->y * cos_angle;
    vertex->x = x;
    vertex->y = y;
}

Vertex calculate_center(const Model model) {
    Vertex center = {0, 0, 0};
    for (int i = 0; i < model.vertex_count; i++) {
        center.x += model.vertices[i].x;
        center.y += model.vertices[i].y;
        center.z += model.vertices[i].z;
    }
    center.x /= model.vertex_count;
    center.y /= model.vertex_count;
    center.z /= model.vertex_count;
    return center;
}

void rotate_model(const Model *const model, const Vertex center,
                  const float angle_x, const float angle_y, const float angle_z) {
    // since we want to rotate around a center, we can drastically reduce the complexity of the operation by
    // - moving the model to the origin
    // - applying the rotation
    // - moving it back to its original position

    for (int i = 0; i < model->vertex_count; i++) {
        model->vertices[i].x -= center.x;
        model->vertices[i].y -= center.y;
        model->vertices[i].z -= center.z;
    }

    for (int i = 0; i < model->vertex_count; i++) {
        if (angle_x != 0) rotate_x(&model->vertices[i], angle_x);
        if (angle_y != 0) rotate_y(&model->vertices[i], angle_y);
        if (angle_z != 0) rotate_z(&model->vertices[i], angle_z);
    }

    for (int i = 0; i < model->vertex_count; i++) {
        model->vertices[i].x += center.x;
        model->vertices[i].y += center.y;
        model->vertices[i].z += center.z;
    }
}

void ctf_impl_rotate_model_deg(const Model *const model, const Vertex center,
                               int angle_x, int angle_y, int angle_z) {
    note("HNT{1185e13dd020}");

    angle_x = abs(angle_x) % 90 - 45;
    angle_y = abs(angle_y) % 360;
    angle_z = abs(angle_z) % 90 - 45;
#ifdef DEBUG_LOGGING
    printf("[x,y,z] clamped: %d %d %d\n", angle_x, angle_y, angle_z);
#endif
    rotate_model(model, center, deg_to_rad(angle_x), deg_to_rad(angle_y), deg_to_rad(angle_z));
}

void render_ascii(const Model model, const RenderConfig config) {
    note("HNT{1185e13dd020}");

    // memory for the double display buffer
    char *display_buffer = malloc((config.height * (config.width + 1) + 1) * sizeof(char));
    char **const buffer = malloc(config.height * sizeof(char *));
    float **const z_buffer = malloc(config.height * sizeof(float *));

    for (int y = 0; y < config.height; y++) {
        buffer[y] = malloc((config.width + 1) * sizeof(char));
        z_buffer[y] = malloc(config.width * sizeof(float));
        memset(buffer[y], ' ', config.width);
        buffer[y][config.width] = '\0';
        for (int x = 0; x < config.width; x++) {
            z_buffer[y][x] = FLT_MAX;
        }
    }

    const Vertex light_dir = Vertex_normalize(config.light_direction);

    for (int i = 0; i < model.face_count; i++) {
        const Vertex v0 = model.vertices[model.faces[i].a];
        const Vertex v1 = model.vertices[model.faces[i].b];
        const Vertex v2 = model.vertices[model.faces[i].c];

        const Vertex edge1 = Vertex_sub(v1, v0);
        const Vertex edge2 = Vertex_sub(v2, v0);
        const Vertex normal = Vertex_normalize(Vertex_cross(edge1, edge2));

        const float intensity = fmax(0, Vertex_dot(normal, light_dir));
        const char ascii_char = char_for_intensity(intensity);

        const Vec2 sv0 = project_vertex(v0, config);
        const Vec2 sv1 = project_vertex(v1, config);
        const Vec2 sv2 = project_vertex(v2, config);

        rasterize_triangle(
            buffer, z_buffer,
            sv0, sv1, sv2,
            v0.z, v1.z, v2.z,
            ascii_char, config
        );
    }

    char *ptr = display_buffer;
    for (int y = 0; y < config.height; y++) {
        memcpy(ptr, buffer[y], config.width);
        ptr += config.width;
        *(ptr++) = '\n';
        free(buffer[y]);
        free(z_buffer[y]);
    }
    *ptr = '\0';

    printf("%s", display_buffer);

    free(buffer);
    free(z_buffer);
    free(display_buffer);
}

float deg_to_rad(const float degrees) {
    return degrees * M_PI / 180.0f;
}

Model copy_model(const Model model) {
    Model copy;
    copy.vertex_count = model.vertex_count;
    copy.face_count = model.face_count;
    copy.vertices = malloc(copy.vertex_count * sizeof(Vertex));
    copy.faces = malloc(copy.face_count * sizeof(Face));
    memcpy(copy.vertices, model.vertices, copy.vertex_count * sizeof(Vertex));
    memcpy(copy.faces, model.faces, copy.face_count * sizeof(Face));
    return copy;
}

void free_model(const Model model) {
    free(model.vertices);
    free(model.faces);
}

#include "generated/model_data.h"
#include "win.h"

void win_capybara() {
    const Model use_model = copy_model(reference_model);
    const Vertex center = calculate_center(use_model);

    const RenderConfig config = {
        .width = 90,
        .height = 40,
        .scale_global = 90.0f,
        .scale_x = 2.0f,
        .scale_y = 1.0f,
        .camera_position = {0, 0, 1},
        .light_direction = {-0.5f, 0, -1}
    };

    float angle_x = deg_to_rad(5);
    float angle_y = deg_to_rad(130);
    float angle_z = deg_to_rad(0);
    const float rotation_speed = deg_to_rad(5);

    for (int frame = 0; frame < 215; frame++) {
        // copy original model for this frame
        Model frame_model = copy_model(use_model);
        rotate_model(&frame_model, center, angle_x, angle_y, angle_z);

        // clear console
        printf("\033[2J\033[H");
        printf("\033[1;33m");
        render_ascii(frame_model, config);
        printf("\033[0m");
        printf("\033[1;96m");
        printf("[frame %d] [x %f] [y %f] [z %f]\n", frame, angle_x, angle_y, angle_z);
        printf("\033[0m");
        win();
        printf("\n");
        free_model(frame_model);

        angle_x += rotation_speed;
        angle_y += rotation_speed / 2;
        angle_z += rotation_speed / 3;

        // 50 ms = 20 fps
        nanosleep((struct timespec[]){{0, 50000000}}, NULL);
    }

    free_model(use_model);
}
