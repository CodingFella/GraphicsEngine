//
// Created by Jacob Lin on 4/17/24.
//

#include <stdint.h>
#include <stddef.h>
//#include <stdio.h>

#define RED 0xFF0000FF
#define GREEN 0xFF00FF00
#define BLUE 0xFFFF0000
#define WHITE 0xFFFFFFFF
#define BLACK 0xFF000000
#define TEAL 0xFF008080
#define LILAC 0xFFE6D7FF
#define YELLOW 0xFF00FFFF
#define ORANGE 0xFF00A5FF
#define HOTPINK 0xFFB469FF


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

// todo
void draw_thick_line(uint32_t *canvas, size_t width, size_t height, int x0, int y0, int x1, int y1, int thickness, uint32_t color) {

    for(int i=-thickness/2; i<thickness/2; i++) {
        draw_line(canvas, width, height, x0, y0, x1, y1, color);
    }
}

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




#define DEFAULT_FONT_HEIGHT 6
#define DEFAULT_FONT_WIDTH 6

static char graphic_glyphs[128][DEFAULT_FONT_HEIGHT][DEFAULT_FONT_WIDTH] = {
        ['a'] = {
                {0, 0, 0, 0, 0},
                {0, 1, 1, 0, 0},
                {0, 0, 0, 1, 0},
                {0, 1, 1, 1, 0},
                {1, 0, 0, 1, 0},
                {0, 1, 1, 1, 0},
        },
        ['b'] = {
                {1, 0, 0, 0, 0},
                {1, 1, 1, 0, 0},
                {1, 0, 0, 1, 0},
                {1, 0, 0, 1, 0},
                {1, 0, 0, 1, 0},
                {1, 1, 1, 0, 0},
        },
        ['c'] = {
                {0, 0, 0, 0, 0},
                {0, 1, 1, 0, 0},
                {1, 0, 0, 1, 0},
                {1, 0, 0, 0, 0},
                {1, 0, 0, 1, 0},
                {0, 1, 1, 0, 0},
        },
        ['d'] = {
                {0, 0, 0, 1, 0},
                {0, 1, 1, 1, 0},
                {1, 0, 0, 1, 0},
                {1, 0, 0, 1, 0},
                {1, 0, 0, 1, 0},
                {0, 1, 1, 1, 0},
        },
        ['e'] = {
                {0, 0, 0, 0, 0},
                {0, 1, 1, 0, 0},
                {1, 0, 0, 1, 0},
                {1, 1, 1, 1, 0},
                {1, 0, 0, 0, 0},
                {0, 1, 1, 1, 0},
        },
        ['f'] = {
                {0, 0, 1, 1, 0},
                {0, 1, 0, 0, 0},
                {1, 1, 1, 1, 0},
                {0, 1, 0, 0, 0},
                {0, 1, 0, 0, 0},
                {0, 1, 0, 0, 0},
        },
        ['g'] = {0},
        ['h'] = {
                {1, 0, 0, 0, 0},
                {1, 1, 1, 0, 0},
                {1, 0, 0, 1, 0},
                {1, 0, 0, 1, 0},
                {1, 0, 0, 1, 0},
                {1, 0, 0, 1, 0},
        },
        ['i'] = {
                {0, 0, 1, 0, 0},
                {0, 0, 0, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 1, 0, 0},
        },
        ['j'] = {0},
        ['k'] = {
                {0, 1, 0, 0, 0},
                {0, 1, 0, 0, 0},
                {0, 1, 0, 1, 0},
                {0, 1, 1, 0, 0},
                {0, 1, 1, 0, 0},
                {0, 1, 0, 1, 0},
        },
        ['l'] = {
                {0, 1, 1, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 1, 1, 1, 0},
        },
        ['m'] = {0},
        ['n'] = {0},
        ['o'] = {
                {0, 0, 0, 0, 0},
                {0, 1, 1, 0, 0},
                {1, 0, 0, 1, 0},
                {1, 0, 0, 1, 0},
                {1, 0, 0, 1, 0},
                {0, 1, 1, 0, 0},
        },
        ['p'] = {
                {1, 1, 1, 0, 0},
                {1, 0, 0, 1, 0},
                {1, 0, 0, 1, 0},
                {1, 1, 1, 0, 0},
                {1, 0, 0, 0, 0},
                {1, 0, 0, 0, 0},
        },
        ['q'] = {0},
        ['r'] = {
                {0, 0, 0, 0, 0},
                {1, 0, 1, 1, 0},
                {1, 1, 0, 0, 1},
                {1, 0, 0, 0, 0},
                {1, 0, 0, 0, 0},
                {1, 0, 0, 0, 0},
        },
        ['s'] = {0},
        ['t'] = {0},
        ['u'] = {0},
        ['v'] = {0},
        ['w'] = {
                {0, 0, 0, 0, 0},
                {1, 0, 0, 0, 1},
                {1, 0, 1, 0, 1},
                {1, 0, 1, 0, 1},
                {1, 0, 1, 0, 1},
                {0, 1, 1, 1, 1},
        },
        ['x'] = {0},
        ['y'] = {0},
        ['z'] = {0},

        ['A'] = {0},
        ['B'] = {0},
        ['C'] = {0},
        ['D'] = {0},
        ['E'] = {0},
        ['F'] = {0},
        ['G'] = {0},
        ['H'] = {0},
        ['I'] = {0},
        ['J'] = {0},
        ['K'] = {0},
        ['L'] = {0},
        ['M'] = {0},
        ['N'] = {0},
        ['O'] = {0},
        ['P'] = {0},
        ['Q'] = {0},
        ['R'] = {0},
        ['S'] = {0},
        ['T'] = {0},
        ['U'] = {0},
        ['V'] = {0},
        ['W'] = {0},
        ['X'] = {0},
        ['Y'] = {0},
        ['Z'] = {0},

        ['0'] = {
                {0, 1, 1, 0, 0},
                {1, 0, 0, 1, 0},
                {1, 0, 0, 1, 0},
                {1, 0, 0, 1, 0},
                {1, 0, 0, 1, 0},
                {0, 1, 1, 0, 0},
        },
        ['1'] = {
                {0, 0, 1, 0, 0},
                {0, 1, 1, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 1, 1, 1, 0},
        },
        ['2'] = {
                {0, 1, 1, 0, 0},
                {1, 0, 0, 1, 0},
                {0, 0, 0, 1, 0},
                {0, 1, 1, 0, 0},
                {1, 0, 0, 0, 0},
                {1, 1, 1, 1, 0},
        },
        ['3'] = {
                {0, 1, 1, 0, 0},
                {1, 0, 0, 1, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 0, 1, 0},
                {1, 0, 0, 1, 0},
                {0, 1, 1, 0, 0},
        },
        ['4'] = {
                {0, 0, 1, 1, 0},
                {0, 1, 0, 1, 0},
                {1, 0, 0, 1, 0},
                {1, 1, 1, 1, 1},
                {0, 0, 0, 1, 0},
                {0, 0, 0, 1, 0},
        },
        ['5'] = {
                {1, 1, 1, 0, 0},
                {1, 0, 0, 0, 0},
                {1, 1, 1, 0, 0},
                {0, 0, 0, 1, 0},
                {1, 0, 0, 1, 0},
                {0, 1, 1, 0, 0},
        },
        ['6'] = {
                {0, 1, 1, 0, 0},
                {1, 0, 0, 0, 0},
                {1, 1, 1, 0, 0},
                {1, 0, 0, 1, 0},
                {1, 0, 0, 1, 0},
                {0, 1, 1, 0, 0},
        },
        ['7'] = {
                {1, 1, 1, 1, 0},
                {0, 0, 0, 1, 0},
                {0, 0, 1, 0, 0},
                {0, 1, 0, 0, 0},
                {0, 1, 0, 0, 0},
                {0, 1, 0, 0, 0},
        },
        ['8'] = {
                {0, 1, 1, 0, 0},
                {1, 0, 0, 1, 0},
                {0, 1, 1, 0, 0},
                {1, 0, 0, 1, 0},
                {1, 0, 0, 1, 0},
                {0, 1, 1, 0, 0},

        },
        ['9'] = {
                {0, 1, 1, 0, 0},
                {1, 0, 0, 1, 0},
                {1, 0, 0, 1, 0},
                {0, 1, 1, 1, 0},
                {0, 0, 0, 1, 0},
                {0, 1, 1, 0, 0},
        },

        [','] = {
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0},
                {0, 0, 0, 1, 0},
                {0, 0, 1, 0, 0},
        },

        ['.'] = {
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0},
                {0, 0, 1, 0, 0},
        },
        ['-'] = {
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0},
                {1, 1, 1, 1, 0},
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0},
        },
        [':'] = {
                {0, 0, 0, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 0, 0, 0},
        }
};

