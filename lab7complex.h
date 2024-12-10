#ifndef _COMPLEX_H
#define _COMPLEX_H

typedef struct complex
{
  double re, im;
} complex;

void complex_add(complex *dst, const complex *z1, const complex *z2);   
void complex_mul(complex *dst, const complex *z1, const complex *z2);

void complex_print(const complex *z);



#endif