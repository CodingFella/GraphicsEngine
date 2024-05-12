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

//#define DIMENSION 8

#define MODE_LENGTH_I 0
#define MODE_WIDTH_J 1
#define MODE_HEIGHT_K 2

#define FLT_MAX 3.402823466e+38F /* max value */
#define FLT_MIN (-FLT_MAX) /* min positive value */

// order goes as this: p0123, p5140, p4062, p5476, p5173, p7362
#define PLANE0 0b00100000
#define PLANE1 0b00010000
#define PLANE2 0b00001000
#define PLANE3 0b00000100
#define PLANE4 0b00000010
#define PLANE5 0b00000001

#define MAX_DIMENSION 10
#define MAX_CUBES (MAX_DIMENSION * MAX_DIMENSION * MAX_DIMENSION - (MAX_DIMENSION-2) * (MAX_DIMENSION-2) * (MAX_DIMENSION-2))

#define TESTPLANE 0b01000000

int DIMENSION = 8;

float A, B, C;
static uint32_t pixels[WIDTH*HEIGHT + MAX_CUBES];


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
    return (input.pointA.z + input.pointB.z + input.pointC.z + input.pointD.z) / (float)NUM_PLANES;
}

float get_average_z_corners(struct Point_3D* input) {
    float sum = 0;
    for(int i=0; i<NUM_CORNERS; i++) {
        sum += input[i].z;
    }
    return sum;

}

// todo: write comments
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

/*
 *  Function:   convert_3D_to_2D
 *  ---------------------
 *  Given the dimensions of the canvas and the set of 3D points, relative to the camera,
 *      it converts the 3D coordinates and places its equivalent 2D points into the 2D
 *      array.
 *
 *  Input params:
 *      struct Point_2D* point2D:   pointer to store the converted 2D points
 *      int width:                  width of canvas
 *      int height:                 height of canvas
 *      struct Point_3D* points:    pointer to 3D points to be converted
 *      int num_corners:            number of corners
 *      struct Point_3D camera:
 *
 *  Return:
 *      No return value. Computed cubes are stored in cubes and the proper image is
 *          drawn into the canvas.
 */
void convert_3D_to_2D(struct Point_2D* point2D, int width, int height, struct Point_3D* points, struct Point_3D camera) {

    for (size_t i = 0; i < NUM_CORNERS; ++i) {
        struct Point_3D curr = points[i];
        float u, v;
        float x, y, z;
        x = curr.x - camera.x;
        y = curr.y - camera.y;
        z = curr.z - camera.z;


        u = (x / z) * 3000.0 + width / 2.0;
        v = (y / z) * 3000.0 + height / 2.0;

        struct Point_2D p = {u, v};
        point2D[i] = p;
    }
}

/*
 *  Function:   draw_cube
 *  ---------------------
 *  Given the canvas, the corners of the cube, the camera, face colors, and planes to
 *      consider, this function draws the cube to its required specifications.
 *
 *  Input params:
 *      struct cube* cubes:     array of cubes that the generated cubes will be stored in
 *      int num_cubes:          number of cubes in the Rubik's Cube
 *      float magnitude:        how long the dimensions of each cube is
 *      float camera_distance:  how far away the camera is from the cube
 *
 *  Return:
 *      No return value. Computed cubes are stored in cubes and the proper image is
 *          drawn into the canvas.
 */
