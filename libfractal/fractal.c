#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "fractal.h"

struct fractal *fractal_new(const char *name, int width, int height, double a, double b)
{
    fractal *fract = (fractal *) malloc(sizeof(fractal));

    fract->width = width;
    fract->height = height;
    fract->a = a;
    fract->b = b;
    char *fractal_name = (char *) malloc(sizeof(char)*strlen(name));

    fractal_name = strncpy(fractal_name, name, strlen(name));
    fract->name = fractal_name;
    fract->values = (int **) malloc(width*sizeof(*(fract->values)));

    for (int i = 0; i < width; i++) {
        (fract->values)[i] = (int *) malloc(height*sizeof(**(fract->values)));
    }
    return fract;
}

void fractal_free(struct fractal *f)
{
    free(f->name);
    for (int i = 0; i < fractal_get_height(f); i++) {
        free((f->values)[i]);
    }
    free(f->values);
    free(f);
}

const char *fractal_get_name(const struct fractal *f)
{
    return f->name;
}

int fractal_get_value(const struct fractal *f, int x, int y)
{
    return (f->values)[x][y];
}

void fractal_set_value(struct fractal *f, int x, int y, int val)
{
    (f->values)[x][y] = val;
}

int fractal_get_width(const struct fractal *f)
{
    return f->width;
}

int fractal_get_height(const struct fractal *f)
{
    return f->height;
}

void fractal_set_height(fractal *f, int height) {
    f->height = height;
}

double fractal_get_a(const struct fractal *f)
{
    return f->a;
}

double fractal_get_b(const struct fractal *f)
{
    return f->b;
}

double fractal_get_average(const struct fractal *f)
{
    return f->average;
}

void fractal_set_average(struct fractal *f, double average) {
    f->average = average;
}