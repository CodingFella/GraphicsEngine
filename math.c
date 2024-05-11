//
// Created by Jacob Lin on 5/9/24.
//


#define TERMS 8

#define PI 3.14159265359


float sin(float x) //sin calculation
{
    float sum;
    float fa;
    float pow;
    sum = 0;
    for(int i = 0; i <= TERMS; i++)
    {
        fa = 1;
        pow = 1;
        for(int j = 1; j <= 2*i+1; j++)
        {
            fa *= j;
            pow *= x;
        }
        sum += (float)((i%2?-1.0:1.0)/fa)*pow;
    }
    return sum;
}

float cos(float x) //cosine calculation
{
    float sum;
    float fa;
    float pow;
    sum = 0;
    for(int i = 0; i <= TERMS; i++)
    {
        fa = 1;
        pow = 1;
        for(int j = 1; j <= 2*i; j++)
        {
            fa *= j;
            pow *= x;
        }
        sum += (float)((i%2?-1.0:1.0)/fa)*pow;
    }
    return sum;
}