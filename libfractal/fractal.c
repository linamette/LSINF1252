#include <stdlib.h>
#include <stdio.h>
#include "fractal.h"

struct fractal *fractal_new(const char *name, int width, int height, double a, double b)
{
    struct fractal *fract = (struct fractal*) malloc(sizeof(struct fractal));
    if (fract == NULL)
    {
        printf("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    fract->value[width][height];
    fract->name = name;
    fract->width = width;
    fract->height = height;
    fract->a = a;
    fract->b = b;
    
    return fract;
}

void fractal_free(struct fractal *f)
{
    free(f);
}

const char *fractal_get_name(const struct fractal *f)
{
	return f->name;
}

int fractal_get_value(const struct fractal *f, int x, int y)
{
	return f->value[x][y];
}

void fractal_set_value(struct fractal *f, int x, int y, int val)
{
    f->value[x][y] = val;
}

int fractal_get_width(const struct fractal *f)
{
    /* TODO */
    return 0;
}

int fractal_get_height(const struct fractal *f)
{
    /* TODO */
    return 0;
}

double fractal_get_a(const struct fractal *f)
{
    /* TODO */
    return 0;
}

double fractal_get_b(const struct fractal *f)
{
    /* TODO */
    return 0;
}
