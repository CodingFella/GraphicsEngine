//
// Created by Jacob Lin on 4/17/24.
//

#include <stdint.h>
#include <stddef.h>

#define RED 0xFF0000FF
#define GREEN 0xFF00FF00
#define BLUE 0xFFFF0000
#define WHITE 0xFFFFFFFF
#define BLACK 0xFF000000
#define TEAL 0xFF008080
#define LILAC 0xFFE6D7FF
#define YELLOW 0xFF00FFFF
#define ORANGE 0xFF00A5FF


#define GRAPHIC_SWAP(T, a, b) do { T t = a; a = b; b = t; } while(0)


void fill_screen(uint32_t *canvas, size_t width, size_t height, uint32_t color) {
    for (size_t i = 0; i<width*height; ++i) {
        canvas[i] = color;
    }
}

//int write_ppm(const uint32_t *canvas, size_t width, size_t height, const char* file_name) {
//    FILE *f = fopen(file_name, "wb");
//    // failed
//    if (f == NULL) {
//        return -1;
//    }
//
//    fprintf(f, "P6\n%d %d\n255\n", (int) width, (int) height);
//
//    for (size_t i = 0; i<width*height; ++i) {
//        uint32_t current = *(canvas + i);
//
//        uint8_t bytes[3] = {
//                (current>>16)&0xFF,
//                (current>>8)&0xFF,
//                (current)&0xFF
//        };
//
//        // printf("Red: %d Green: %d Blue: %d\n", bytes[0], bytes[1], bytes[2]);
//
//        fwrite(bytes, sizeof(bytes), 1, f);
//    }
//
//    fclose(f);
//
//    return 0;
//}

void draw_point(uint32_t *canvas, size_t width, size_t height, int x, int y, uint32_t color) {
    if(x< 0 || y < 0 || x >= width || y >= height) {
        return;
    }

    canvas[y*width + x] = color;
}

int min(int x1, int x2) {
    if(x1 < x2) {
        return x1;
    }
    return x2;
}

int max(int x1, int x2) {
    if(x1 > x2) {
        return x1;
    }
    return x2;
}

int abs (int x1) {
    if (x1 < 0) return -x1;
    return x1;
}