void draw_cube(uint32_t *canvas, int width, int height, struct Point_3D* points,
        struct Point_3D camera, uint32_t* colors, int planesToConsider) {
    struct Point_2D c_2D[NUM_CORNERS];

    convert_3D_to_2D(c_2D, width, height, points, camera);

    struct plane p0123 = {points[0], points[1], points[2], points[3], c_2D[0], c_2D[1], c_2D[2], c_2D[3], colors[0]};
    struct plane p5140 = {points[5], points[1], points[4], points[0], c_2D[5], c_2D[1], c_2D[4], c_2D[0],
                          colors[1]};
    struct plane p4062 = {points[4], points[0], points[6], points[2], c_2D[4], c_2D[0], c_2D[6], c_2D[2], colors[2]};
    struct plane p5476 = {points[5], points[4], points[7], points[6], c_2D[5], c_2D[4], c_2D[7], c_2D[6],
                          colors[3]};
    struct plane p5173 = {points[5], points[1], points[7], points[3], c_2D[5], c_2D[1], c_2D[7], c_2D[3],
                          colors[4]};
    struct plane p7362 = {points[7], points[3], points[6], points[2], c_2D[7], c_2D[3], c_2D[6], c_2D[2],
                          colors[5]};

    struct plane planes[NUM_PLANES] = {p0123, p5140, p4062, p5476, p5173, p7362};
    int curr_plane = PLANE0;
    for (int i = 0; i < NUM_PLANES; i++) {
        if ((planesToConsider & curr_plane) != 0) {
            planes[i].color = BG_COLOR;
        }
//        if((planesToConsider & TESTPLANE)) {
//            planes[i].color = LILAC;
//        }
        curr_plane = curr_plane >> 1;
    }

    draw_planes(canvas, WIDTH, HEIGHT, planes, NUM_PLANES);
}


/*
 *  Functions:      calculateX, calculateY, calculateZ
 *  -------------------
 *  Given the current coordinates, it will rotate it using linear algebra to its
 *      correct orientation.
 *
 *  Input params:
 *      struct cube* cubes:     array of cubes that the generated cubes will be stored in
 *      int num_cubes:          number of cubes in the Rubik's Cube
 *      float magnitude:        how long the dimensions of each cube is
 *      float camera_distance:  how far away the camera is from the cube
 *
 *  Return:
 *      No return value. Computed cubes are stored in cubes and the proper image is
 *          drawn into the canvas.
 */
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


/*
 *  Function:    f_modulo
 *  ---------------------
 *  Given the initial value and the mod value, this function will modify
 *      the float value that it is pointed to
 *
 *  Runtime Complexity: O(1)
 *
 *  Input params:
 *      float *x:   pointer to initial dividend value
 *      float y:    modulus divisor value
 *
 *  Return:
 *      No return value. The modulo value is placed directly into the input float.
 */
void f_modulo(float* x, float y) {
    *x =  *x - y * (float)(int)(*x/y);
}


/*
 *  Function:    generateCorners
 *  ----------------------------
 *  Given the magnitude, camera distance, offsets in the x, y, and z axis,
 *      this function places the correct corner coordinates into the cube
 *      pointer that is passed in. By default, the cube will's center will
 *      be at (0,0,0).
 *
 *  Runtime Complexity: O(1)
 *
 *  Input params:
 *      struct cube *cube:      cube object where the corners will be set
 *      float magnitude:        length of cube size
 *      float camera_distance:  distance of camera from cube
 *      float x_offset:         amount of offset in the x-direction
 *      float y_offset:         amount of offset in the y-direction
 *      float z_offset:         amount of offset in the z-direction
 *
 *  Return:
 *      No return value. Computed corners are placed into the cube pointer
 *          that was passed in.
 */
