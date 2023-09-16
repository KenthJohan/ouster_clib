#include "viz/Sg.h"
#include "viz/Windows.h"
#include <platform/log.h>
#include <platform/fs.h>
#include <platform/assert.h>


ECS_COMPONENT_DECLARE(SgPipelineCreate);
ECS_COMPONENT_DECLARE(SgPipeline);
ECS_COMPONENT_DECLARE(SgShaderCreate);
ECS_COMPONENT_DECLARE(SgShader);
ECS_COMPONENT_DECLARE(SgAttribute);
ECS_COMPONENT_DECLARE(SgAttributes);
ECS_COMPONENT_DECLARE(SgVertexFormat);
ECS_COMPONENT_DECLARE(SgIndexType);
ECS_COMPONENT_DECLARE(SgPrimitiveType);
ECS_COMPONENT_DECLARE(SgCullMode);

ECS_TAG_DECLARE(SgNone);
ECS_TAG_DECLARE(SgPoints);
ECS_TAG_DECLARE(SgLines);
ECS_TAG_DECLARE(SgTriangles);
ECS_TAG_DECLARE(SgFloat4);
ECS_TAG_DECLARE(SgUbyte4n);
ECS_TAG_DECLARE(SgU16);
ECS_TAG_DECLARE(SgU32);
ECS_TAG_DECLARE(SgFront);
ECS_TAG_DECLARE(SgBack);

#define ENTITY_COLOR "#003366"

void print_entity(ecs_world_t * world, ecs_entity_t e)
{
	char *path_str = ecs_get_fullpath(world, e);
	char *type_str = ecs_type_str(world, ecs_get_type(world, e));
	platform_log("%s [%s]\n", path_str, type_str);
	ecs_os_free(type_str);
	ecs_os_free(path_str);
}


void const * ecsx_get_target_data(ecs_world_t * world, ecs_entity_t e, ecs_entity_t type)
{
	ecs_entity_t target = ecs_get_target(world, e, type, 0);
	if(target == 0){return NULL;}
	return ecs_get_id(world, target, type);
}


void iterate_children(ecs_world_t * world, ecs_entity_t parent)
{
	ecs_iter_t it = ecs_children(world, parent);
	while (ecs_children_next(&it))
	{
		for (int i = 0; i < it.count; i ++)
		{
			ecs_entity_t e = it.entities[i];
			print_entity(world, e);
		}
	}
}


void iterate_shader_attrs(ecs_world_t * world, ecs_entity_t parent, sg_shader_attr_desc * descs)
{
	ecs_iter_t it = ecs_children(world, parent);
	while (ecs_children_next(&it))
	{
		for (int i = 0; i < it.count; i ++)
		{
			ecs_entity_t e = it.entities[i];
			ecs_doc_set_color(world, e, ENTITY_COLOR);
			char const * name = ecs_get_name(world, e);
			ecs_i32_t index = ecs_get(world, e, SgAttribute)->index;
			descs[index].name = name;
		}
	}
}



void iterate_vertex_attrs(ecs_world_t * world, ecs_entity_t parent, sg_vertex_attr_state * descs)
{
	ecs_iter_t it = ecs_children(world, parent);
	while (ecs_children_next(&it))
	{
		for (int i = 0; i < it.count; i ++)
		{
			ecs_entity_t e = it.entities[i];
			ecs_doc_set_color(world, e, ENTITY_COLOR);
			ecs_i32_t index = ecs_get(world, e, SgAttribute)->index;
			SgVertexFormat const * format = ecsx_get_target_data(world, e, ecs_id(SgVertexFormat));
			descs[index].buffer_index = 0;
			descs[index].offset = 0;
			descs[index].format = format->value;
		}
	}
}



