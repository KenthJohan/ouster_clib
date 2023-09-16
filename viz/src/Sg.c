#include "viz/Sg.h"
#include "viz/Windows.h"



ECS_COMPONENT_DECLARE(SgPipelineCreate);
ECS_COMPONENT_DECLARE(SgPipeline);
ECS_TAG_DECLARE(SgFormat);
ECS_TAG_DECLARE(SgCullmode);
ECS_TAG_DECLARE(SgFloat4);
ECS_TAG_DECLARE(SgUbyte4n);
ECS_TAG_DECLARE(SgNone);
ECS_TAG_DECLARE(SgPoints);


void Pip_Create(ecs_iter_t *it)
{
	SgPipelineCreate *pipcreate = ecs_field(it, SgPipelineCreate, 1);

	int self1 = ecs_field_is_self(it, 1);

	for (int i = 0; i < it->count; ++i)
	{
		SgPipeline *pip = ecs_get_mut(it->world, it->entities[i], SgPipeline);
		/*
		sg_pipeline_desc pip_desc = {
			.layout = {
				.attrs[0].format = SG_VERTEXFORMAT_FLOAT4,
				.attrs[1].format = SG_VERTEXFORMAT_UBYTE4N,
			},
			.shader = create_shader("../../viz/src/points.fs.glsl", "../../viz/src/points.vs.glsl"),
			.depth = {.write_enabled = true, .compare = SG_COMPAREFUNC_LESS_EQUAL},
			.index_type = SG_INDEXTYPE_NONE,
			.primitive_type = SG_PRIMITIVETYPE_POINTS,
		};
		*/

	}
}


void SgImport(ecs_world_t *world)
{
	ECS_MODULE(world, Sg);
	ECS_IMPORT(world, Windows);
	ecs_set_name_prefix(world, "Sg");

	ECS_COMPONENT_DEFINE(world, SgPipelineCreate);
	ECS_COMPONENT_DEFINE(world, SgPipeline);
	ECS_TAG_DEFINE(world, SgFormat);
	ECS_TAG_DEFINE(world, SgCullmode);
	ECS_TAG_DEFINE(world, SgFloat4);
	ECS_TAG_DEFINE(world, SgUbyte4n);
	ECS_TAG_DEFINE(world, SgNone);
	ECS_TAG_DEFINE(world, SgPoints);

	ecs_struct(world, {.entity = ecs_id(SgPipelineCreate),
		.members = {
			{.name = "dummy", .type = ecs_id(ecs_i32_t)},
		}});


	ecs_system_init(world, &(ecs_system_desc_t){
		.entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
		.callback = Pip_Create,
		.query.filter.terms =
			{
				{.id = ecs_id(SgPipelineCreate), .src.flags = EcsSelf},
				{.id = ecs_id(SgPipeline), .oper = EcsNot}, // Adds this
			}});

}