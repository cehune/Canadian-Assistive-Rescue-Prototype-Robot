#include "PC_FileIO.c"

void configure_all_sensors();
int drive_path(int distance, int motor_power);
void rotate(bool dir, int motor_power);
void rotate_to_begin(int quadrant, int motor_power);
void return_to_base();
void manouver_obstacle(int motor_power);

/*
ADJUST WHEEL SIZE ON ALL FUNCTIONS

*/
void Input (TFileHandle & fin, char *name, int *location, int *time_last_seen)
{

	int count = 0;
	while(count < 4)
	{
		char current_name = ' ';
		int current_location = 0;
		int time = 0;
		readCharPC(fin, current_name);
		name[count] = current_name;

		readIntPC(fin, current_location);
		location[count] = current_location;

		readIntPC(fin, time);
		time_last_seen[count] = time;
	//	Location[Name] = Quadrant;
		//TimeLastSeen[Name] = Time;
		++count;
	}

	return;
}

/*
	should call this everytime it saves someone, feeding in that persons index
	in the names and pick_up_time arrays
*/
void Output (TFileHandle & fout, char *name, float *pick_up_time, int patient_num)
{

	string exposure_sentence = " was exposed for ";
	string minutes_word = "minutes";
	int patient = name[patient_num];
	int time_taken = pick_up_time[patient_num];
		writeCharPC(fout, patient);
		writeTextPC(fout, exposure_sentence);
		writeFloatPC(fout, time_taken);
		writeTextPC(fout, minutes_word);
		writeEndlPC(fout);

}




void calculate_order(int *last_time_seen, int *order)
{
	int temp = 0;
	int temp2 = 0;
	int times_copy[4];
	for (int i = 0; i < 4; ++i)
	{

		times_copy[i] = last_time_seen[i];
	}


	for (int i = 0; i < 3; i++) {
        for (int j = i + 1; j < 4; j++) {
            if (times_copy[i] < times_copy[j]) {

                temp = times_copy[j];
                times_copy[j] = times_copy[i];
                times_copy[i] = temp;
                temp2 = order[j];
                order[j] = order[i];
                order[i] = temp2;
            }
        }
    }
	return;
}

void calculate_exposure (int time_to_save, int *times_last_seen, int *updated_times, int patient_num)
{
	updated_times[patient_num] = times_last_seen[patient_num] + time_to_save;
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

		rotate(1, motor_power, -90);
		drive_path(y, motor_power);

		rotate(1, motor_power, -90);
		drive_path(x, motor_power);

		rotate(0, motor_power, 90);

		wait1Msec(500);
		nMotorEncoder[motorA] = current_counts + (2*x*180/(PI*2.75));
}

/*
	I am not calling a drive function here, because if I call drive path it will make create
	an unecessary back and forth between the two functions, as drive_path will continuoutsly recall
	catch_person because the person is close.

	Also we can afford toreset the motor encoder count here becayse it is returning after this function call.

	distance variable should be equal to same amount that the distance away from the person
	that the ir sensor senss\es them
*/
void catch_person( int motor_power)
{
	nMotorEncoder[motorB] = 0;
	motor[motorB]= -5;
	while(nMotorEncoder[motorB] > -105)
	{}
	motor[motorB]= 0;
	wait1Msec(1000);

	nMotorEncoder[motorA] = 0;
	int const distance = 50;
	int const distance_counts = distance * 180 / (2.75 * PI);
	motor[motorA] = motor[motorD] = motor_power;
	while(nMotorEncoder[motorA] < distance_counts)
	{}

		motor[motorB]= 7;
	while(nMotorEncoder[motorB] < 15)
	{}

	return;
}

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
					catch_person(motor_power);
					return 1;
			}

	}

	motor[motorA] = motor[motorD] = 0;
	wait1Msec(500);
	return 0;
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

/*
after rotating to face the x axis, it should drive until it hits the coloured tape border,
then rotate to face the center until it gets to the center square, marked by the black tape then drive a little
more to get to the true origin.

make sure to adjust rotation speed.

*/
void return_to_beginning (int motor_power, int quadrant)
{

		int x = 50;
		const int TO_COUNTS = 180/(PI*2.75);
		int border = 0;
		int rotation_dir = 0;
		int rotation_angle = 90;
		/* for which direction it turns to face the center once
		it reaches the x axis. If its 1 or 3, then it should turn right. It should turn left
		if it is on quadrant 2 or 4. rotation should be 1 for counter clockwise and 0 for clockwise.

		*/

		if (quadrant == 1)
		{
				border = 3; // int for the colour green

		}
		else if (quadrant == 2)
		{
				border = 5; //int for the colour red
				rotation_dir = 1;
				rotation_angle = -90;
		}
		else if (quadrant == 3)
		{
				border = 4; // int for the colour yellow
		}
		else
		{
				border = 2; // int for the colour blue
				rotation_dir = 1;
				rotation_angle = -90;
		}

		nMotorEncoder[motorA] = 0;
		motor[motorA] = motor[motorD] = motor_power;

		while(SensorValue[S3] != border)
		{}

		motor[motorA] = motor[motorD] = 0;
		wait1Msec(1000);
		rotate(rotation_dir, 15, rotation_angle);
		wait1Msec(1000);
		motor[motorA] = motor[motorD] = motor_power;

		/*
				The int corresponds to the colour black, which is how we're marking the center;
		*/
		while(SensorValue[S3] != 1)
		{}

		motor[motorA] = motor[motorD] = 0;
		wait1Msec(1000);
		nMotorEncoder[motorA] = 0;
		motor[motorA] = motor[motorD] = motor_power;

		while(nMotorEncoder[motorA] < (x * 180 / (PI * 2.80)))
		{}
		wait1Msec(500);

		if (quadrant == 1)
		{
					motor[motorA] = -motor_power;
					motor[motorD] = motor_power;

					while(SensorValue[S4] < 360)
					{}

					SensorValue[S4] = 0;
		}
		else if (quadrant = 4)
		{
					motor[motorA] = -motor_power;
					motor[motorD] = motor_power;

					while(SensorValue[S4] < 0)
					{}
		}

		else
		{
					motor[motorA] = motor_power;
					motor[motorD] = -motor_power;

					while(SensorValue[S4] > 0)
					{}
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

	// initialization
	const int ROTATE_90 = 90;
	const int Robot_Length = 32;
	const int Robot_Width = 18;
	const int Patient_Size = 2.75; // coke can radius
	// const int Obsticle_Size = ; // party cup radius


	char people[4] = {'A', 'B', 'C', 'D'};
	int quadrant [4] = {0, 0, 0, 0};
	int times [4] = {0,0,0,0};
	int order[4] = {1, 2, 3, 4};

	configure_all_sensors();

	// Open the file
	TFileHandle fin;
	bool ReadCheck = openReadPC(fin, "Peole_To_Save.txt");

	// check if the file is opened correctly
	if (!ReadCheck)
	{
		displayString(5, "Error: Fail to open file");
		wait1Msec(5000);
	}


	Input(fin, people, quadrant, times);
	calculate_order(times, order);


	// open
	TFileHandle fout;
	bool WriteCheck = WriteTextPC(fout, "Report.txt");


	// check if the file is opened correctly



	// check if the file is opened correctly

	if (!WriteCheck)
	{
		displayString(5, "Error: Fail to open file");
		wait1Msec(5000);
	}


	//writeFloatPC(fout, pickuptime);
	//writeEndlPC(fout);


	closeFilePC(fout);



	closeFilePC(fout);

}
