/*
*This file contains all the commands that the drop pod can operate, each one is referenced with a specific float value which is transmitted via the RF24 radio transmitter and then executed.
*/
 static float PING = 0.0f;//Simple command where the responce indicates that the drop pod has booted up/ is responding
 static float DETATCH_POD = 1.0f;// Closes the servo mechanism attatching the pod to the drone
 static float ATTATCH_POD = 2.0f; // Similar to DETATCH_POD, however the mechanism is just opened
 static float DEPLOY_PARACHUTE = 3.0f;//Opens the servo mechanism and the parachute is held in by
 static float CLOSE_PARACHUTE_CATCH = 4.0f; // Refers to the servo mechanism that holds the parachute into the pod
 static float GET_ACCELERATION = 5.0f;// Current acceleration of gravity that the pod feels, in free fall this should be close to 0g
 static float GET_ORIENTATION = 6.0f; // Final operation reaturns angle at which the pod is orientated (degrees)