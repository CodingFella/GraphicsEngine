#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>

#include <time.h>

#include "graphi.c"

#define WIDTH 100
#define HEIGHT 100




static uint32_t pixels[WIDTH*HEIGHT];

int main() {
    fill_screen(pixels, WIDTH, HEIGHT, WHITE);


    clock_t start, end;
    double cpu_time_used;

    start = clock();

    uint32_t current_color = LILAC;
//    for(int i=WIDTH; i>= 0; i -= 5) {
//        draw_full_circle(pixels, WIDTH, HEIGHT, 200, 200, i, current_color);
//        if((i/5) % 2 == 0) {
//            current_color = WHITE;
//        }
//        else {
//            current_color = LILAC;
//        }
//    }
    //draw_full_circle(pixels, WIDTH, HEIGHT, 250, 250, 250, RED);
    //draw_circle(pixels, WIDTH, HEIGHT, 5, 5, 5, LILAC);



//    fill_flat_bottom_triangle(pixels, WIDTH, HEIGHT,
//                  0, 0, 30, 40, 20, 90, RED);

    fill_triangle(pixels, WIDTH, HEIGHT, 0, 0, 30, 40, 20, 90, LILAC);

    //draw_triangle(pixels, WIDTH, HEIGHT, 0, 0, 30, 40, 20, 90, RED);

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time taken: %f\n seconds", cpu_time_used);

    //write_ppm(pixels, WIDTH, HEIGHT, "out3.ppm");



    return 0;
}
