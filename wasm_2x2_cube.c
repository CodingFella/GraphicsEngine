//
// Created by Jacob Lin on 5/7/24.
//

// clang --target=wasm32 --no-standard-libraries -Wl,--export-all -Wl,--no-entry -o wasm_cube.wasm wasm_cube.c

#include "graphi.c"
#include "math.c"

#define WIDTH 800
#define HEIGHT 600

#define NUM_PLANES 6
#define PLANE_POINTS 4
#define NUM_CORNERS 8

// 1.2 0.1 is nice
#define SCALE 1.2
#define GAP 0.1

#define BG_COLOR BLACK

#define DIMENSION 10

// order goes as this: p0123, p5140, p4062, p5476, p5173, p7362
#define PLANE0 0b00100000
#define PLANE1 0b00010000
#define PLANE2 0b00001000
#define PLANE3 0b00000100
#define PLANE4 0b00000010
#define PLANE5 0b00000001

#define TESTPLANE 0b01000000

struct Point_3D {
    float x, y, z;
};

struct Point_2D {
    float x, y;
};

struct plane {
    struct Point_3D pointA;
    struct Point_3D pointB;
    struct Point_3D pointC;
    struct Point_3D pointD;

    struct Point_2D s_pointA;
    struct Point_2D s_pointB;
    struct Point_2D s_pointC;
    struct Point_2D s_pointD;

    uint32_t color;
};

struct cube {
    struct Point_3D points[NUM_CORNERS];
    uint8_t planesToDraw;
};

float get_average_z(struct plane input) {
    return (input.pointA.z + input.pointB.z + input.pointC.z + input.pointD.z) / (float)4.0;
}

float get_average_z_corners(struct Point_3D* input) {
    float sum = 0;
    for(int i=0; i<8; i++) {
        sum += input[i].z;
    }
    return sum;

}


void draw_planes(uint32_t *canvas, int width, int height,
                 struct plane* p, int count) {
    // sort planes based on average z value

    int i, j, min_idx;

    // selection sort
    for (i = 0; i < count-1; i++)
    {
        // Find the minimum element in unsorted array
        min_idx = i;
        for (j = i+1; j < count; j++)
            if (get_average_z(p[j]) > get_average_z(p[min_idx]))
                min_idx = j;

        // Swap the found minimum element with the first element
        if(min_idx != i) {
            struct plane temp = p[min_idx];
            p[min_idx] = p[i];
            p[i] = temp;
        }
    }

    // paint accordingly
    for(i = 3; i < 6; i++) {
        fill_triangle(canvas, width, height,
                      (int)(p[i].s_pointA.x+0.5), (int)(p[i].s_pointA.y+0.5),
                      (int)(p[i].s_pointB.x+0.5), (int)(p[i].s_pointB.y+0.5),
                      (int)(p[i].s_pointC.x+0.5), (int)(p[i].s_pointC.y+0.5),
                      p[i].color);

        fill_triangle(canvas, width, height,
                      (int)(p[i].s_pointD.x+0.5), (int)(p[i].s_pointD.y+0.5),
                      (int)(p[i].s_pointB.x+0.5), (int)(p[i].s_pointB.y+0.5),
                      (int)(p[i].s_pointC.x+0.5), (int)(p[i].s_pointC.y+0.5),
                      p[i].color);

    }
}


