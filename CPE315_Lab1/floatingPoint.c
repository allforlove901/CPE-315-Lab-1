//
//  main.c
//  CPE315_Lab1
//
//  Created by Brett Nelson on 9/28/16.
//  Copyright © 2016 Brett Nelson. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "floatingPoint.h"

int main(int argc, const char * argv[]) {
    
    
    
    return 0;
}

//function to extract components sign, exponent, and
//fraction from a single precision floating point number
void extract_float(INTFLOAT_PTR x, float f){
    
    //declare unsigned int extract
    unsigned extract = * (unsigned int *) &f;
    
    //get sign
    x->sign = (extract & 0x80000000);
    
    //get exponent
    x->exponent = (extract >> 23) & 0x000000FF;
    x->exponent -= 127; //subtract bias
    
    //get fraction
    x->fraction = (extract << 7) & 0x3FFFFF80;
    x->fraction |= 0x40000000; //add hidden 1
    
    printf("Fraction x: %x\n", x->fraction);

    if(x->sign)
        x->fraction = -x->fraction; //negate if negative
    printf("Fraction x: %x\n", x->fraction);

    
}

void scale_it(INTFLOAT_PTR x, int n){
    
    x->fraction >>= n; //shift right n positions
    x->exponent += n;  //increment exponent n times
    
}

void normalize(INTFLOAT_PTR x){
    
    unsigned int signCompare;
    signCompare = (x->fraction >> 1);
    
    if (x->fraction == 0) return; //zero case
    
    //push left until sign and adjacent bits different
    while(((x->fraction ^ signCompare) & 0x40000000) == 0)
    {
        x->fraction <<= 1; //shift fraction left
        x->exponent--;     //decrement exponent
    }
}

//function to add together two single precision floating point numbers
float single_float_add(float a, float b){
    
    if(a == 0)
        return b;
    else if(b == 0)
        return a;
    
    INTFLOAT_PTR ax = malloc(sizeof(INTFLOAT));
    INTFLOAT_PTR bx = malloc(sizeof(INTFLOAT));
    INTFLOAT_PTR sum = malloc(sizeof(INTFLOAT));
    int expDifference;
    
    extract_float(ax, a);
    extract_float(bx, b);
    
    printf("Sign A: %x\n" ,ax->sign);
    printf("Fraction A: %x\n", ax->fraction);
    printf("Exponent A: %x\n", ax->exponent);
    
    printf("Sign B: %x\n" ,bx->sign);
    printf("Fraction: B %x\n", bx->fraction);
    printf("Exponent: B %x\n", bx->exponent);
    
    expDifference = ax->exponent - bx->exponent; //Get difference in scale
    
    //scale down whichever has smaller exponent
    if(expDifference > 0)
        scale_it(bx, expDifference);
    else if(expDifference < 0)
        scale_it(ax, -1*expDifference);
    /*
    printf("STEP ONE \n");
    
    printf("Sign A: %x\n" ,ax->sign);
    printf("Fraction A: %x\n", ax->fraction);
    printf("Exponent A: %x\n", ax->exponent);
    
    printf("Sign B: %x\n" ,bx->sign);
    printf("Fraction: B %x\n", bx->fraction);
    printf("Exponent: B %x\n", bx->exponent);*/
    
    //add fractions and scale each down by 1 to prevent overflow
    if((ax->fraction >> 1) + (bx->fraction >> 1) < 0)
        sum->sign = 0x80000000;
    
    sum->fraction = (ax->fraction >> 1) + (bx->fraction >> 1);
    sum->exponent = ax->exponent + 1;
    /*
    printf("STEP TWO \n");

    
    printf("Sign A: %x\n" ,ax->sign);
    printf("Fraction A: %x\n", ax->fraction);
    printf("Exponent A: %x\n", ax->exponent);
    
    printf("Sign B: %x\n" ,bx->sign);
    printf("Fraction: B %x\n", bx->fraction);
    printf("Exponent: B %x\n", bx->exponent);*/
    
    //normalize sum
    normalize(sum);
    
    /*
    printf("Sign Sum: %x\n" ,sum->sign);
    printf("Fraction Sum: %x\n", sum->fraction);
    printf("Exponent Sum: %x\n", sum->exponent);*/
    
    return pack_float(sum);
}

//method to package fixed point fields into IEEE 754 number format
float pack_float(INTFLOAT_PTR x){
    
    unsigned int f = 0;
    float output = 0;
    
    //place sign bit
    f |= (x->sign & 0x80000000);
    
    //printf("Float f: %x\n" ,f);

    //add bias and place exponent bits
    x->exponent += 127;
    //printf("Exponent x: %x\n" ,x->exponent);
    f |= (x->exponent << 23);
    //printf("Float f: %x\n" ,f);
    
    //printf("START HERE\n");

    //place fraction bits
    x->fraction >>= 7;
    //printf("Fraction x: %x\n" ,x->fraction);
    f |= (x->fraction) & 0x007FFFFF;
    //printf("Float f: %x\n" ,f);
    
    output = *((float*)&f);
    
    return output;
    
}

float single_float_subtract(float a, float b){
    
    int expDifference = 0;
    INTFLOAT ax, bx, sum;
    
    extract_float(&ax, a);
    extract_float(&bx, b);
    
    expDifference = ax.exponent - bx.exponent; //Get difference in scale
    
    //scale down whichever has smaller exponent
    if(expDifference > 0)
        scale_it(&bx, expDifference);
    else if(expDifference < 0)
        scale_it(&ax, expDifference);

    //shift right one to avoid overflow and subtract
    sum.fraction = (ax.fraction >> 1) - (bx.fraction >> 1);
    sum.exponent = ax.exponent + 1;
    
    //normalize, pack and return
    normalize(&sum);
    return pack_float(&sum);
    
}
