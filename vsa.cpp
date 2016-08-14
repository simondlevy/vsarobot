/*
vsa.cpp - VSA implementation of connectionist behavior-based robot controller.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as 
    published by the Free Software Foundation, either version 3 of the 
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    Simon D. Levy      Washington & Lee University       April 2013
*/


#include <stdlib.h> // for random()
#include <stdio.h>  // for FILE

#include "control.hpp"

static const int NDIMS = 1000;

typedef struct VSA 
{
    // Sensor->Controller weights
    char s_oc[NDIMS];
    char s_lc[NDIMS];
    char s_rc[NDIMS];
    
    // Controller->Actuator weights
    char a_cl[NDIMS];
    char a_cr[NDIMS];
    
    // Controller
    char c[NDIMS];
    
} VSA;

// Fills M vectors B with random values -1, +1
static void _randomize(char *v)
{
    for (int k=0; k<NDIMS; ++k) 
    {
        v[k] = 2 * (random() % 2) - 1;
    }
}

// Dot product with hard squashing function
static char _dotprod(char * a, char * b)
{
    int d = 0;
    
    for (int k=0; k<NDIMS; ++k) 
    {
        d += a[k] * b[k];
    }
    
    if (d < -NDIMS/2) {
        return -1;
    }
    
    else if (d > NDIMS/2) {
        return +1;
    }
    
    return 0;
}

// normalize (0,1) to (-1,+1)
static void _normposneg(char * bit)
{
    *bit = 2 * (*bit) - 1;
}

static void _enter(
    VSA * vsa, 
    char obst, 
    char lgt_l, 
    char lgt_r, 
    char mot_l,
    char mot_r)
{
    for (int k=0; k<NDIMS; ++k) 
    {
        char s_oc_k = obst  ? vsa->s_oc[k] : 1;
        char s_lc_k = lgt_l ? vsa->s_lc[k] : 1;
        char s_rc_k = lgt_r ? vsa->s_rc[k] : 1;
        
        char a_cl_k = mot_l ? vsa->a_cl[k] : 0;
        char a_cr_k = mot_r ? vsa->a_cr[k] : 0;
        
        vsa->c[k] += (s_oc_k*s_lc_k*s_rc_k * (a_cl_k+a_cr_k));
    }    
}

static void _lookup(
    void * p, 
    char obst, 
    char lgt_l, 
    char lgt_r, 
    char * mot_l,
    char * mot_r)
{
    VSA * vsa = (VSA *)p;
    
    char result[NDIMS];
    
    for (int k=0; k<NDIMS; ++k) 
    {
        char s_oc_k = obst  ? vsa->s_oc[k] : 1;
        char s_lc_k = lgt_l ? vsa->s_lc[k] : 1;
        char s_rc_k = lgt_r ? vsa->s_rc[k] : 1;
        
        result[k] = vsa->c[k] * s_oc_k * s_lc_k * s_rc_k;
    }
    
    *mot_l = _dotprod(result, vsa->a_cl);
    *mot_r = _dotprod(result, vsa->a_cr);
}


void BehaviorBasedControl::getMotorSpeeds(
    char noseSensor, 
    char lftVerticalSensor, 
    char rgtVerticalSensor, 
    char * lftMotorSpeed, 
    char * rgtMotorSpeed,
    FILE * logfp)
{    
    _lookup(this->p, noseSensor, lftVerticalSensor, rgtVerticalSensor, 
        lftMotorSpeed, rgtMotorSpeed);
    
    _normposneg(lftMotorSpeed);
    _normposneg(rgtMotorSpeed);
    
}

BehaviorBasedControl::BehaviorBasedControl(FILE * logfp) 
{   
    VSA * vsa = new VSA();
    
    // Fill sensor, actuator symbols with random values
    _randomize(vsa->s_oc);
    _randomize(vsa->s_lc);
    _randomize(vsa->s_rc);
    _randomize(vsa->a_cl);
    _randomize(vsa->a_cr);
    
    // Zero-out controller
    for (int k=0; k<NDIMS; ++k) 
    {
        vsa->c[k] = 0;
    }
    
    // Seek light
    _enter(vsa,  0, 1, 1,  1, 1);
    _enter(vsa,  1, 1, 1,  1, 1);
    _enter(vsa,  0, 1, 0,  0, 1);
    _enter(vsa,  1, 1, 0,  0, 1);
    _enter(vsa,  0, 0, 1,  1, 0);
    _enter(vsa,  1, 0, 1,  1, 0);
    
    // Avoid obstacles
    _enter(vsa,  1, 0, 0,  0, 1);
    
    // Cruise
    _enter(vsa,  0, 0, 0,  1, 1);  
    
    this->p = vsa;    
}

BehaviorBasedControl::~BehaviorBasedControl()
{
    free(this->p);
}





