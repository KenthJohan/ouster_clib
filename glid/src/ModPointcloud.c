#include "glid/ModPointcloud.h"
#include "sokol_gfx.h"
#include "sokol_gl.h"
#include "sokol_app.h"
#include <math.h>

ECS_COMPONENT_DECLARE(Pointcloud);


typedef struct { float r, g, b; } rgb_t;

static const rgb_t palette[16] = {
    { 0.957f, 0.263f, 0.212f },
    { 0.914f, 0.118f, 0.388f },
    { 0.612f, 0.153f, 0.690f },
    { 0.404f, 0.227f, 0.718f },
    { 0.247f, 0.318f, 0.710f },
    { 0.129f, 0.588f, 0.953f },
    { 0.012f, 0.663f, 0.957f },
    { 0.000f, 0.737f, 0.831f },
    { 0.000f, 0.588f, 0.533f },
    { 0.298f, 0.686f, 0.314f },
    { 0.545f, 0.765f, 0.290f },
    { 0.804f, 0.863f, 0.224f },
    { 1.000f, 0.922f, 0.231f },
    { 1.000f, 0.757f, 0.027f },
    { 1.000f, 0.596f, 0.000f },
    { 1.000f, 0.341f, 0.133f },
};


static rgb_t compute_color(float t) {
    // t is expected to be 0.0f <= t <= 1.0f
    const uint8_t i0 = ((uint8_t)(t * 16.0f)) % 16;
    const uint8_t i1 = (i0 + 1) % 16;
    const float l = fmodf(t * 16.0f, 1.0f);
    const rgb_t c0 = palette[i0];
    const rgb_t c1 = palette[i1];
    return (rgb_t){
        (c0.r * (1.0f - l)) + (c1.r * l),
        (c0.g * (1.0f - l)) + (c1.g * l),
        (c0.b * (1.0f - l)) + (c1.b * l),
    };
}


void Move(ecs_iter_t *it)
{
    int frame_count = (int)sapp_frame_count();
    const float angle = fmodf((float)frame_count, 360.0f);

    sgl_defaults();
    sgl_begin_points();
    float psize = 5.0f;
    for (int i = 0; i < 300; i++) {
        const float a = sgl_rad(angle + i);
        const rgb_t color = compute_color(fmodf((float)(frame_count+i), 300.0f) / 300.0f);
        const float r = sinf(a*4);
        const float s = sinf(a);
        const float c = cosf(a);
        const float x = s * r;
        const float y = c * r;
        sgl_c3f(color.r, color.g, color.b);
        sgl_point_size(psize);
        sgl_v2f(x, y);
        psize *= 1.005f;
    }
    sgl_end();
}

void ModPointcloudImport(ecs_world_t *world)
{
    ECS_MODULE(world, ModPointcloud);
    ECS_COMPONENT_DEFINE(world, Pointcloud);
    ECS_SYSTEM(world, Move, EcsOnUpdate, Pointcloud);
}