void Pip_Create(ecs_iter_t *it)
{
	ecs_world_t * world = it->world;
	//SgPipelineCreate *create = ecs_field(it, SgPipelineCreate, 1); // self
	//SgAttributes * attrs = ecs_field(it, SgAttributes, 2); // up
	ecs_entity_t entity_attrs = ecs_field_src(it, 2);
	SgShader *shader = ecs_field(it, SgShader, 3); // up
	ecs_doc_set_color(world, entity_attrs, ENTITY_COLOR);

	for (int i = 0; i < it->count; ++i)
	{
		ecs_entity_t e = it->entities[i];
		ecs_doc_set_color(world, e, ENTITY_COLOR);
		//print_entity(world, e);

		SgPipeline *pip = ecs_get_mut(world, e, SgPipeline);
		
		SgIndexType const * index_type = ecsx_get_target_data(world, e, ecs_id(SgIndexType));
		SgPrimitiveType const * primitive_type = ecsx_get_target_data(world, e, ecs_id(SgPrimitiveType));
		SgCullMode const * cull_mode = ecsx_get_target_data(world, e, ecs_id(SgCullMode));

		platform_assert_notnull(index_type);
		platform_assert_notnull(primitive_type);
		platform_assert_notnull(cull_mode);

		sg_pipeline_desc desc = {
			.shader = shader->id,
			.depth = {.write_enabled = true, .compare = SG_COMPAREFUNC_LESS_EQUAL},
			.index_type = index_type->value,
			.primitive_type = primitive_type->value,
			.cull_mode = cull_mode->value,
		};
		iterate_vertex_attrs(world, entity_attrs, desc.layout.attrs);
	}
}





static sg_shader create_shader(char *path_fs, char *path_vs)
{
	platform_log("Creating shaders from files %s %s in ", path_fs, path_vs);
	fs_pwd();
	platform_log("\n");
	sg_shader_desc desc = {0};
	desc.attrs[0].name = "position";
	desc.attrs[1].name = "color0";
	desc.vs.source = fs_readfile(path_vs);
	desc.vs.entry = "main";
	desc.vs.uniform_blocks[0].size = 80;
	desc.vs.uniform_blocks[0].layout = SG_UNIFORMLAYOUT_STD140;
	desc.vs.uniform_blocks[0].uniforms[0].name = "vs_params";
	desc.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
	desc.vs.uniform_blocks[0].uniforms[0].array_count = 5;
	desc.fs.source = fs_readfile(path_fs);
	desc.fs.entry = "main";
	desc.label = "primtypes_shader";
	sg_shader shd = sg_make_shader(&desc);
	return shd;
}




// https://github.com/SanderMertens/flecs/blob/ca73ed213310f2ca23f2afde38f72af793091e50/examples/c/entities/hierarchy/src/main.c#L52
void Shader_Create(ecs_iter_t *it)
{
	ecs_world_t * world = it->world;
	SgShaderCreate * desc = ecs_field(it, SgShaderCreate, 1);
	SgAttributes * attrs = ecs_field(it, SgAttributes, 2);
	int self1 = ecs_field_is_self(it, 1);
	ecs_entity_t entity_attrs = ecs_field_src(it, 2);
	ecs_doc_set_color(world, entity_attrs, ENTITY_COLOR);

	for (int i = 0; i < it->count; ++i)
	{
		ecs_entity_t e = it->entities[i];
		ecs_doc_set_color(world, e, "#003366");
		SgShader *shader = ecs_get_mut(world, it->entities[i], SgShader);
		sg_shader_desc desc = {0};
		//shader->id = create_shader(desc->filename_fs, desc->filename_vs);
		iterate_shader_attrs(world, entity_attrs, desc.attrs);
		platform_log("");
	}
}