void draw_cube(uint32_t *canvas, int width, int height, struct Point_3D* points, int num_corners, struct Point_3D camera, uint32_t color, int planesToConsider) {
    struct Point_2D c_2D[num_corners];
    for (size_t i = 0; i < num_corners; ++i) {
        struct Point_3D curr = points[i];
        float u, v;
        float x, y, z;
        x = curr.x - camera.x;
        y = curr.y - camera.y;
        z = curr.z - camera.z;


        u = (x / z) * 3000.0 + WIDTH / 2.0;
        v = (y / z) * 3000.0 + HEIGHT / 2.0;

        //draw_point(canvas, width, height, u, v, color);

        struct Point_2D p = {u, v};
        c_2D[i] = p;
    }

    struct plane p0123 = {points[0], points[1], points[2], points[3], c_2D[0], c_2D[1], c_2D[2], c_2D[3], BLUE};
    struct plane p5140 = {points[5], points[1], points[4], points[0], c_2D[5], c_2D[1], c_2D[4], c_2D[0],
                          YELLOW};
    struct plane p4062 = {points[4], points[0], points[6], points[2], c_2D[4], c_2D[0], c_2D[6], c_2D[2], RED};
    struct plane p5476 = {points[5], points[4], points[7], points[6], c_2D[5], c_2D[4], c_2D[7], c_2D[6],
                          GREEN};
    struct plane p5173 = {points[5], points[1], points[7], points[3], c_2D[5], c_2D[1], c_2D[7], c_2D[3],
                          ORANGE};
    struct plane p7362 = {points[7], points[3], points[6], points[2], c_2D[7], c_2D[3], c_2D[6], c_2D[2],
                          WHITE};

    struct plane planes[NUM_PLANES] = {p0123, p5140, p4062, p5476, p5173, p7362};
    int curr_plane = PLANE0;
    for (int i = 0; i < NUM_PLANES; i++) {
        if ((planesToConsider & curr_plane) != 0) {
            planes[i].color = BG_COLOR;
        }
        if((planesToConsider & TESTPLANE)) {
            planes[i].color = LILAC;
        }
        curr_plane = curr_plane >> 1;
    }




    draw_planes(canvas, WIDTH, HEIGHT, planes, NUM_PLANES);


}

float A, B, C;

// used to calculate linear transformation
// Pre-condition: coordinates of corner in computer's 3D model
// Post-condition: returns new coordinate based on current angle
float calculateX(float i, float j, float k) {
    return j * sin(A) * sin(B) * cos(C) - k * cos(A) * sin(B) * cos(C) +
           j * cos(A) * sin(C) + k * sin(A) * sin(C) + i * cos(B) * cos(C);
}

float calculateY(float i, float j, float k) {
    return j * cos(A) * cos(C) + k * sin(A) * cos(C) -
           j * sin(A) * sin(B) * sin(C) + k * cos(A) * sin(B) * sin(C) -
           i * cos(B) * sin(C);
}

float calculateZ(float i, float j, float k) {
    return k * cos(A) * cos(B) - j * sin(A) * cos(B) + i * sin(B);
}


static uint32_t pixels[WIDTH*HEIGHT];

void f_modulo(float* x, float y) {
    *x =  *x - y * (float)(int)(*x/y);
}
float angle = 0;


// creates corners of a cube
// Precondition: canvas, length of cube, distance of camera, offset in x, y, and z (default centered at (0,0,0))
// Postcondition: places corners into points, no return value
void generateCorners(struct cube *cube, float magnitude, float camera_distance, float x_offset, float y_offset, float z_offset) {

    float x_factor = -1;
    float y_factor = -1;
    float z_factor = -1;

    for(int i=0; i<8; i++){
        struct Point_3D newPoint;
        // to get all combinations! like truth table thing to get all combos
        x_factor *= -1;
        if((i % 2) == 0) y_factor *= -1;
        if((i % 4) == 0) z_factor *= -1;

        newPoint.x = x_factor * magnitude + x_offset;
        newPoint.y = y_factor * magnitude + y_offset;
        newPoint.z = camera_distance + z_factor * magnitude + z_offset;

        cube->points[i] = newPoint;
    }

}

