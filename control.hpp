/*
behavior.hpp - C++ class definition for behavior-based robot simulator.

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

class BehaviorBasedControl {
    
private:
    
    // generic storage
    void * p; 
    
public:    
    
    BehaviorBasedControl(FILE * logfp);
    
    ~BehaviorBasedControl();

    void getMotorSpeeds(
        
        // triggers are 0,1
        char noseSensorTrigger,         
        char leftVertSensorTrigger, 
        char rightVertSensorTrigger, 
        
        // motor speeds are -1,+1
        char * leftMotorSpeed,          
        char * rightMotorSpeed,
        
        // for logging
        FILE * logfp);
};

