/*
behavior_classical.hpp - classic if/then/else hierachy for behavior-based robot
                         simulator

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

#include <stddef.h> // for NULL
#include <stdio.h>  // for FILE

#include "control.hpp"

static bool seekLight(
    char noseSensor,
    char lftVerticalSensor, 
    char rgtVerticalSensor, 
    char * lftMotorSpeed, 
    char * rgtMotorSpeed)
{
    if (lftVerticalSensor && rgtVerticalSensor)
    {
        *lftMotorSpeed = +1;
        *rgtMotorSpeed = +1;
        return true;
    }
    else if (lftVerticalSensor)
    {
        *lftMotorSpeed = -1;
        *rgtMotorSpeed = +1;
        return true;
    }
    else if (rgtVerticalSensor)
    {
        *lftMotorSpeed = +1;
        *rgtMotorSpeed = -1;
        return true;
    }
    
    return false;
}

static bool avoidObstacles(
    char noseSensor, 
    char lftVerticalSensor, 
    char rgtVerticalSensor, 
    char * lftMotorSpeed, 
    char * rgtMotorSpeed)
{
    if (noseSensor) 
    {
        *lftMotorSpeed = -1;
        *rgtMotorSpeed = +1;
        return true;
    }
    return false;
}

static bool cruise(
    char noseSensor, 
    char lftVerticalSensor, 
    char rgtVerticalSensor, 
    char * lftMotorSpeed, 
    char * rgtMotorSpeed)
{
    *lftMotorSpeed = +1;
    *rgtMotorSpeed = +1;
    return true;
}


void BehaviorBasedControl::getMotorSpeeds(
    char noseSensor, 
    char lftVerticalSensor, 
    char rgtVerticalSensor, 
    char * lftMotorSpeed, 
    char * rgtMotorSpeed,
    FILE * logfp)
{    
    typedef bool (* Behavior )(char, char, char, char *, char *);
    
    Behavior behaviors[3] = {seekLight, avoidObstacles, cruise};
    
    // abritrate
    for (int k=0; k<3; ++k) 
    {
        if (behaviors[k](noseSensor, lftVerticalSensor, rgtVerticalSensor, 
                         lftMotorSpeed, rgtMotorSpeed))
        {
            return;
        }
    }
}

BehaviorBasedControl::BehaviorBasedControl(FILE * logfp) 
{
    p = NULL;
}

BehaviorBasedControl::~BehaviorBasedControl()
{
}