void generateCorners(struct cube *cube, float magnitude, float camera_distance, float x_offset, float y_offset, float z_offset) {

    float x_factor = -1;
    float y_factor = -1;
    float z_factor = -1;

    for(int i=0; i<NUM_CORNERS; i++){
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

void copy_cube(struct cube *dest, struct cube *src) {
    for(int i=0; i<NUM_CORNERS; i++) {
        dest->points[i] = src->points[i];
    }
    dest->planesToDraw = src->planesToDraw;
}

void copy_3D_point(struct Point_3D *dest, struct Point_3D *src) {
    dest->x = src->x;
    dest->y = src->y;
    dest->z = src->z;
}



/*
 *  Function:    generateCubes
 *  -------------------
 *  given the number of cubes, magnitude, and camera distance, this function generates
 *      the correct corner coordinates for each cube, finds the active planes (i.e.
 *      corner cube has 3 active ones, edges have 2, and faces have 1). Lastly, it
 *      sorts the cubes based on the average z coordinates of the corners so that it
 *      is painted in the right order.
 *
 *  Input params:
 *      struct cube* cubes:     array of cubes that the generated cubes will be stored in
 *      int num_cubes:          number of cubes in the Rubik's Cube
 *      float magnitude:        how long the dimensions of each cube is
 *      float camera_distance:  how far away the camera is from the cube
 *
 *  Return:
 *      No return value. Computed cubes are stored in cubes and the proper image is
 *          drawn into the canvas.
 */
void generateCubes(struct cube *cubes, int num_cubes, float magnitude, float camera_distance, uint32_t *colors) {
    int i, j, k;

    const int cubes_rows = DIMENSION;
    int cubes_cols = DIMENSION;
    int cubes_height = DIMENSION;

    struct cube translatedCubes[num_cubes];

    for(i=0; i<num_cubes; i++) {
        for(j=0; j<NUM_CORNERS; j++) {
            translatedCubes[i].points[j] = cubes[i].points[j];
        }
        translatedCubes[i].planesToDraw = 0;
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
                    generateCorners(&translatedCubes[count], magnitude, camera_distance, x_offset + (spacing * (float) i),
                                    y_offset + (spacing * (float) j), z_offset + (spacing * (float) k));

                    // assigns each cube with which planes are active
                    if(i == 0) translatedCubes[count].planesToDraw += PLANE4;
                    if(i == DIMENSION - 1) translatedCubes[count].planesToDraw += PLANE2;


                    if(j == 0) translatedCubes[count].planesToDraw += PLANE5;
                    if(j == DIMENSION - 1) translatedCubes[count].planesToDraw += PLANE1;

                    if(k == 0) translatedCubes[count].planesToDraw += PLANE3;
                    if(k == DIMENSION - 1) translatedCubes[count].planesToDraw += PLANE0;

                    //if(i==0 && j==1 && k==0) cubes[count].planesToDraw += TESTPLANE;

                    count++;
                }
            }
        }
    }

    for(i=0; i<num_cubes; i++) {
        copy_cube(&cubes[i], &translatedCubes[i]);
    }

    // rotate corners for each cube
    for(i=0; i<num_cubes; i++) {
        for(j=0; j<NUM_CORNERS; j++) {
            struct Point_3D new_point = {calculateX(translatedCubes[i].points[j].x, translatedCubes[i].points[j].y, translatedCubes[i].points[j].z - camera_distance),
                    calculateY(translatedCubes[i].points[j].x, translatedCubes[i].points[j].y, translatedCubes[i].points[j].z - camera_distance),
                    calculateZ(translatedCubes[i].points[j].x, translatedCubes[i].points[j].y, translatedCubes[i].points[j].z - camera_distance) + camera_distance};
            translatedCubes[i].points[j] = new_point;
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
            if (get_average_z_corners(translatedCubes[j].points) > get_average_z_corners(translatedCubes[min_idx].points))
                min_idx = j;

        // Swap the found minimum element with the first element
        struct Point_3D temp;
        uint8_t temp_num;
        if(min_idx != i) {
            for(j=0; j < NUM_CORNERS; j++) {
                temp = translatedCubes[min_idx].points[j];
                translatedCubes[min_idx].points[j] = translatedCubes[i].points[j];
                translatedCubes[i].points[j] = temp;
            }

            temp_num = translatedCubes[min_idx].planesToDraw;
            translatedCubes[min_idx].planesToDraw = translatedCubes[i].planesToDraw;
            translatedCubes[i].planesToDraw = temp_num;

        }
    }

    for(i=0; i<num_cubes; i++) {
        draw_cube(pixels, WIDTH, HEIGHT, translatedCubes[i].points, camera, colors, ~translatedCubes[i].planesToDraw);
    }
}

