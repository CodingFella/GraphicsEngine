//
// Created by Jacob Lin on 5/9/24.
//

#define TERMS 8

#define PI 3.14159265359

#define NUMBERTYPE double

NUMBERTYPE sin(NUMBERTYPE x) //sin calculation
{
    NUMBERTYPE sum;
    NUMBERTYPE fa;
    NUMBERTYPE pow;
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
        sum += (NUMBERTYPE)((i%2?-1.0:1.0)/fa)*pow;
    }
    return sum;
}

NUMBERTYPE cos(NUMBERTYPE x) //cosine calculation
{
    NUMBERTYPE sum;
    NUMBERTYPE fa;
    NUMBERTYPE pow;
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
        sum += (NUMBERTYPE)((i%2?-1.0:1.0)/fa)*pow;
    }
    return sum;
}