void generateCubes(float magnitude, float camera_distance) {
    int i, j, k;

    const int cubes_rows = DIMENSION;
    int cubes_cols = DIMENSION;
    int cubes_height = DIMENSION;
    int num_cubes = (DIMENSION == 1) ? 1 : DIMENSION * DIMENSION * DIMENSION - (DIMENSION-2) * (DIMENSION-2) * (DIMENSION-2);
    int num_points = 8;
//    struct Point_3D points[num_cubes][num_points];

    struct cube cubes[num_cubes];

    for(i=0; i < num_cubes; i++) {
        cubes[i].planesToDraw = 0;
    }

    float spacing = (float)(SCALE*2);

    float x_offset = -spacing*((DIMENSION-1)/(float)2);
    float y_offset = -spacing*((DIMENSION-1)/(float)2);
    float z_offset = -spacing*((DIMENSION-1)/(float)2);
    int count = 0;

    // stickers!
    for(i=0; i<cubes_rows; i++) {
        for(j=0; j<cubes_cols; j++) {
            for(k=0; k<cubes_height; k++) {
                if(i == 0 || i == cubes_rows - 1 ||
                    j == 0 || j == cubes_cols - 1 ||
                    k == 0 || k == cubes_height - 1) {
                    generateCorners(&cubes[count], magnitude, camera_distance, x_offset + (spacing * (float) i),
                                    y_offset + (spacing * (float) j), z_offset + (spacing * (float) k));

                    // assigns each cube with which planes are active
                    if(i == 0) cubes[count].planesToDraw += PLANE4;
                    if(i == DIMENSION - 1) cubes[count].planesToDraw += PLANE2;


                    if(j == 0) cubes[count].planesToDraw += PLANE5;
                    if(j == DIMENSION - 1) cubes[count].planesToDraw += PLANE1;

                    if(k == 0) cubes[count].planesToDraw += PLANE3;
                    if(k == DIMENSION - 1) cubes[count].planesToDraw += PLANE0;

                    if(i==0 && j==1 && k==0) cubes[count].planesToDraw += TESTPLANE;

                    count++;
                }
            }
        }
    }

    // rotate corners for each cube
    for(i=0; i<num_cubes; i++) {
        for(j=0; j<NUM_CORNERS; j++) {
            struct Point_3D new_point = {calculateX(cubes[i].points[j].x, cubes[i].points[j].y, cubes[i].points[j].z - camera_distance),
                    calculateY(cubes[i].points[j].x, cubes[i].points[j].y, cubes[i].points[j].z - camera_distance),
                    calculateZ(cubes[i].points[j].x, cubes[i].points[j].y, cubes[i].points[j].z - camera_distance) + camera_distance};
            cubes[i].points[j] = new_point;
        }
    }

    struct Point_3D camera = {0, 0, 0};

    // draw the cubes!
    // selection sort
    int min_idx;
    for (i = 0; i < num_cubes-1; i++)
    {
        // Find the minimum element in unsorted array
        min_idx = i;
        for (j = i+1; j < num_cubes; j++)
            if (get_average_z_corners(cubes[j].points) > get_average_z_corners(cubes[min_idx].points))
                min_idx = j;

        // Swap the found minimum element with the first element
        struct Point_3D temp;
        uint8_t temp_num;
        if(min_idx != i) {
            for(j=0; j < NUM_CORNERS; j++) {
                temp = cubes[min_idx].points[j];
                cubes[min_idx].points[j] = cubes[i].points[j];
                cubes[i].points[j] = temp;
            }

            temp_num = cubes[min_idx].planesToDraw;
            cubes[min_idx].planesToDraw = cubes[i].planesToDraw;
            cubes[i].planesToDraw = temp_num;

        }
    }

    for(i=0; i<num_cubes; i++) {
        draw_cube(pixels, WIDTH, HEIGHT, cubes[i].points, 8, camera, RED, ~cubes[i].planesToDraw);
    }
}

uint32_t* render(int dt, float a, float b, float c) {
    dt = dt % 360;
    angle = PI * dt / 180.0;

    fill_screen(pixels, WIDTH, HEIGHT, BG_COLOR);

    A = a;
    B = b;
    C = c;

    f_modulo(&A, 2*(float)PI);
    f_modulo(&B, 2*(float)PI);
    f_modulo(&C, 2*(float)PI);

    float magnitude = SCALE - GAP;
    float camera_distance = 200;


    generateCubes(magnitude, camera_distance);

    return pixels;
}




void *memcpy(void *dest, const void *src, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        ((char*)dest)[i] = ((char*)src)[i];
    }
}

int main(void)
{
    float magnitude = SCALE - GAP;
    float camera_distance = 200;


    generateCubes(magnitude, camera_distance);

    return 0;
}
