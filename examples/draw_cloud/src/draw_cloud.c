#include <viz/viz.h>
#include <viz/Geometries.h>
#include <viz/Renderings.h>

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

#include <flecs.h>

#include <stdlib.h>
#include <stdio.h>

typedef enum 
{
    FIELD_RANGE,
    FIELD_COUNT
} field_t;

typedef struct
{
    char const * metafile;
    int fd;
    ouster_meta_t meta;
    ouster_lut_t lut;
    ouster_field_t fields[FIELD_COUNT];
    ouster_lidar_t lidar;
    double * xyz;
} LidarUDP;

ECS_COMPONENT_DECLARE(LidarUDP);


void SysUpdateColor(ecs_iter_t *it)
{
    Pointcloud *cloud = ecs_field(it, Pointcloud, 1);
    LidarUDP *lidar = ecs_field(it, LidarUDP, 2);
    //ecs_os_sleep(1,0);
    for(int i = 0; i < it->count; ++i, cloud++, lidar++)
    {
        char buf[1024*100];
        int64_t n = net_read(lidar->fd, buf, sizeof(buf));
        if(n > 0)
        {
            printf("n %ji\n", (intmax_t)n);
            ouster_lidar_get_fields(&lidar->lidar, &lidar->meta, buf, lidar->fields, FIELD_COUNT);
            if(lidar->lidar.last_mid == lidar->meta.mid1)
            {
                ouster_mat4_apply_mask_u32(&lidar->fields[FIELD_RANGE].mat, lidar->fields[FIELD_RANGE].mask);
                ouster_lut_cartesian(&lidar->lut, lidar->fields[FIELD_RANGE].mat.data, lidar->xyz);
                //printf("mat = %i of %i\n", fields[0].num_valid_pixels, fields[0].mat.dim[1] * fields[0].mat.dim[2]);
                ouster_mat4_zero(&lidar->fields[FIELD_RANGE].mat);
                printf("New frame %ji\n", (intmax_t)lidar->lidar.frame_id);

                int nj = ECS_MIN(cloud->count, lidar->lut.w * lidar->lut.h);
                for(int j = 0; j < nj; ++j)
                {
                    float * dst = cloud->pos + j*3;
                    double * src = lidar->xyz + j*3;
                    dst[j*3 + 0] = src[j*3 + 0];
                    dst[j*3 + 1] = src[j*3 + 1];
                    dst[j*3 + 2] = src[j*3 + 2];
                    printf("xyz %f %f %f\n", dst[0], dst[1], dst[2]);
                }
            }
        }
    }
}

void Observer_LidarUDP_OnAdd(ecs_iter_t *it)
{
    LidarUDP *item = ecs_field(it, LidarUDP, 1);
    for(int i = 0; i < it->count; ++i, item++)
    {
        item->fd = ouster_sock_create_udp_lidar("7502");
        char * content = fs_readfile(item->metafile);
        ouster_meta_parse(content, &item->meta);
        free(content);
        ouster_lut_init(&item->lut, &item->meta);
        printf("Column window: %i %i\n", item->meta.mid0, item->meta.mid1);
        item->fields[FIELD_RANGE].quantity = OUSTER_QUANTITY_RANGE;
        ouster_field_init(item->fields, FIELD_COUNT, &item->meta);
        item->xyz = calloc(1, item->lut.w * item->lut.h * sizeof(double) * 3);
    }
}




int main(int argc, char* argv[])
{
    fs_pwd();
    
    if(argc <= 1)
    {
        printf("Missing input meta file\n");
        return 0;
    }

    ecs_world_t * world = ecs_init();
    //https://www.flecs.dev/explorer/?remote=true
	ecs_set(world, EcsWorld, EcsRest, {.port = 0});
	//ecs_plecs_from_file(world, "./src/config.flecs");

    //ecs_set_threads(world, 4);

    ECS_IMPORT(world, Geometries);
    ECS_COMPONENT_DEFINE(world, LidarUDP);

    ECS_OBSERVER(world, Observer_LidarUDP_OnAdd, EcsOnSet, LidarUDP);


    ecs_entity_t s = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
        .callback = SysUpdateColor,
        .query.filter = {
            .expr = "Pointcloud, LidarUDP"
        }
    });
    



    {
        ecs_entity_t e = ecs_new_entity(world, "Hello");
        ecs_set(world, e, Pointcloud, {.count = 300});
        //char const * metafile = argv[1];
        //ecs_set(world, e, LidarUDP, {.metafile = metafile});
    }

    while(0)
    {
        ecs_progress(world, 0);
    }

    viz_state_t state = 
    {
        .world = world
    };
    viz_init(&state);


    return ecs_fini(world);
}