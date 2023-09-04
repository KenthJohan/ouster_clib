#pragma once

#include "ouster_clib/meta.h"


typedef struct
{
    int w;
    int h;
    double * direction;
    double * offset;
} ouster_lut_t;


void ouster_lut_init(ouster_lut_t * lut, ouster_meta_t const * meta);
void ouster_lut_fini(ouster_lut_t * lut);

void ouster_lut_cartesian(ouster_lut_t const * lut, uint32_t const * range, double * xyz);