void draw_guide_lines(struct Point_2D *translatedPoints) {
    // draw neighbor line
    int i;
    for(i=0; i<NUM_CORNERS; i+=2) {
        draw_line(pixels, WIDTH, HEIGHT, (int)(translatedPoints[i].x + 0.5), (int)(translatedPoints[i].y + 0.5), (int)(translatedPoints[i+1].x + 0.5),
                  (int)(translatedPoints[i+1].y + 0.5), HOTPINK);
    }

    // draw long line
    for(i = 0; i<NUM_CORNERS; i+=2) {
        draw_line(pixels, WIDTH, HEIGHT, (int)(translatedPoints[i].x + 0.5), (int)(translatedPoints[i].y + 0.5), (int)(translatedPoints[(i + 2) % 8].x + 0.5), (int)(translatedPoints[(i + 2) % 8].y + 0.5), HOTPINK);
        draw_line(pixels, WIDTH, HEIGHT, (int)(translatedPoints[i+1].x + 0.5), (int)(translatedPoints[i+1].y + 0.5), (int)(translatedPoints[(i + 3) % 8].x + 0.5), (int)(translatedPoints[(i + 3) % 8].y + 0.5), HOTPINK);
    }
}

void process_mode_length_i(struct cube* cubes, struct cube* cubesOfInterest, int location, float camera_distance, struct Point_3D camera) {
    int i, j, k;
    int count = 0;
    for(i=0; i<DIMENSION; i++) {
        for(j=0; j<DIMENSION; j++) {
            for(k=0; k<DIMENSION; k++) {
                if(i == 0 || i == DIMENSION - 1 ||
                   j == 0 || j == DIMENSION - 1 ||
                   k == 0 || k == DIMENSION - 1) {
                    if(i == location && j == 0 && k == 0) {
                        copy_cube(&cubesOfInterest[0], &cubes[count]);
                    }
                    if(i == location && j == DIMENSION-1 && k == 0) {
                        copy_cube(&cubesOfInterest[1], &cubes[count]);
                    }
                    if(i == location && j == DIMENSION-1 && k == DIMENSION-1) {
                        copy_cube(&cubesOfInterest[2], &cubes[count]);
                    }
                    if(i == location && j == 0 && k == DIMENSION-1) {
                        copy_cube(&cubesOfInterest[3], &cubes[count]);
                    }


                    count++;
                }
            }
        }
    }
    // assume mode == MODE_LENGTH_I

    float miny = FLT_MAX, minz = FLT_MAX;

    for(i=0; i<NUM_CORNERS; i++) {
        if(cubesOfInterest[0].points[i].y < miny) {
            miny = cubesOfInterest[0].points[i].y;
        }
        if(cubesOfInterest[0].points[i].z < minz) {
            minz = cubesOfInterest[0].points[i].z;
        }
    }

    float maxy = FLT_MIN, maxz = FLT_MIN;

    for(i=0; i<NUM_CORNERS; i++) {
        if(cubesOfInterest[2].points[i].y > maxy) {
            maxy = cubesOfInterest[2].points[i].y;
        }
        if(cubesOfInterest[2].points[i].z > maxz) {
            maxz = cubesOfInterest[2].points[i].z;
        }
    }

    struct Point_3D corners[NUM_CORNERS];
    int temp = 0;

    for(i=0; i<NUM_CORNERS; i++) {
        if(cubesOfInterest[0].points[i].y == miny && cubesOfInterest[0].points[i].z == minz) {
            copy_3D_point(&corners[temp], &cubesOfInterest[0].points[i]);
            temp++;
        }
    }

    for(i=0; i<NUM_CORNERS; i++) {
        if(cubesOfInterest[1].points[i].y == maxy && cubesOfInterest[1].points[i].z == minz) {
            copy_3D_point(&corners[temp], &cubesOfInterest[1].points[i]);
            temp++;
        }
    }

    for(i=0; i<NUM_CORNERS; i++) {
        if (cubesOfInterest[2].points[i].y == maxy && cubesOfInterest[2].points[i].z == maxz) {
            copy_3D_point(&corners[temp], &cubesOfInterest[2].points[i]);
            temp++;
        }
    }

    for(i=0; i<NUM_CORNERS; i++) {
        if(cubesOfInterest[3].points[i].y == miny && cubesOfInterest[3].points[i].z == maxz) {
            copy_3D_point(&corners[temp], &cubesOfInterest[3].points[i]);
            temp++;
        }
    }

    for(i=0; i<NUM_CORNERS; i++) {
        struct Point_3D new_point = {calculateX(corners[i].x, corners[i].y, corners[i].z - camera_distance),
                                     calculateY(corners[i].x, corners[i].y, corners[i].z - camera_distance),
                                     calculateZ(corners[i].x, corners[i].y, corners[i].z - camera_distance) + camera_distance};
        corners[i] = new_point;
    }

    struct Point_2D translatedPoints[NUM_CORNERS];
    convert_3D_to_2D(translatedPoints, WIDTH, HEIGHT, corners, camera);

    draw_guide_lines(translatedPoints);

}

