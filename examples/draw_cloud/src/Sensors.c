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
    int socks[SOCK_INDEX_COUNT];
    ouster_meta_t meta;
    ouster_lut_t lut;
    ouster_field_t fields[FIELD_COUNT];
    ouster_lidar_t lidar;
    double * xyz;
} OusterSensor;

ECS_COMPONENT_DECLARE(OusterSensor);


void SysUpdateColor(ecs_iter_t *it)
{
    Pointcloud *cloud = ecs_field(it, Pointcloud, 1);
    OusterSensor *sensor = ecs_field(it, OusterSensor, 2);
    //ecs_os_sleep(1,0);
    for(int i = 0; i < it->count; ++i, cloud++, sensor++)
    {
        char buf[NET_UDP_MAX_SIZE];
        int64_t n = net_read(sensor->socks[SOCK_INDEX_LIDAR], buf, sizeof(buf));
        if(n <= 0){continue;}
        printf("n %ji\n", (intmax_t)n);
        ouster_lidar_get_fields(&sensor->lidar, &sensor->meta, buf, sensor->fields, FIELD_COUNT);
        if(sensor->lidar.last_mid != sensor->meta.mid1) {continue;}
        ouster_mat4_apply_mask_u32(&sensor->fields[FIELD_RANGE].mat, sensor->fields[FIELD_RANGE].mask);
        ouster_lut_cartesian(&sensor->lut, sensor->fields[FIELD_RANGE].mat.data, sensor->xyz);
        //printf("mat = %i of %i\n", fields[0].num_valid_pixels, fields[0].mat.dim[1] * fields[0].mat.dim[2]);
        ouster_mat4_zero(&sensor->fields[FIELD_RANGE].mat);
        printf("New frame %ji\n", (intmax_t)sensor->lidar.frame_id);
        int nj = ECS_MIN(cloud->count, sensor->lut.w * sensor->lut.h);
        for(int j = 0; j < nj; ++j)
        {
            float * dst = cloud->pos + j*3;
            double * src = sensor->xyz + j*3;
            dst[j*3 + 0] = src[j*3 + 0];
            dst[j*3 + 1] = src[j*3 + 1];
            dst[j*3 + 2] = src[j*3 + 2];
            printf("xyz %f %f %f\n", dst[0], dst[1], dst[2]);
        }
    }
}

void Observer_LidarUDP_OnAdd(ecs_iter_t *it)
{
    OusterSensor *sensor = ecs_field(it, OusterSensor, 1);
    for(int i = 0; i < it->count; ++i, sensor++)
    {
        sensor->socks[SOCK_INDEX_LIDAR] = ouster_sock_create_udp_lidar("7502");
        sensor->socks[SOCK_INDEX_IMU] = ouster_sock_create_udp_lidar("7503");
        char * content = fs_readfile(sensor->metafile);
        ouster_meta_parse(content, &sensor->meta);
        free(content);
        ouster_lut_init(&sensor->lut, &sensor->meta);
        printf("Column window: %i %i\n", sensor->meta.mid0, sensor->meta.mid1);
        sensor->fields[FIELD_RANGE].quantity = OUSTER_QUANTITY_RANGE;
        ouster_field_init(sensor->fields, FIELD_COUNT, &sensor->meta);
        sensor->xyz = calloc(1, sensor->lut.w * sensor->lut.h * sizeof(double) * 3);
    }
}

void SensorsImport(ecs_world_t *world)
{
    ECS_MODULE(world, Sensors);
    ECS_IMPORT(world, Geometries);

    ECS_COMPONENT_DEFINE(world, OusterSensor);

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
        .callback = SysUpdateColor,
        .query.filter = {
            .expr = "Pointcloud, OusterSensor"
        }
    });


    ECS_OBSERVER(world, Observer_LidarUDP_OnAdd, EcsOnSet, OusterSensor);
}