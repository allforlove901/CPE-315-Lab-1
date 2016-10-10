//
//  floatingPoint.h
//  CPE315_Lab1
//
//  Created by Brett Nelson on 9/28/16.
//  Copyright © 2016 Brett Nelson. All rights reserved.
//

#ifndef floatingPoint_h
#define floatingPoint_h
#include <stdio.h>
#include <stdlib.h>

typedef struct _intfloat {
    unsigned int sign;
    int exponent;
    int fraction;
} INTFLOAT, *INTFLOAT_PTR;

void extract_float(INTFLOAT_PTR x, float f);
void scale_it(INTFLOAT_PTR x, int n);
void normalize(INTFLOAT_PTR x);
float single_float_add(float a, float b);
float single_float_subtract(float a, float b);
float pack_float(INTFLOAT_PTR x);

#endif /* floatingPoint_h */
