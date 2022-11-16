void configure_all_sensors();
int drive_path(int distance, int motor_power);
void rotate(bool dir, int motor_power);
void rotate_to_begin(int quadrant, int motor_power);
void return_to_base();
void manouver_obstacle(int motor_power);



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
	return;
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
void rotate(bool dir, int motor_power, int angle) // dir=1 for ccw
{
	int  current_dir = abs(SensorValue(S4));
	motor[motorA]=(dir*1+!dir*-1)*motor_power; //+ if dir==1
	motor[motorD]=(!dir*1+dir*-1)*motor_power; //- if dir==1

	if (dir == 0)
	{
			while (abs(SensorValue[S4]) < current_dir  + angle)
			{	}
	}

	else
	{
			while ((SensorValue[S4]) > current_dir + angle)
			{	}
	}

	/*

	Heres the */

	motor[motorB]=0;
	motor[motorC]=0;
	wait1Msec(500);
	return;
}


/*
		I believe that the 2 clock wise and 2 counter clockwise rotations will
		counter eachother's motor encoder counts, so the only changes will be the driving
		counts

*/
void manouver_obstacle(int motor_power)
{
		int x = 50;
		int y = 100;

		wait1Msec(500);
		int current_counts = nMotorEncoder[motorA];

		rotate(0, motor_power, 90);
		drive_path(x, motor_power);

		rotate(1, motor_power, 90);
		drive_path(y, motor_power);

		rotate(1, motor_power, 90);
		drive_path(x, motor_power);

		rotate(0, motor_power, 90);

		wait1Msec(500);
		nMotorEncoder[motorA] = current_counts + (2*x*180/(PI*2.75));
}

int catch_person()
{}

/*

We track the motor encoder counts here, but we might want TO_BLUETOOTO_BLUETOOTH
do this inside of the actual maneouver obstacle func

*/

int drive_path(int distance, int motor_power)
{
	int x = 50;
	const int TO_COUNTS = 180/(PI*2.75);

	nMotorEncoder[motorA] = 0;
	motor[motorA] = motor[motorD] = motor_power;

	while(nMotorEncoder[motorA] < distance*TO_COUNTS)
	{
			if (SensorValue[2] <= x)
			{
				manouver_obstacle(motor_power);
			}
			else if (SensorValue[S1] <= x && SensorValue[S2] >= x)
			{
					catch_person();
					return 1;
			}

	}

	motor[motorA] = motor[motorD] = 0;
	wait1Msec(500);
	return 0;
}

/*
		Only on the return path will the robot have to worry about sensing the black type
		that marks the middle, therefore, I'm creating a seperate function to drive back to
		the centre. Otherwise, we would have a single drive function checking the colour sensor
		while its out and about on the bouphostredon, which is inefficient.
*/
void drive_return(int distance, int motor_power)
{
		int x = 50;
		const int TO_COUNTS = 180/(PI*2.75);

		nMotorEncoder[motorA] = 0;
		motor[motorA] = motor[motorD] = motor_power;

		while(nMotorEncoder[motorA] < distance*TO_COUNTS)
		{
				//The color sensor
				if (SensorValue[3] == 1)
				{
					return;
				}
		}

		motor[motorA] = motor[motorD] = 0;
		wait1Msec(500);
		return;
}



/*
	every turn should increment, based on how it moves. It needs to change the direction based on what the gyro value is.
*/

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

void bouphostredon(int motor_power, int length, int width, int quadrant)
{
		int found_person = 0;

		int count_13 = 0;
		int count_24 = 1;
		if (quadrant == 1 || quadrant == 3)
		{
				count_13 = 1;
				count_24 = 0;
		}


		while( count_13 < 5 && count_24 < 5)
		{
				if (count_13 % 2 == 1)
				{

							found_person = drive_path(length, motor_power);
							if (found_person == 1) return;
							rotate(1, motor_power, 90);

							found_person = drive_path(width, motor_power);
							if (found_person == 1) return;
							rotate(1, motor_power, 90);

				}
				else
				{
							found_person = drive_path(length, motor_power);
							if (found_person == 1) return;
							rotate(0, motor_power, 90);

							found_person = drive_path(width, motor_power);
							if (found_person == 1) return;
							rotate(0, motor_power, 90);
				}
				++count_13;
				++count_24;
		}
}


task main()
{
	const int ROTATE_90 = 90;
	configure_all_sensors();



}
