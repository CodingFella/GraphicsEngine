//
// Created by Jacob Lin on 4/17/24.
//

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

void fill_screen(uint32_t *canvas, size_t width, size_t height, u_int32_t color) {
    for (size_t i = 0; i<width*height; ++i) {
        canvas[i] = color;
    }
}

int write_ppm(const uint32_t *canvas, size_t width, size_t height, const char* file_name) {
    FILE *f = fopen(file_name, "wb");
    // failed
    if (f == NULL) {
        return -1;
    }

    fprintf(f, "P6\n%d %d\n255\n", (int) width, (int) height);

    for (size_t i = 0; i<width*height; ++i) {
        uint32_t current = *(canvas + i);

        uint8_t bytes[3] = {
                (current>>16)&0xFF,
                (current>>8)&0xFF,
                (current)&0xFF
        };

        // printf("Red: %d Green: %d Blue: %d\n", bytes[0], bytes[1], bytes[2]);

        fwrite(bytes, sizeof(bytes), 1, f);
    }

    fclose(f);

    return 0;
}

void draw_point(uint32_t *canvas, int width, int height, int x, int y, uint32_t color) {
    assert(x >= 0 && y >= 0 && x < width && y < height);

    canvas[y*width + x] = color;
}