#pragma once

#include "ouster_clib/meta.h"



typedef struct
{
    double * direction;
    double * offset;
} ouster_lut_t;


void ouster_lut_init(ouster_lut_t * lut, ouster_meta_t * meta);
void ouster_lut_fini(ouster_lut_t * lut);

void ouster_lut_cartesian(ouster_lut_t * lut, uint32_t * range, double * xyz, int n);