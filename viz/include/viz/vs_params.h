#pragma once
#include <sokol/align.h>
#include <sokol/HandmadeMath.h>

#pragma pack(push, 1)
SOKOL_SHDC_ALIGN(16)
typedef struct vs_params_t
{
	hmm_mat4 mvp;
	hmm_vec4 viewport;
} vs_params_t;
#pragma pack(pop)