void process_mode_width_j(struct cube* cubes, struct cube* cubesOfInterest, int location, float camera_distance, struct Point_3D camera) {
    int i, j, k;
    int count = 0;
    for(i=0; i<DIMENSION; i++) {
        for(j=0; j<DIMENSION; j++) {
            for(k=0; k<DIMENSION; k++) {
                if(i == 0 || i == DIMENSION - 1 ||
                   j == 0 || j == DIMENSION - 1 ||
                   k == 0 || k == DIMENSION - 1) {
                    if(i == 0 && j == location && k == 0) {
                        copy_cube(&cubesOfInterest[0], &cubes[count]);
                    }
                    if(i == DIMENSION-1 && j == location && k == 0) {
                        copy_cube(&cubesOfInterest[1], &cubes[count]);
                    }
                    if(i == DIMENSION-1 && j == location && k == DIMENSION-1) {
                        copy_cube(&cubesOfInterest[2], &cubes[count]);
                    }
                    if(i == 0 && j == location && k == DIMENSION-1) {
                        copy_cube(&cubesOfInterest[3], &cubes[count]);
                    }


                    count++;
                }
            }
        }
    }
    // assume mode == MODE_LENGTH_I

    float minx = FLT_MAX, minz = FLT_MAX;

    for(i=0; i<NUM_CORNERS; i++) {
        if(cubesOfInterest[0].points[i].x < minx) {
            minx = cubesOfInterest[0].points[i].x;
        }
        if(cubesOfInterest[0].points[i].z < minz) {
            minz = cubesOfInterest[0].points[i].z;
        }
    }

    float maxx = FLT_MIN, maxz = FLT_MIN;

    for(i=0; i<NUM_CORNERS; i++) {
        if(cubesOfInterest[2].points[i].x > maxx) {
            maxx = cubesOfInterest[2].points[i].x;
        }
        if(cubesOfInterest[2].points[i].z > maxz) {
            maxz = cubesOfInterest[2].points[i].z;
        }
    }

    struct Point_3D corners[NUM_CORNERS];
    int temp = 0;

    for(i=0; i<NUM_CORNERS; i++) {
        if(cubesOfInterest[0].points[i].x == minx && cubesOfInterest[0].points[i].z == minz) {
            copy_3D_point(&corners[temp], &cubesOfInterest[0].points[i]);
            temp++;
        }
    }

    for(i=0; i<NUM_CORNERS; i++) {
        if(cubesOfInterest[1].points[i].x == maxx && cubesOfInterest[1].points[i].z == minz) {
            copy_3D_point(&corners[temp], &cubesOfInterest[1].points[i]);
            temp++;
        }
    }

    for(i=0; i<NUM_CORNERS; i++) {
        if (cubesOfInterest[2].points[i].x == maxx && cubesOfInterest[2].points[i].z == maxz) {
            copy_3D_point(&corners[temp], &cubesOfInterest[2].points[i]);
            temp++;
        }
    }

    for(i=0; i<NUM_CORNERS; i++) {
        if(cubesOfInterest[3].points[i].x == minx && cubesOfInterest[3].points[i].z == maxz) {
            copy_3D_point(&corners[temp], &cubesOfInterest[3].points[i]);
            temp++;
        }
    }

    for(i=0; i<NUM_CORNERS; i++) {
        struct Point_3D new_point = {calculateX(corners[i].x, corners[i].y, corners[i].z - camera_distance),
                                     calculateY(corners[i].x, corners[i].y, corners[i].z - camera_distance),
                                     calculateZ(corners[i].x, corners[i].y, corners[i].z - camera_distance) + camera_distance};
        corners[i] = new_point;
    }

    struct Point_2D translatedPoints[NUM_CORNERS];
    convert_3D_to_2D(translatedPoints, WIDTH, HEIGHT, corners, camera);

    draw_guide_lines(translatedPoints);
}