// draws line
// Input: pointer to canvas array, width and height of canvas, initial point (x0, y0), final point (x1, y1), color
// Output: void
void draw_line(uint32_t *canvas, size_t width, size_t height, int x0, int y0, int x1, int y1, uint32_t color) {
//    if(x0< 0 || y0 < 0 || x0 >= width || y0 >= height) {
//        return;
//    }
//    if(x1< 0 || y1 < 0 || x1 >= width || y1 >= height) {
//        return;
//    }
    int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2; /* error value e_xy */

    for (;;){  /* loop */
        //canvas[x0 + y0*width] = color;
        draw_point(canvas, width, height, x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
        if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
    }

}
// TODO: AA lines https://en.wikipedia.org/wiki/Xiaolin_Wu%27s_line_algorithm


// helper function for draw_circle
void draw_8_points(uint32_t *canvas, int width, int height, int cx, int cy, int x, int y, uint32_t color) {
    draw_point(canvas, width, height, cx + x, cy + y, color);
    draw_point(canvas, width, height, cx + x, cy - y, color);
    draw_point(canvas, width, height, cx - x, cy + y, color);
    draw_point(canvas, width, height, cx - x, cy - y, color);
    draw_point(canvas, width, height, cx + y, cy + x, color);
    draw_point(canvas, width, height, cx + y, cy - x, color);
    draw_point(canvas, width, height, cx - y, cy + x, color);
    draw_point(canvas, width, height, cx - y, cy - x, color);
}


void draw_circle(uint32_t *canvas, int width, int height, int cx, int cy, int r, uint32_t color) {
    int error = -r;
    int x = r;
    int y = 0;

    while (y <= x) {
        draw_8_points(canvas, width, height, cx, cy, x, y, color);
        error += (y << 1) + 1;
        ++y;
        if(error >= 0) {
            error -= (x << 1) - 1;
            x -= 1;
        }
    }
}

// helper function for draw_full_circle
void draw_4_lines(uint32_t *canvas, int width, int height, int cx, int cy, int x, int y, uint32_t color) {
    draw_line(canvas, width, height, cx + x, cy + y, cx + x, cy - y, color);
    draw_line(canvas, width, height, cx - x, cy + y, cx - x, cy - y, color);
    draw_line(canvas, width, height, cx + y, cy + x, cx + y, cy - x, color);
    draw_line(canvas, width, height, cx - y, cy + x, cx - y, cy - x, color);
}

void draw_full_circle(uint32_t *canvas, int width, int height, int cx, int cy, int r, uint32_t color) {
    int error = -r;
    int x = r;
    int y = 0;

    while (y <= x) {
        draw_4_lines(canvas, width, height, cx, cy, x, y, color);
        error += (y << 1) + 1;
        ++y;
        if(error >= 0) {
            error -= (x << 1) - 1;
            x -= 1;
        }
    }
}




void sort_points(int *x1, int *y1, int *x2, int *y2, int *x3, int *y3) {

    // p1 p2 p3
    if(*y2 < *y1) {
        GRAPHIC_SWAP(int, *y1, *y2);
        GRAPHIC_SWAP(int, *x1, *x2);
    }

    if(*y3 < *y2) {
        GRAPHIC_SWAP(int, *y2, *y3);
        GRAPHIC_SWAP(int, *x2, *x3);

    }

    if(*y2 < *y1) {
        GRAPHIC_SWAP(int, *y1, *y2);
        GRAPHIC_SWAP(int, *x1, *x2);

    }
}


void draw_triangle(uint32_t *canvas, size_t width, size_t height,
                   int x1, int y1,
                   int x2, int y2,
                   int x3, int y3, uint32_t color) {
    draw_line(canvas, width, height, x1, y1, x2, y2, color);
    draw_line(canvas, width, height, x1, y1, x3, y3, color);
    draw_line(canvas, width, height, x3, y3, x2, y2, color);

}


//             * (x1, y1)
//           *      *
//         *            *
//       * (x2, y2) ******** (x3, y3)

void fill_flat_bottom_triangle(uint32_t *canvas, size_t width, size_t height,
                               int x1, int y1,
                               int x2, int y2,
                               int x3, int y3, uint32_t color) {

    float slope1_2 = (float)(x2 - x1) / (float)(y2 - y1);
    float slope1_3 = (float)(x3 - x1) / (float)(y3 - y1);

    float curr_x_left = (float)x1;
    float curr_x_right = (float)x1;

    for(int curr_y = y1; curr_y <= y2; curr_y++) {
        draw_line(canvas, width, height, (int)(curr_x_left - 0.5), (int)(curr_y), (int)(curr_x_right+0.5), (int)(curr_y), color);

        curr_x_left += slope1_2;
        curr_x_right += slope1_3;
    }

}


//       * (x1, y1) ******** (x2, y3)
//         *            *
//           *      *
//             * (x3, y3)

void fill_flat_top_triangle(uint32_t *canvas, size_t width, size_t height,
                            int x1, int y1,
                            int x2, int y2,
                            int x3, int y3, uint32_t color) {

    float slope1_3 = (float)(x3 - x1) / (float)(y3 - y1);
    float slope2_3 = (float)(x3 - x2) / (float)(y3 - y2);

    float curr_x_left = (float)x3;
    float curr_x_right = (float)x3;

    for(int curr_y = y3; curr_y > y1; curr_y--) {
        draw_line(canvas, width, height, (int)(curr_x_left), curr_y, (int)(curr_x_right+0.5), curr_y, color);

        curr_x_left -= slope1_3;
        curr_x_right -= slope2_3;
    }

}

void fill_triangle(uint32_t *canvas, size_t width, size_t height,
                   int x1, int y1,
                   int x2, int y2,
                   int x3, int y3, uint32_t color) {

    sort_points(&x1, &y1, &x2, &y2, &x3, &y3);


    // p1 < p2 < p3



    int mid_x = (int)((float)x1 + ((float)(y2 - y1) / (float)(y3 - y1)) * (float)(x3 - x1));

    fill_flat_bottom_triangle(canvas, width, height, x1, y1, x2, y2, mid_x, y2, color);
    fill_flat_top_triangle(canvas, width, height, x2, y2, mid_x, y2, x3, y3, color);
}