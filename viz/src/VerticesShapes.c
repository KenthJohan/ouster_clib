#include "viz/VerticesShapes.h"
#include "viz/Geometries.h"


#include "vendor/sokol_gfx.h"
#include "vendor/sokol_shape.h"

#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include <platform/fs.h>
#include <platform/log.h>



typedef struct
{
    int cap;
    sshape_buffer_t buf;
} VerticesShapesBuffer;

typedef struct
{
    sshape_element_range_t element;
} DrawDesc;

ECS_COMPONENT_DECLARE(VerticesShapesBuffer);


void Setup(ecs_iter_t *it)
{
    VerticesShapesBuffer *buffer = ecs_field(it, VerticesShapesBuffer, 1);

    for(int i = 0; i < it->count; ++i, ++buffer)
    {
        sshape_vertex_t vertices[6 * 1024];
        uint16_t indices[16 * 1024];
        sshape_buffer_t buf = {
            .vertices.buffer = SSHAPE_RANGE(vertices),
            .indices.buffer  = SSHAPE_RANGE(indices),
        };
        
        buf = sshape_build_sphere(&buf, &(sshape_sphere_t) {
            .radius = 0.10f,
            .slices = 5,
            .stacks = 3,
            .random_colors = true,
        });
        sshape_element_range_t element = sshape_element_range(&buf);
    }
}

void AddShape(ecs_iter_t *it)
{
    VerticesShapesBuffer *buffer = ecs_field(it, VerticesShapesBuffer, 1); // up
    DrawDesc *draw = ecs_field(it, DrawDesc, 2);

    for(int i = 0; i < it->count; ++i, ++draw)
    {
        buffer->buf = sshape_build_sphere(&buffer->buf, &(sshape_sphere_t) {
            .radius = 0.10f,
            .slices = 5,
            .stacks = 3,
            .random_colors = true,
        });
        draw->element = sshape_element_range(&buffer->buf);
    }
}

void VerticesShapesImport(ecs_world_t *world)
{
    ECS_MODULE(world, Renderings);
    ECS_IMPORT(world, Geometries);
    

    ECS_COMPONENT_DEFINE(world, VerticesShapesBuffer);


}