void process_mode_height_k(struct cube* cubes, struct cube* cubesOfInterest, int location, float camera_distance, struct Point_3D camera) {
    int i, j, k;
    int count = 0;
    for(i=0; i<DIMENSION; i++) {
        for(j=0; j<DIMENSION; j++) {
            for(k=0; k<DIMENSION; k++) {
                if(i == 0 || i == DIMENSION - 1 ||
                   j == 0 || j == DIMENSION - 1 ||
                   k == 0 || k == DIMENSION - 1) {
                    if(i == 0 && j == 0 && k == location) {
                        copy_cube(&cubesOfInterest[0], &cubes[count]);
                    }
                    if(i == DIMENSION-1 && j == 0 && k == location) {
                        copy_cube(&cubesOfInterest[1], &cubes[count]);
                    }
                    if(i == DIMENSION-1 && j == DIMENSION-1 && k == location) {
                        copy_cube(&cubesOfInterest[2], &cubes[count]);
                    }
                    if(i == 0 && j == DIMENSION-1 && k == location) {
                        copy_cube(&cubesOfInterest[3], &cubes[count]);
                    }


                    count++;
                }
            }
        }
    }
    // assume mode == MODE_LENGTH_I

    float minx = FLT_MAX, miny = FLT_MAX;

    for(i=0; i<NUM_CORNERS; i++) {
        if(cubesOfInterest[0].points[i].x < minx) {
            minx = cubesOfInterest[0].points[i].x;
        }
        if(cubesOfInterest[0].points[i].y < miny) {
            miny = cubesOfInterest[0].points[i].y;
        }
    }

    float maxx = FLT_MIN, maxy = FLT_MIN;

    for(i=0; i<NUM_CORNERS; i++) {
        if(cubesOfInterest[2].points[i].x > maxx) {
            maxx = cubesOfInterest[2].points[i].x;
        }
        if(cubesOfInterest[2].points[i].y > maxy) {
            maxy = cubesOfInterest[2].points[i].y;
        }
    }

    struct Point_3D corners[NUM_CORNERS];
    int temp = 0;

    for(i=0; i<NUM_CORNERS; i++) {
        if(cubesOfInterest[0].points[i].x == minx && cubesOfInterest[0].points[i].y == miny) {
            copy_3D_point(&corners[temp], &cubesOfInterest[0].points[i]);
            temp++;
        }
    }

    for(i=0; i<NUM_CORNERS; i++) {
        if(cubesOfInterest[1].points[i].x == maxx && cubesOfInterest[1].points[i].y == miny) {
            copy_3D_point(&corners[temp], &cubesOfInterest[1].points[i]);
            temp++;
        }
    }

    for(i=0; i<NUM_CORNERS; i++) {
        if (cubesOfInterest[2].points[i].x == maxx && cubesOfInterest[2].points[i].y == maxy) {
            copy_3D_point(&corners[temp], &cubesOfInterest[2].points[i]);
            temp++;
        }
    }

    for(i=0; i<NUM_CORNERS; i++) {
        if(cubesOfInterest[3].points[i].x == minx && cubesOfInterest[3].points[i].y == maxy) {
            copy_3D_point(&corners[temp], &cubesOfInterest[3].points[i]);
            temp++;
        }
    }

    for(i=0; i<NUM_CORNERS; i++) {
        struct Point_3D new_point = {calculateX(corners[i].x, corners[i].y, corners[i].z - camera_distance),
                                     calculateY(corners[i].x, corners[i].y, corners[i].z - camera_distance),
                                     calculateZ(corners[i].x, corners[i].y, corners[i].z - camera_distance) + camera_distance};
        corners[i] = new_point;
    }

    struct Point_2D translatedPoints[NUM_CORNERS];
    convert_3D_to_2D(translatedPoints, WIDTH, HEIGHT, corners, camera);

    // draw neighbor line
    draw_guide_lines(translatedPoints);
}

