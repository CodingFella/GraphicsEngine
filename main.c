#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>

#include <time.h>

#include "graphi.c"

#define WIDTH 800
#define HEIGHT 600


static uint32_t pixels[WIDTH*HEIGHT];



int main() {
    fill_screen(pixels, WIDTH, HEIGHT, WHITE);


    clock_t start, end;
    double cpu_time_used;

    start = clock();

    uint32_t current_color = LILAC;

    fill_screen(pixels, WIDTH, HEIGHT, current_color);

//    draw_rect(pixels, WIDTH, HEIGHT, 5, 5, 5, 5, RED);

    //draw_text(pixels, WIDTH, HEIGHT, "3.14159", 5, 5, 1, 5, 7, RED);
    draw_text(pixels, WIDTH, HEIGHT, "123", 5, 20, 8, 5, 7, 1, RED);
    draw_text(pixels, WIDTH, HEIGHT, "12:34:56", 5, 200, 15, 5, 7, 1, RED);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time taken: %f\n seconds", cpu_time_used);

    write_ppm(pixels, WIDTH, HEIGHT, "out3.ppm");


    return 0;
}