void SgImport(ecs_world_t *world)
{
	ECS_MODULE(world, Sg);
	ECS_IMPORT(world, Windows);
	ecs_set_name_prefix(world, "Sg");
	ECS_COMPONENT_DEFINE(world, SgPipelineCreate);
	ECS_COMPONENT_DEFINE(world, SgPipeline);
	ECS_COMPONENT_DEFINE(world, SgShaderCreate);
	ECS_COMPONENT_DEFINE(world, SgShader);
	ECS_COMPONENT_DEFINE(world, SgAttribute);
	ECS_COMPONENT_DEFINE(world, SgAttributes);
	ECS_COMPONENT_DEFINE(world, SgVertexFormat);
	ECS_COMPONENT_DEFINE(world, SgIndexType);
	ECS_COMPONENT_DEFINE(world, SgPrimitiveType);
	ECS_COMPONENT_DEFINE(world, SgCullMode);

	ecs_add_id(world, ecs_id(SgVertexFormat), EcsUnion);
	ecs_add_id(world, ecs_id(SgIndexType), EcsUnion);
	ecs_add_id(world, ecs_id(SgPrimitiveType), EcsUnion);
	ecs_add_id(world, ecs_id(SgCullMode), EcsUnion);

	ECS_TAG_DEFINE(world, SgNone);
	ECS_TAG_DEFINE(world, SgPoints);
	ECS_TAG_DEFINE(world, SgLines);
	ECS_TAG_DEFINE(world, SgTriangles);
	ECS_TAG_DEFINE(world, SgFloat4);
	ECS_TAG_DEFINE(world, SgUbyte4n);
	ECS_TAG_DEFINE(world, SgU16);
	ECS_TAG_DEFINE(world, SgU32);
	ECS_TAG_DEFINE(world, SgFront);
	ECS_TAG_DEFINE(world, SgBack);



	ecs_set(world, SgPoints, SgPrimitiveType, {SG_PRIMITIVETYPE_POINTS});
	ecs_set(world, SgLines, SgPrimitiveType, {SG_PRIMITIVETYPE_LINES});
	ecs_set(world, SgTriangles, SgPrimitiveType, {SG_PRIMITIVETYPE_TRIANGLES});

	ecs_set(world, SgFloat4, SgVertexFormat, {SG_VERTEXFORMAT_FLOAT4});
	ecs_set(world, SgUbyte4n, SgVertexFormat, {SG_VERTEXFORMAT_UBYTE4N});

	ecs_set(world, SgNone, SgIndexType, {SG_INDEXTYPE_NONE});
	ecs_set(world, SgU16, SgIndexType, {SG_INDEXTYPE_UINT16});
	ecs_set(world, SgU32, SgIndexType, {SG_INDEXTYPE_UINT32});


	ecs_set(world, SgNone, SgCullMode, {SG_CULLMODE_NONE});
	ecs_set(world, SgFront, SgCullMode, {SG_CULLMODE_FRONT});
	ecs_set(world, SgBack, SgCullMode, {SG_CULLMODE_BACK});


	ecs_struct(world, {.entity = ecs_id(SgPipelineCreate),
		.members = {
			{.name = "dummy", .type = ecs_id(ecs_i32_t)},
		}});

	ecs_struct(world, {.entity = ecs_id(SgShaderCreate),
		.members = {
			{.name = "filename_vs", .type = ecs_id(ecs_string_t)},
			{.name = "filename_fs", .type = ecs_id(ecs_string_t)},
		}});

	ecs_struct(world, {.entity = ecs_id(SgShader),
		.members = {
			{.name = "id", .type = ecs_id(ecs_i32_t)},
		}});

	ecs_struct(world, {.entity = ecs_id(SgAttribute),
		.members = {
			{.name = "index", .type = ecs_id(ecs_i32_t)},
		}});

	ecs_struct(world, {.entity = ecs_id(SgVertexFormat),
		.members = {
			{.name = "value", .type = ecs_id(ecs_i32_t)},
		}});

	ecs_system_init(world, &(ecs_system_desc_t){
		.entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
		.callback = Pip_Create,
		.query.filter.terms =
			{
				{.id = ecs_id(SgPipelineCreate), .src.flags = EcsSelf},
				{.id = ecs_id(SgAttributes), .src.trav = EcsIsA, .src.flags = EcsUp},
				{.id = ecs_id(SgShader), .src.trav = EcsIsA, .src.flags = EcsUp},
				{.id = ecs_id(SgPipeline), .oper = EcsNot}, // Adds this
			}});

	ecs_system_init(world, &(ecs_system_desc_t){
		.entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
		.callback = Shader_Create,
		.query.filter.terms =
			{
				{.id = ecs_id(SgShaderCreate), .src.flags = EcsSelf},
				{.id = ecs_id(SgAttributes), .src.trav = EcsIsA, .src.flags = EcsUp},
				{.id = ecs_id(SgShader), .oper = EcsNot}, // Adds this
			}});

}