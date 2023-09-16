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
	sg_pipeline pip;
} SgPipeline;

typedef struct
{
	sg_shader shader;
} SgShader;

extern ECS_COMPONENT_DECLARE(SgPipelineCreate);
extern ECS_COMPONENT_DECLARE(SgPipeline);
extern ECS_TAG_DECLARE(SgFormat);
extern ECS_TAG_DECLARE(SgCullmode);
extern ECS_TAG_DECLARE(SgFloat4);
extern ECS_TAG_DECLARE(SgUbyte4n);
extern ECS_TAG_DECLARE(SgNone);
extern ECS_TAG_DECLARE(SgPoints);

void SgImport(ecs_world_t *world);