#pragma once
#include <flecs.h>
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_shape.h>

typedef struct
{
    sshape_buffer_t buf;
} ShapeBufferImpl;

typedef struct
{
    sshape_element_range_t element;
} ShapeIndex;

extern ECS_COMPONENT_DECLARE(ShapeBufferImpl);
extern ECS_COMPONENT_DECLARE(ShapeIndex);


void GraphicsShapesImport(ecs_world_t *world);