#include <glid/glid.h>
#include <glid/ModPointcloud.h>
#include <flecs.h>
#include <stdio.h>
#include <stdlib.h>

#include <ouster_clib/sock.h>
#include <ouster_clib/client.h>
#include <ouster_clib/log.h>
#include <ouster_clib/types.h>
#include <ouster_clib/lidar.h>
#include <ouster_clib/mat.h>
#include <ouster_clib/meta.h>
#include <ouster_clib/field.h>
#include <ouster_clib/lut.h>


#include <platform/net.h>
#include <platform/fs.h>

typedef enum 
{
    FIELD_RANGE,
    FIELD_COUNT
} field_t;

typedef struct
{
    int fd;
    ouster_meta_t meta;
    ouster_lut_t lut;
    ouster_mat4_t coords;
    ouster_field_t fields[FIELD_COUNT];
    ouster_lidar_t lidar;
    double * xyz;
} LidarUDP;
ECS_COMPONENT_DECLARE(LidarUDP);


void SysUpdateColor(ecs_iter_t *it)
{
    Pointcloud *clouds = ecs_field(it, Pointcloud, 1);
    LidarUDP *item = ecs_field(it, LidarUDP, 2);
    //ecs_os_sleep(1,0);
    for(int i = 0; i < it->count; ++i, clouds++, item++)
    {
        char buf[1024*100];
        int64_t n = net_read(item->fd, buf, sizeof(buf));
        if(n > 0)
        {
            printf("n %ji\n", (intmax_t)n);
            ouster_lidar_get_fields(&item->lidar, &item->meta, buf, item->fields, FIELD_COUNT);
            if(item->lidar.last_mid == item->meta.column_window[1])
            {
                ouster_mat4_apply_mask_u32(&item->fields[FIELD_RANGE].mat, item->fields[FIELD_RANGE].mask);
                ouster_lut_cartesian(&item->lut, item->fields[FIELD_RANGE].mat.data, item->xyz);
                //printf("mat = %i of %i\n", fields[0].num_valid_pixels, fields[0].mat.dim[1] * fields[0].mat.dim[2]);
                ouster_mat4_zero(&item->fields[FIELD_RANGE].mat);
                printf("New frame %ji\n", (intmax_t)item->lidar.frame_id);

                int nj = ECS_MIN(clouds->count, item->lut.w * item->lut.h);
                for(int j = 0; j < nj; ++j)
                {
                    double * pos = (double*)(clouds->pos + (j * clouds->pos_step));
                    double * xyz = item->xyz + (j * 3);
                    pos[0] = xyz[0];
                    pos[1] = xyz[1];
                    pos[2] = xyz[2];
                    printf("xyz %f %f %f\n", pos[0], pos[1], pos[2]);
                }
            }
        }
    }
}

void Observer_LidarUDP_OnAdd(ecs_iter_t *it)
{
    LidarUDP *item = ecs_field(it, LidarUDP, 1);
    ecs_os_memset_n(item, 0, LidarUDP, it->count);
    //ecs_os_sleep(1,0);
    for(int i = 0; i < it->count; ++i, item++)
    {
        item->fd = ouster_sock_create_udp_lidar("7502");
        char * content = ouster_os_file_read("../in.json");
        item->coords.dim[0] = sizeof(double);
        item->coords.dim[1] = 3;
        item->coords.dim[2] = 3;
        ouster_mat4_init(&item->coords);
        ouster_meta_parse(content, &item->meta);
        free(content);
        ouster_lut_init(&item->lut, &item->meta);
        printf("Column window: %i %i\n", item->meta.column_window[0], item->meta.column_window[1]);
        item->fields[FIELD_RANGE].quantity = OUSTER_QUANTITY_RANGE;
        ouster_field_init(item->fields, FIELD_COUNT, &item->meta);
        item->xyz = calloc(1, item->lut.w * item->lut.h * sizeof(double) * 3);
    }
}




int main(int argc, char* argv[])
{
    ecs_world_t * world = ecs_init_w_args(argc, argv);
    ecs_set_threads(world, 4);

    ECS_IMPORT(world, ModPointcloud);
    ECS_COMPONENT_DEFINE(world, LidarUDP);

    ECS_OBSERVER(world, Observer_LidarUDP_OnAdd, EcsOnAdd, LidarUDP);

    ecs_entity_t s = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)}}),
        .callback = SysUpdateColor,
        .query.filter = {
            .expr = "Pointcloud, LidarUDP"
        },
        .multi_threaded = false,
        .no_readonly = true
    });



    {
        ecs_entity_t e = ecs_new(world, 0);
        ecs_add(world, e, Pointcloud);
        ecs_add(world, e, LidarUDP);
    }



    glid_state_t state = 
    {
        .world = world
    };
    glid_init(&state);


    return ecs_fini(world);
}