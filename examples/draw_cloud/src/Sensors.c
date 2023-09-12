#include "Sensors.h"

#include <ouster_clib/sock.h>
#include <ouster_clib/client.h>
#include <ouster_clib/types.h>
#include <ouster_clib/lidar.h>
#include <ouster_clib/mat.h>
#include <ouster_clib/meta.h>
#include <ouster_clib/field.h>
#include <ouster_clib/lut.h>


#include <platform/log.h>
#include <platform/net.h>
#include <platform/fs.h>

#include <viz/Geometries.h>
#include <viz/Pointclouds.h>

#include <flecs.h>

#include <stdlib.h>
#include <stdio.h>

typedef enum 
{
    FIELD_RANGE,
    FIELD_COUNT
} field_t;

typedef enum 
{
    SOCK_INDEX_LIDAR,
    SOCK_INDEX_IMU,
    SOCK_INDEX_COUNT
} sock_index_t;

typedef struct
{
    char const * metafile;
    double * xyz;
    ecs_os_thread_t thread;
} OusterSensor;

ECS_COMPONENT_DECLARE(OusterSensor);
ECS_COMPONENT_DECLARE(OusterSensorDesc);




void * thread_receiver(void * arg)
{
    OusterSensor * sensor = arg;
    char const * metafile = sensor->metafile;


    ouster_meta_t meta = {0};
    ouster_lut_t lut = {0};
    ouster_field_t fields[FIELD_COUNT] = {0};
    ouster_lidar_t lidar = {0};
    int socks[SOCK_INDEX_COUNT] = {0};
    double * xyz;

    
    socks[SOCK_INDEX_LIDAR] = ouster_sock_create_udp_lidar("7502");
    socks[SOCK_INDEX_IMU] = ouster_sock_create_udp_lidar("7503");

    char * content = fs_readfile(metafile);
    ouster_meta_parse(content, &meta);
    free(content);

    ouster_lut_init(&lut, &meta);
    printf("Column window: %i %i\n", meta.mid0, meta.mid1);

    fields[FIELD_RANGE].quantity = OUSTER_QUANTITY_RANGE;
    ouster_field_init(fields, FIELD_COUNT, &meta);

    xyz = calloc(1, lut.w * lut.h * sizeof(double) * 3);

    while(1)
    {
        int timeout_sec = 1;
        int timeout_usec = 0;
        uint64_t a = net_select(socks, SOCK_INDEX_COUNT, timeout_sec, timeout_usec);

        if(a == 0)
        {
            platform_log("Timeout\n");
        }


        if(a & (1 << SOCK_INDEX_LIDAR))
        {
            char buf[NET_UDP_MAX_SIZE];
            int64_t n = net_read(socks[SOCK_INDEX_LIDAR], buf, sizeof(buf));
            if(n <= 0){continue;}
            //printf("n %ji\n", (intmax_t)n);
            ouster_lidar_get_fields(&lidar, &meta, buf, fields, FIELD_COUNT);
            if(lidar.last_mid != meta.mid1) {continue;}
            ouster_mat4_apply_mask_u32(&fields[FIELD_RANGE].mat, fields[FIELD_RANGE].mask);
            ouster_lut_cartesian(&lut, fields[FIELD_RANGE].mat.data, xyz);
            //printf("mat = %i of %i\n", sensor->fields[0].num_valid_pixels, sensor->fields[0].mat.dim[1] * sensor->fields[0].mat.dim[2]);
            ouster_mat4_zero(&fields[FIELD_RANGE].mat);
            printf("New frame %ji\n", (intmax_t)lidar.frame_id);
            /*
            cloud->count = ECS_MIN(cap, lut.w * lut.h);
            for(int j = 0; j < cloud->count; ++j)
            {
                float * dst = cloud->pos + j*3;
                double * src = sensor->xyz + j*3;
                dst[j*3 + 0] = src[j*3 + 0];
                dst[j*3 + 1] = src[j*3 + 1];
                dst[j*3 + 2] = src[j*3 + 2];
                //printf("xyz %f %f %f\n", dst[0], dst[1], dst[2]);
            }
            */
        }


        if(a & (1 << SOCK_INDEX_IMU))
        {
            //char buf[1024*256];
            //int64_t n = net_read(socks[SOCK_INDEX_IMU], buf, sizeof(buf));
            //platform_log("%-10s %5ji:  \n", "SOCK_IMU", (intmax_t)n);
        }
    }
}









void SysUpdateColor(ecs_iter_t *it)
{
    Pointcloud *cloud = ecs_field(it, Pointcloud, 1);
    OusterSensor *sensor = ecs_field(it, OusterSensor, 2);
    //ecs_os_sleep(1,0);
    for(int i = 0; i < it->count; ++i, cloud++, sensor++)
    {

    }
}

static void Observer_LidarUDP_OnAdd(ecs_iter_t *it)
{
    OusterSensorDesc *desc = ecs_field(it, OusterSensorDesc, 1);
    for(int i = 0; i < it->count; ++i, ++desc)
    {
        OusterSensor * sensor = ecs_get_mut(it->world, it->entities[i], OusterSensor);
        sensor->metafile = desc->metafile;
        sensor->thread = ecs_os_thread_new(thread_receiver, sensor);
    }
}

void SensorsImport(ecs_world_t *world)
{
    ECS_MODULE(world, Sensors);
    ECS_IMPORT(world, Geometries);
    ECS_IMPORT(world, Pointclouds);

    ECS_COMPONENT_DEFINE(world, OusterSensor);
    ECS_COMPONENT_DEFINE(world, OusterSensorDesc);

	ecs_struct(world, {
	.entity = ecs_id(OusterSensorDesc),
	.members = {
	{ .name = "metafile", .type = ecs_id(ecs_string_t) },
	}
	});

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
        .callback = Observer_LidarUDP_OnAdd,
        .query.filter.terms =
        {
            { .id = ecs_id(OusterSensorDesc), .src.flags = EcsSelf },
            { .id = ecs_id(OusterSensor), .oper = EcsNot}, // Adds this
        }
    });

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
        .callback = SysUpdateColor,
        .query.filter.terms =
        {
            { .id = ecs_id(Pointcloud), .src.flags = EcsSelf },
            { .id = ecs_id(OusterSensor), .src.trav = EcsIsA, .src.flags = EcsUp},
        }
    });
}