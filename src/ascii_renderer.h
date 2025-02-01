#ifndef ASCII_RENDERER_H
#define ASCII_RENDERER_H

#include "renderer_types.h"

float deg_to_rad(float degrees);

Model copy_model(Model model);

void free_model(Model model);

void rotate_model(const Model *model, Vertex center,
                  float angle_x, float angle_y, float angle_z);

void ctf_impl_rotate_model_deg(const Model *const model, const Vertex center,
                               const int angle_x, const int angle_y, const int angle_z);

void render_ascii(Model model, RenderConfig config);

void win_capybara();

Vertex calculate_center(Model model);

#endif //ASCII_RENDERER_H
