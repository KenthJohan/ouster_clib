#pragma once

#define M4(i, j) ((i) * 4 + (j))
#define M3(i, j) ((i) * 3 + (j))
void mul3(double *r, double const *a, double const *x);

void m4_print(double const *a);

void m3_print(double const *a);

void v3_print(double const *a);