void generateIndicator(struct cube* cubes, int mode, int location, float camera_distance, struct Point_3D camera) {
    if(location >= DIMENSION) {
        return;
    }

    struct cube cubesOfInterest[4];

    if(mode == MODE_LENGTH_I) {
        process_mode_length_i(cubes, cubesOfInterest, location, camera_distance, camera);
    } else if(mode == MODE_WIDTH_J) {
        process_mode_width_j(cubes, cubesOfInterest, location, camera_distance, camera);
    } else if(mode == MODE_HEIGHT_K) {
        process_mode_height_k(cubes, cubesOfInterest, location, camera_distance, camera);
    } else {
        return;
    }




}


/*
 *  Function:    render
 *  -------------------
 *  calls the methods to draw/compute the cube and store it in an
 *      uint32_t array describing the pixels on the screen.
 *
 *  Input params (called by js file):
 *      int dt:     the current 'tick', computed on the js file
 *      float a:    controls angle of an axis
 *      float b:    controls angle of another axis
 *      float c:    controls angle of yet another axis
 *
 *  Return:
 *      uint32_t *pixels: a 2D array containing the pixels to be drawn
 *          onto the screen.
 */
uint32_t* render(int dt, float a, float b, float c, int dimension, int mode, int location) {
    DIMENSION = dimension;

    dt = dt % 360;

    fill_screen(pixels, WIDTH, HEIGHT, BG_COLOR);

    A = a;
    B = b;
    C = c;

    f_modulo(&A, 2*(float)PI);
    f_modulo(&B, 2*(float)PI);
    f_modulo(&C, 2*(float)PI);

    float magnitude = SCALE - GAP;
    float camera_distance = 200;

    int num_cubes = (DIMENSION == 1) ? 1 : DIMENSION * DIMENSION * DIMENSION - (DIMENSION-2) * (DIMENSION-2) * (DIMENSION-2);
    struct cube cubes[num_cubes];

    struct Point_3D camera = {0, 0, 0};

    uint32_t colors[NUM_PLANES] = {BLUE, YELLOW, RED, GREEN, ORANGE, WHITE};

    generateCubes(cubes, num_cubes, magnitude, camera_distance, colors);

    generateIndicator(cubes, mode, location, camera_distance, camera);

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

    int num_cubes = (DIMENSION == 1) ? 1 : DIMENSION * DIMENSION * DIMENSION - (DIMENSION-2) * (DIMENSION-2) * (DIMENSION-2);
    struct cube cubes[num_cubes];

    struct Point_3D camera = {0, 0, 0};

    uint32_t colors[NUM_PLANES] = {BLUE, YELLOW, RED, GREEN, ORANGE, WHITE};

    generateCubes(cubes, num_cubes, magnitude, camera_distance, colors);

    generateIndicator(cubes, MODE_LENGTH_I, 3, camera_distance, camera);

    return 0;
}