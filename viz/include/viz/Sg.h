#pragma once

#include <flecs.h>
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_debugtext.h>
#include <sokol/sokol_app.h>
#include <sokol/HandmadeMath.h>

typedef struct
{
	ecs_i32_t dummy;
} SgPipelineCreate;

typedef struct
{
	sg_pipeline id;
} SgPipeline;

typedef struct
{
	ecs_string_t filename_vs;
	ecs_string_t filename_fs;
} SgShaderCreate;

typedef struct
{
	sg_shader id;
} SgShader;

typedef struct
{
	ecs_i32_t index;
} SgLocation;

typedef struct
{
	ecs_i32_t offset;
	ecs_i32_t buffer_index;
} SgAttribute;

typedef struct
{
	ecs_i32_t stride;
	ecs_i32_t step_func;
	ecs_i32_t step_rate;
} SgVertexBufferLayout;

typedef struct
{
	ecs_i32_t dummy;
} SgAttributes;

typedef struct
{
	ecs_i32_t dummy;
} SgUniformBlocks;

typedef struct
{
	sg_vertex_format value;
} SgVertexFormat;

typedef struct
{
	sg_uniform_type value;
} SgUniformType;

typedef struct
{
	sg_index_type value;
} SgIndexType;

typedef struct
{
	sg_primitive_type value;
} SgPrimitiveType;

typedef struct
{
	sg_cull_mode value;
} SgCullMode;

typedef struct
{
	ecs_i32_t index;
	ecs_i32_t size;
} SgUniformBlock;

typedef struct
{
	ecs_i32_t index;
	ecs_i32_t array_count;
} SgUniform;

extern ECS_COMPONENT_DECLARE(SgPipelineCreate);
extern ECS_COMPONENT_DECLARE(SgPipeline);
extern ECS_COMPONENT_DECLARE(SgShaderCreate);
extern ECS_COMPONENT_DECLARE(SgShader);
extern ECS_COMPONENT_DECLARE(SgAttribute);
extern ECS_COMPONENT_DECLARE(SgVertexBufferLayout);
extern ECS_COMPONENT_DECLARE(SgLocation);
extern ECS_COMPONENT_DECLARE(SgAttributes);
extern ECS_COMPONENT_DECLARE(SgUniformBlocks);
extern ECS_COMPONENT_DECLARE(SgVertexFormat);
extern ECS_COMPONENT_DECLARE(SgUniformType);
extern ECS_COMPONENT_DECLARE(SgIndexType);
extern ECS_COMPONENT_DECLARE(SgPrimitiveType);
extern ECS_COMPONENT_DECLARE(SgCullMode);
extern ECS_COMPONENT_DECLARE(SgUniformBlock);
extern ECS_COMPONENT_DECLARE(SgUniform);



extern ECS_TAG_DECLARE(SgNone);
extern ECS_TAG_DECLARE(SgPoints);
extern ECS_TAG_DECLARE(SgLines);
extern ECS_TAG_DECLARE(SgTriangles);
extern ECS_TAG_DECLARE(SgFloat4);
extern ECS_TAG_DECLARE(SgUbyte4n);
extern ECS_TAG_DECLARE(SgU16);
extern ECS_TAG_DECLARE(SgU32);
extern ECS_TAG_DECLARE(SgFront);
extern ECS_TAG_DECLARE(SgBack);

extern ECS_TAG_DECLARE(SgAttributeShapePosition);
extern ECS_TAG_DECLARE(SgAttributeShapeNormal);
extern ECS_TAG_DECLARE(SgAttributeShapeTextcoord);
extern ECS_TAG_DECLARE(SgAttributeShapeColor);
extern ECS_TAG_DECLARE(SgVertexBufferLayoutShape);

void SgImport(ecs_world_t *world);