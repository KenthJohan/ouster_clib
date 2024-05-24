#include "save_csv.h"
#include <stdio.h>


static int snapshot_id = 0;
void save_csv(vertex_t v[], int n) 
{
	snapshot_id++;
	char buf[128];
	sprintf(buf, "snapshot_xyz_%i.csv\n", snapshot_id);
	FILE * f = fopen(buf, "w");
	for(int i = 0; i < n; ++i) {
		fprintf(f, "%f, %f, %f\n", v->x, v->y, v->z);
	}
	fclose(f);
}