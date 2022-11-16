void configure_all_sensors();
void drive(int distance, int motor_power);
void rotate(bool dir, int motor_power);
void rotate_to_begin(int quadrant, int motor_power);
void return_to_base();

void calculate_order(int *last_time_seen, int *order)
{

 	int temp = 0;
 	for (int i = 0; i < 3; i++) {
        for (int j = i + 1; j < 4; j++) {
            if (last_time_seen[i] < last_time_seen[j]) {

                temp = order[j];
                order[j] = order[i];
                order[i] = temp;
            }
        }
    }


}

task main()
{
	const int ROTATE_90 = 90;
	configure_all_sensors();

	int names[4];
	int quadrant[4];
	int last_time_seen[4];
	int order[4] = {1, 2, 3 , 4};



}

void configure_all_sensors()
{
 SensorType[S1] = sensorEV3_IRSensor;
 SensorType[S2] = sensorEV3_Ultrasonic;
 SensorType[S3] = sensorEV3_Color;
 wait1Msec(50);
 SensorMode[S3] = modeEV3Color_Color;
 wait1Msec(50);
 SensorType[S4] = sensorEV3_Gyro;
 wait1Msec(50);
 SensorMode[S4] = modeEV3Gyro_Calibration;
 wait1Msec(100);
 SensorMode[S4] = modeEV3Gyro_RateAndAngle;
}

void drive(int distance, int motor_power)
{

	nMotorEncoder[motorA] = 0;
	motor[motorA] = motor[motorD] = motor_power;

	while(nMotorEncoder[motorA] < distance*180/(PI*2.75))
	{}

	motor[motorA] = motor[motorD] = 0;
	return;
}


/*
	every turn should increment, based on how it moves. It needs to change the direction based on what the gyro value is.
*/
void rotate(bool dir, int motor_power, int angle, bool returning ) // dir=1 for ccw
{
	int  current_dir = abs(SensorValue(S4));
	motor[motorA]=(dir*1+!dir*-1)*motor_power; //+ if dir==1
	motor[motorD]=(!dir*1+dir*-1)*motor_power; //- if dir==1

	if (dir == 0)
	{
			while (abs(SensorValue[S4]) < current_dir  + angle)
			{	}
	}
	else (dir == 1)
	{
			while ((SensorValue[S4]) > current_dir + angle)
			{	}
	}

	/*

	Heres the */

	motor[motorB]=0;
	motor[motorC]=0;
	return;
}
//Done
void rotate_to_begin(int quadrant, int motor_power)
{
	int current_dir = SensorValue[S4];

	if (quadrant == 1 || quadrant == 2)
	{
			/*Either the angle is below 170 or below */
			const int DOWNWARD = 180;

			if (current_dir > DOWNWARD)
			{
					//get lowest angle below 180 that faces the same direction.
					SensorValue[S4] = current_dir - 360;
			}

			motor[motorA] = -motor_power;
			motor[motorD] = motor_power;

			if (current_dir < -DOWNWARD)
			{
					while(SensorValue[S4] < -DOWNWARD)
					{}
			}
			else if (current_dir < DOWNWARD)
			{

					/*the gyro is above 0 degrees, should spin until it faces
					directly down, which is any scalar multiple of 180*/
					while(SensorValue[S4] < DOWNWARD)
					{}
			}
	}
	else if (quadrant == 3 || quadrant == 4)
	{
			/*
				By subtracting 360 earlier, the angle for the gyro must be below 0.
				Therefore, we only have to account for it reaching 0.
			*/

			motor[motorA] = -motor_power;
			motor[motorD] = motor_power;

			if (current_dir < 0)
			{
					while(SensorValue[S4] < 0)
					{}
			}
			else if (current_dir < 360)
			{
					while(SensorValue[S4] < 360)
					{}
					SensorValue[S4] = 0;
			}
	}
	return;
}
