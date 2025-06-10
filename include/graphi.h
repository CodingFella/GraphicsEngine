#ifndef GRAPHI_H
#define GRAPHI_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RED     0xFF0000FF
#define GREEN   0xFF00FF00
#define BLUE    0xFFFF0000
#define WHITE   0xFFFFFFFF
#define BLACK   0xFF000000
#define TEAL    0xFF008080
#define LILAC   0xFFE6D7FF
#define YELLOW  0xFF00FFFF
#define ORANGE  0xFF00A5FF
#define HOTPINK 0xFFB469FF

void fill_screen(uint32_t *canvas, size_t width, size_t height, uint32_t color);
void draw_point(uint32_t *canvas, size_t width, size_t height, int x, int y, uint32_t color);
void draw_line(uint32_t *canvas, size_t width, size_t height, int x0, int y0, int x1, int y1, uint32_t color);
void draw_thick_line(uint32_t *canvas, size_t width, size_t height, int x0, int y0, int x1, int y1, int thickness, uint32_t color);
void draw_circle(uint32_t *canvas, size_t width, size_t height, int cx, int cy, int r, uint32_t color);
void draw_full_circle(uint32_t *canvas, int width, int height, int cx, int cy, int r, uint32_t color);
void draw_triangle(uint32_t *canvas, size_t width, size_t height, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);
void fill_triangle(uint32_t *canvas, size_t width, size_t height, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);
void draw_rect(uint32_t *canvas, size_t width, size_t height, int x, int y, int w, int h, uint32_t color);
void graphi_draw_text(uint32_t *canvas, size_t width, size_t height, const char *text, int tx, int ty, int font_size, int spacing, uint32_t color);

#define DEFAULT_FONT_HEIGHT 6
#define DEFAULT_FONT_WIDTH 6

#define DIGITAL_FONT_HEIGHT 7
#define DIGITAL_FONT_WIDTH 5

#ifdef __cplusplus
}
#endif

#endif // GRAPHI_H
