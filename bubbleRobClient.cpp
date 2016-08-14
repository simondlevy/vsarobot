/*
bubbleRobClient.cpp - client for bubbleRob behavior-based robot simulator.

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


#include <stdio.h>
#include <stdlib.h>

#include "control.hpp"

const char * LOG_FILE                   = "/Users/levys/Documents/confs_and_talks/aaai2013/code/vsarobot/log.csv";

static const char * SERVER_IPADDR       = "127.0.0.1";
static const int TIMEOUT_MSEC           = 2000;
static const int COMM_THREAD_CYCLE_MSEC = 5;
static const float MOTOR_SPEED          = 3.1415f;

extern "C" {
    #include "extApi.h"
}

bool readProximitySensor(int clientID, int sensorHandle, char * sensorTrigger) 
{
    return simxReadProximitySensor(clientID, sensorHandle, sensorTrigger,
        NULL,NULL,NULL,simx_opmode_continuous) == simx_error_noerror;
}

int main(int argc, char ** argv)
{
#ifdef _TEST
    FILE * logfp = NULL;
    
    BehaviorBasedControl * c = new BehaviorBasedControl(logfp);
    
    char mot_l = 0;
    char mot_r = 0;
    
    c->getMotorSpeeds(1, 0, 1, &mot_l, &mot_r, logfp);
    
    printf("%d %d\n", mot_l, mot_r);
    
    return 0;
#else

    // Open log file for writing
	FILE * logfp = fopen(LOG_FILE, "w");
	
	// Check adequate number of arguments from server script
	if (argc < 5) 
	{
		fprintf(logfp, "Inadequate input\n");
		fclose(logfp);
		extApi_sleepMs(5000);
		return 0;
	}
	
	// Grab arguments from server script
	int portNb = atoi(argv[1]);
	int leftMotorHandle = atoi(argv[2]);
	int rightMotorHandle = atoi(argv[3]);
	int noseSensorHandle = atoi(argv[4]);
	int leftVertSensorHandle = atoi(argv[5]);
	int rightVertSensorHandle = atoi(argv[6]);
	
	// Connect to server with error check
	int clientID = simxStart(SERVER_IPADDR, portNb, true, true, TIMEOUT_MSEC, COMM_THREAD_CYCLE_MSEC);
	if (clientID == -1)
	{
	    fprintf(logfp, "Unable to connect to server at %s on port %d\n",
	        SERVER_IPADDR, portNb);
	    return 1;
	}
	
	// Initialize controller
	BehaviorBasedControl controller(logfp);
		
	// Keep looping as long as we have a connection
	while (simxGetConnectionId(clientID) != -1)
	{
	    char noseSensorTrigger = 0;
	    char leftVertSensorTrigger = 0;
	    char rightVertSensorTrigger = 0;
	    
	    char leftMotorSpeed  = 0;
	    char rightMotorSpeed = 0;
	    
	    // Read sensors from server and proceed on success
	    if (readProximitySensor(clientID, noseSensorHandle, &noseSensorTrigger) &&
	        readProximitySensor(clientID, leftVertSensorHandle, &leftVertSensorTrigger) &&
	    readProximitySensor(clientID, rightVertSensorHandle, &rightVertSensorTrigger)) 
	    {			    
	        
	        // Get motor speeds from controller
	        controller.getMotorSpeeds(
	            noseSensorTrigger, 
	            leftVertSensorTrigger,
	            rightVertSensorTrigger,
	            &leftMotorSpeed, 
	            &rightMotorSpeed,
	            logfp);
	        	        
	        // Send motor control commands back to server
	        simxSetJointTargetVelocity(clientID, leftMotorHandle, MOTOR_SPEED*leftMotorSpeed, simx_opmode_oneshot);			
	        simxSetJointTargetVelocity(clientID, rightMotorHandle,MOTOR_SPEED*rightMotorSpeed,simx_opmode_oneshot);			
	    }
	    
	    // Pause a bit between iterations
	    extApi_sleepMs(5);
	}
	
	// Done with simulation
	simxFinish(clientID);
	
	// Close log file
	fclose(logfp);
	
#endif

	// Report success
	return 0;
}