#define DIGITAL_FONT_HEIGHT 7
#define DIGITAL_FONT_WIDTH 5


static char digital_glyphs[128][DIGITAL_FONT_HEIGHT][DIGITAL_FONT_WIDTH] = {
        ['1'] = {
                {0, 1, 1, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 1, 1, 1, 0}
        },
        ['2'] = {
                {0, 1, 1, 1, 0},
                {1, 0, 0, 0, 1},
                {0, 0, 0, 1, 0},
                {0, 0, 1, 0, 0},
                {0, 1, 0, 0, 0},
                {1, 0, 0, 0, 0},
                {1, 1, 1, 1, 1}
        },
        ['3'] = {
                {0, 1, 1, 1, 0},
                {1, 0, 0, 0, 1},
                {0, 0, 0, 0, 1},
                {0, 1, 1, 1, 0},
                {0, 0, 0, 0, 1},
                {1, 0, 0, 0, 1},
                {0, 1, 1, 1, 0}
        },
        ['4'] = {
                {0, 0, 0, 1, 0},
                {0, 0, 1, 1, 0},
                {0, 1, 0, 1, 0},
                {1, 0, 0, 1, 0},
                {1, 1, 1, 1, 1},
                {0, 0, 0, 1, 0},
                {0, 0, 0, 1, 0}
        },
        ['5'] = {
                {1, 1, 1, 1, 1},
                {1, 0, 0, 0, 0},
                {1, 0, 0, 0, 0},
                {0, 1, 1, 1, 0},
                {0, 0, 0, 0, 1},
                {0, 0, 0, 0, 1},
                {1, 1, 1, 1, 0}
        },
        ['6'] = {
                {0, 1, 1, 1, 0},
                {1, 0, 0, 0, 1},
                {1, 0, 0, 0, 0},
                {1, 1, 1, 1, 0},
                {1, 0, 0, 0, 1},
                {1, 0, 0, 0, 1},
                {0, 1, 1, 1, 0}
        },
        ['7'] = {
                {1, 1, 1, 1, 1},
                {0, 0, 0, 0, 1},
                {0, 0, 0, 0, 1},
                {0, 0, 0, 1, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 1, 0, 0}
        },
        ['8'] = {
                {0, 1, 1, 1, 0},
                {1, 0, 0, 0, 1},
                {1, 0, 0, 0, 1},
                {0, 1, 1, 1, 0},
                {1, 0, 0, 0, 1},
                {1, 0, 0, 0, 1},
                {0, 1, 1, 1, 0}
        },
        ['9'] = {
                {0, 1, 1, 1, 0},
                {1, 0, 0, 0, 1},
                {1, 0, 0, 0, 1},
                {0, 1, 1, 1, 0},
                {0, 0, 0, 0, 1},
                {1, 0, 0, 0, 1},
                {0, 1, 1, 1, 0}
        },
        ['0'] = {
                {0, 1, 1, 1, 0},
                {1, 0, 0, 0, 1},
                {1, 0, 0, 0, 1},
                {1, 0, 0, 0, 1},
                {1, 0, 0, 0, 1},
                {1, 0, 0, 0, 1},
                {0, 1, 1, 1, 0}
        },
        [':'] = {
                {0, 0, 0, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 0, 0, 0}
        }

};



void draw_rect(uint32_t *canvas, size_t width, size_t height, int x, int y, int w, int h, uint32_t color) {
    if(x + w >= width || y + h >= height) {
        return;
    }

    for(int i=x; i<x+w; i++) {
        draw_line(canvas, width, height, i, y, i, y + h - 1, color);
    }
}

void draw_text(uint32_t *canvas, size_t width, size_t height, const char *text, int tx, int ty, int font_size, int glyph_width, int glyph_height, int spacing, uint32_t color) {
    for (size_t i = 0; *text; ++i, ++text) {
        int char_anchor_x = tx + font_size * i * (glyph_width + spacing);
        int temp = char_anchor_x;
        int char_anchor_y = ty;
        const char *glyph = &digital_glyphs[*text][0][0];


        for(int j=0; j<glyph_height; j++){
            for(int k=0; k<glyph_width; k++) {
//                printf("%d ", glyph[j*glyph_width + k]);
                if(glyph[j*glyph_width + k]) {
                    draw_rect(canvas, width, height, char_anchor_x, char_anchor_y, font_size, font_size, color);
                }
                char_anchor_x += font_size;
            }
            char_anchor_y += font_size;
            char_anchor_x = temp;
//            printf("\n");
        }

    }
}