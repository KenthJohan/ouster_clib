#include "ouster_clib.h"
#include <string.h>

void ouster_vec_init(ouster_vec_t *v, int esize, int cap)
{
	ouster_assert_notnull(v);
	ouster_assert(cap >= 0, "");
	ouster_assert(esize >= 0, "");
	v->data = malloc(cap * esize);
	ouster_assert_notnull(v->data);
	v->cap = cap;
	v->esize = esize;
}

void ouster_vec_append(ouster_vec_t *v, void const *data, int n, float factor)
{
	ouster_assert_notnull(v);
	ouster_assert_notnull(data);
	ouster_assert(n >= 0, "");
	ouster_assert(factor >= 1.0f, "");
	int count = v->count + n;
	if (count > v->cap) {
		int cap = (float)count * factor;
		v->data = realloc(v->data, cap);
		ouster_assert_notnull(v->data);
		ouster_assert(count <= cap, "");
		v->cap = cap + 1;
	}
	int offset = v->esize * v->count;
	ouster_assert(offset >= 0, "");
	ouster_assert((offset + n) <= v->cap, "");
	memcpy(OUSTER_OFFSET(v->data, offset), data, n);
	v->count = count;
}

void test_ouster_vec()
{
	char const *d1 = "Hello";
	char const *d2 = " ";
	char const *d3 = "world!";
	ouster_vec_t v;
	ouster_vec_init(&v, 1, 2);
	ouster_vec_append(&v, d1, strlen(d1), 1.5f);
	ouster_vec_append(&v, d2, strlen(d2), 1.5f);
	ouster_vec_append(&v, d3, strlen(d3), 1.5f);

	char const *str = v.data;
	int diff = strcmp("Hello world!", str);
	ouster_assert(diff == 0, "");
}