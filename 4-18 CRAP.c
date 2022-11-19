#include "PC_FileIO.c"

void Input (TFileHandle & fin, char *name, int *location, int *time_last_seen);
void Output (TFileHandle & fout, char name, float time_taken, int found_person);
void calculate_exposure (float time_to_save, int time_last_seen, float *updated_times, int patient_num);
void configure_all_sensors();
void rotate(bool dir, int motor_power, int angle);
void manouver_obstacle(int motor_power_drive, int motor_power_rotate);
void catch_person( int motor_power);
int drive_path(int distance, int motor_power_drive, int motor_power_rotate);
void drive_dist (int distance, int motor_power);
void rotate_to_begin(int quadrant, int motor_power);
void return_to_begin (int motor_power_drive, int motor_power_rotate, int quadrant);
void exit_centre(int motor_power_drive, int motor_power_rotate, int quadrant);
int bouphostredon(int motor_power_drive, int motor_power_rotate, int length, int width, int quadrant);



/*
		Fills arrays witht the information of the patients

		Parameters
				TFileHandle & fin: Reference to the input file
				char *name: Array that will store Patient nams
				int *location: Array that will store the quadrants each person
						is in
				int *time_last_seen: Array that will store the amount of time in
						minutes that the person has been lost for

		Note
				Reads input file and puts that information into the given parallel arrays
*/
void Input (TFileHandle & fin, char *name, int *location, int *time_last_seen)
{

	int count = 0;
	while(count < 4)
	{
		/*
				Creating variables to read the file info into.
				Then we set the name, location, and time_last_seen
				arrays at the index of count as the variables we just
				created.
		*/
		char current_name = ' ';
		int current_location = 0;
		int time = 0;
		readCharPC(fin, current_name);
		name[count] = current_name;

		readIntPC(fin, current_location);
		location[count] = current_location;

		readIntPC(fin, time);
		time_last_seen[count] = time;

		++count;
	}

	return;
}

/*
			Outputs a single persons name and the time that they were exposed to the weather for.
			This is vital for doctors reference.

			Parameters
					TFileHandle & fout: A reference to the output file
					char name: The patients name
					float time_taken: The total time that a person was exposed
							to the weather for.
					int found_person: An integer representing whether the robot actually found
							a person or not.

			Notes
					The int found_person is set to 1 if the robot found a person during its
					bouphostredon journey. If it finishes the bouphotredon path function
					without finding someone, then found_person is set to 0, meaning
					the robot could not find a person.

*/
void Output (TFileHandle & fout, char name, float time_taken, int found_person)
{
	if (found_person == 1)
	{
		string exposure_sentence = " was exposed for ";
		string minutes_word = "minutes";

		writeCharPC(fout, name);
		writeTextPC(fout, exposure_sentence);
		writeFloatPC(fout, time_taken);
		writeTextPC(fout, minutes_word);
		writeEndlPC(fout);
	}

	/*
			Catch for if the robot does not find a person.

			This part of the function should never happen. If it does, then the
			function outputs that it could not find the person, and that they are
			still lost out in the wilderness.
	*/
	else
	{
			string couldnt_find_person = "could not find ";
			writeCharPC(fout, name);
			writeEndlPC(fout);
	}
		return;
}



/*
		This is a selection sorting algorithm to calculate the order that the robot should
		save people.

		Parameters
				int *last_time_seen: The array containing the last time that each person was seen,
						given by the input file.
				int *order: An array we are updating to contain the indexes of people in the order
						of largest time_last_seen to smallest.


		Notes
				We initialize the order array as {0, 1, 2, 3}, representing the index of each values.
				The function makes a copy of the values of the last_time_seen array, and sorts that
				copy as to not edit the original. This copy is called times_copy.

				While sorting times_copy, the function rearranges the order array with the same switches.

				This means that by the end, while the times_copy array is sorted from greatest to least,
				the order array holds the indexes of the greatest value in last_time_seen to least.

				If last_time_seen = {5, 7, 8, 2}
				times_copy would sort to {8, 7, 5, 2}
				and order would rearrange to {2, 1, 0, 3}

				The value at the first index of order, 2, is the index of last_time_seen with the greatest
				value. The value at the second index, 1, represents the index of the second largest value
				in last_time_seen, 7.

*/
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

                /*
                		Using temporary variables to temporarily store the value of an index
                		while we switch it with the position of another.
                */
                temp2 = order[j];
                order[j] = order[i];
                order[i] = temp2;
            }
        }
    }
	return;
}


/*
		Calculates the time a person was exposed to the winter.

		Parameters
				float time_to_save: The time taken for the robot to save the person in seconds.
						we treat these as minutes for our simulation (10 seconds our time is 10 minutes)
						for the simulation.
				int time_last_seen: The time last seen given by the input file
				float *updated_times: An array containing the total exposure time. The function updates this
						to contain what it calculates as the total exposure time.
				int patient_num: The index corresponding to the persons name and other information.

		Notes
				The function adds the time_last_seen and time_to_save, and adds these values to
				the updated times array at the index of patient_num
*/
void calculate_exposure (float time_to_save, int time_last_seen, float *updated_times, int patient_num)
{
	updated_times[patient_num] = time_last_seen + time_to_save;
	return;
}


//Config
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

/*
	Rotates the robot at a given angle.

	Parameters:
			int dir: 1 or 0. If given 1, it turns counter clockwise. If given
					0, it turns clockwise.

			int motor_power: Controls the rate of the robot motors. Has to be a
					lower amount, or else the rotation is less accurate.

			int angle: Must be given an integer angle in degrees. If it is turning
					counter clockwise (dir = 1), then it must be negative, because
					the gyro counts would be decreasing. If it is turning clockwise
					then angle should be given a positive value.

	Notes
			We are not allowed to reset the gyro whenever it rotates, as we
			need to keep a relative gyro count to return to the center. This is
			further explained in the rotate_to_begin function.

			It tracks the current gyro value using the variable current_dir. Then
			it adds the given angle (would subtract if the angle is negative), and
			turns until the gyro values reachs that sum.


*/
void rotate(bool dir, int motor_power, int angle)
{
	//current gyro value
	int  current_dir = abs(SensorValue(S4));
	motor[motorA]=(dir*1+!dir*-1)*motor_power;
	motor[motorD]=(!dir*1+dir*-1)*motor_power;

	//If it is turning clockwise, the gyro will increase.
	//Therefore, the sensor value turns while it is less than the sum
	//of current_dir and the angle.
	if (dir == 0)
	{
			while (SensorValue[S4] < current_dir  + angle)
			{	}
	}

	//Opposite as the comment above.
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
		Moves around an obstacle with a series of rotations.

		Parameters:
			int motor_power_drive: The motor power when it is driving forward.
			int motor_power_rotate: The motor power for when the robot is rotating.
					It should be lower to prevent innacuracies.

		Notes
			This function is called when the sensors in drive_path sense an obstacle.
			The robot makes a right turn and drives a little, perpindicular to its original
			path. Then it makes a left turn and drives past the obstacle, parallel
			to its original path. Then the robot does another left turn, and drives
			to return to its path. It
			then does a right turn to face its original direction, and then it can
			continue driving

			The gyro counts should not have a net change after this function is called
			because the rotations cancel eachother out.

			Lastly, the function deletes the motor encoder counts when driving away and towards
			the original path. It keeps the encoder counts from when it was driving past the obstacle
			parallel to its original path because it would've traveled that distance
			regardless, should it have not encountered an obstacle.
*/
void manouver_obstacle(int motor_power_drive, int motor_power_rotate)
{
		int x = 50;
		int y = 100;

		wait1Msec(500);
		int current_counts = nMotorEncoder[motorA];

		//right turn and drive
		rotate(0, motor_power_rotate, 90);
		drive_dist(x, motor_power_drive);


		//left turn and drive past obstacle
		rotate(1, motor_power_rotate, -90);
		drive_dist(y, motor_power_drive);


		//left turn to return to path
		rotate(1, motor_power_rotate, -90);
		drive_dist(x, motor_power_drive);


		//right turn, faces its original path
		rotate(0, motor_power_rotate, 90);

		wait1Msec(500);

		//reassigns the motor encoder by subtracting the counts
		//while traveling away and toward its original path
		nMotorEncoder[motorA] = current_counts + (2*x*180/(PI*2.75));
}



/*
		Opens the gate and receives the person, then closes the gate
		to secure them

		Parameters
				int motor_power: The motor power when it is driving forward.

		Notes
			This function is called when the sensors sense a person in front
			of it.

			The gate will open first, controlled by the medium motor, motorB.
			The robot then drives forward to receive the person.
			The gate then closes, and the function ends, so that the robot can
			safely return them to the base.

			Should you modify the power for motor B, it must be very low, or the
			gate will not properly open.
*/
void catch_person( int motor_power)
{
	//opens gate.
	nMotorEncoder[motorB] = 0;
	motor[motorB]= -5;
	while(nMotorEncoder[motorB] > -105)
	{}
	motor[motorB]= 0;
	wait1Msec(1000);

	//drives forward until the person is inside of the safety bed.
	nMotorEncoder[motorA] = 0;
	int const distance = 50;
	int const distance_counts = distance * 180 / (2.75 * PI);
	motor[motorA] = motor[motorD] = motor_power;
	while(nMotorEncoder[motorA] < distance_counts)
	{}

	//gate closes.
	motor[motorB]= 7;
	while(nMotorEncoder[motorB] < 15)
	{}

	return;
}

/*
		Drives a section of the bouphostredon

		Parameters
				int distance: Distance that the robot should drive
				int motor_power_drive: The motor power when driving forward
				int motor_power_rotate: The motor power when rotating. Must be low
						to prevent innacuracies.

		Notes
				The robot drives until it reaches a certain distance, and constantly
				checks while driving if there is an obstacle or human in front of it.

				If it finds an obstacle, it will simply manouver around it, calling the
				manouver_obstacle function. If its a human, it will call the catch_person
				function, and end prematurely.

				If it doesn't find either, it just travels the full distance.

*/

int drive_path(int distance, int motor_power_drive, int motor_power_rotate)
{
	int x = 50;
	const int TO_COUNTS = 180/(PI*2.75);

	nMotorEncoder[motorA] = 0;
	motor[motorA] = motor[motorD] = motor_power_drive;

	while(nMotorEncoder[motorA] < distance*TO_COUNTS)
	{
			if (SensorValue[S2] <= x)
			{
				//checks for obstacles using the ultrasonic sensor
				manouver_obstacle(motor_power_drive, motor_power_rotate);
			}

			//If the ir sensor senses a something close, but the ultrasonic doesnt
			//Then we know it is a human, because the humans are shorter than the
			//height of the ultrasonic sensor.
			else if (SensorValue[S1] <= x && SensorValue[S2] >= x)
			{
					catch_person(motor_power_drive);
					return 1;
			}

	}

	motor[motorA] = motor[motorD] = 0;
	wait1Msec(500);
	return 0;
}

/*
		Basic drive distance function. Drives a certain distance.

		Parameters
				int distance: Distance that the robot should drive
				int motor_power: The motor power when driving forward

		Notes
				This is similar to the drive_path function, but is for the
				functions that do not need to check for people / checking for
				people or obstacles could bring in sensing errors.

				Using the motor encoder of motor D because the manouver obstacle
				function needs to call this, and it will mess with tracking the
				path if we use motor A.
*/
void drive_dist (int distance, int motor_power)
{
	nMotorEncoder[motorD] = 0;
	motor[motorA] = motor[motorD] = motor_power;

	while (nMotorEncoder[motorD] < distance * 180 / (PI * 2.80))
	{}

	motor[motorA] = motor[motorD] = 0;
	wait1Msec(500);
	return;
}




/*
		No matter which direction the robot is facing, this function rotates it to face the x axis.

		Parameters
				int quadrant: The persons quadrant, whichever one the robot is currently in
				int motor_power: The motor power when rotating

		Notes

*/

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
void return_to_begin (int motor_power_drive, int motor_power_rotate, int quadrant)
{

		int x = 50;

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
		motor[motorA] = motor[motorD] = motor_power_drive;

		while(SensorValue[S3] != border)
		{}

		motor[motorA] = motor[motorD] = 0;
		wait1Msec(1000);
		rotate(rotation_dir, motor_power_rotate, rotation_angle);
		wait1Msec(1000);
		motor[motorA] = motor[motorD] = motor_power_drive;

		/*
				The int corresponds to the colour black, which is how we're marking the center;
		*/
		while(SensorValue[S3] != 1)
		{}

		motor[motorA] = motor[motorD] = 0;
		wait1Msec(1000);
		nMotorEncoder[motorA] = 0;
		motor[motorA] = motor[motorD] = motor_power_drive;

		while(nMotorEncoder[motorA] < (x * 180 / (PI * 2.80)))
		{}
		wait1Msec(500);

		if (quadrant == 1)
		{
					motor[motorA] = -motor_power_rotate;
					motor[motorD] = motor_power_rotate;

					while(SensorValue[S4] < 360)
					{}

					SensorValue[S4] = 0;
		}
		else if (quadrant == 4)
		{
					motor[motorA] = -motor_power_rotate;
					motor[motorD] = motor_power_rotate;

					while(SensorValue[S4] < 0)
					{}
		}

		else
		{
					motor[motorA] = motor_power_rotate;
					motor[motorD] = -motor_power_rotate;

					while(SensorValue[S4] > 0)
					{}
		}

		return;
}

void exit_centre(int motor_power_drive, int motor_power_rotate, int quadrant)
{

	if (quadrant == 3 || quadrant == 4)
	{
				rotate(0, motor_power_rotate, 180);
	}

	motor[motorA] = motor[motorD] = motor_power_drive;
	while(SensorValue[S4] != 1)
	{}

	if (quadrant == 1 || quadrant == 3)
	{
			rotate(0, motor_power_rotate, 90);
	}
	else if (quadrant == 2 || quadrant == 4)
	{
			rotate(1, motor_power_rotate, -90);
	}
	motor[motorA] = motor[motorD] = 0;
	nMotorEncoder[motorA] = 0;
	wait1Msec(500);

	motor[motorA] = motor[motorD] = motor_power_drive;
	while(nMotorEncoder[motorA] < (10 * 180 / (2.80 * PI)))
	{}
	motor[motorA] = motor[motorD] = 0;
	wait1Msec(500);

//leads into the bouphostredon function
	return;
}


int bouphostredon(int motor_power_drive, int motor_power_rotate, int length, int width, int quadrant)
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

							found_person = drive_path(length, motor_power_drive, motor_power_rotate);
							if (found_person == 1) return 1;
							rotate(1, motor_power_rotate, 90);

							found_person = drive_path(width, motor_power_drive,  motor_power_rotate);
							if (found_person == 1) return 1;
							rotate(1, motor_power_rotate, 90);

				}
				else
				{
							found_person = drive_path(length, motor_power_drive, motor_power_rotate);
							if (found_person == 1) return 1;
							rotate(0, motor_power_rotate, 90);

							found_person = drive_path(width, motor_power_drive, motor_power_rotate);
							if (found_person == 1) return 1;
							rotate(0, motor_power_rotate, 90);
				}
				++count_13;
				++count_24;
		}
		return 0;
}


task main()
{

	// initialization
	/*const int ROTATE_90 = 90;
	const int Robot_Length = 32;
	const int Robot_Width = 18;
	const int Patient_Size = 2.75; // coke can radius*/
	// const int Obsticle_Size = ; // party cup radius
	const int MOTOR_POWER_DRIVE = 50;
	const int MOTOR_POWER_ROTATE = 15;
	const int BOUPHOSTREDON_LENGTH = 100;
	const int BOUPHOSTREDON_WIDTH = 20;

	//all data arrays
	char people[4] = {'A', 'B', 'C', 'D'};
	int location[4] = {0, 0, 0, 0};
	int times [4] = {0,0,0,0};
	int order[4] = {0, 1, 2, 3};
	float updated_times [4] = {0,0,0,0};

	configure_all_sensors();

	// Opening and error handling the reading file
	TFileHandle fin;
	bool ReadCheck = openReadPC(fin, "Peole_To_Save.txt");

	if (!ReadCheck)
	{
		displayString(5, "Error: Fail to open file");
		wait1Msec(5000);
	}

	Input(fin, people, location, times);
	calculate_order(times, order);

	// Error handling and opening the writing file
	TFileHandle fout;
	bool WriteCheck = writeTextPC(fout, "Report.txt");

	if (!WriteCheck)
	{
		displayString(5, "Error: Fail to open file");
		wait1Msec(5000);
	}

	for (int count = 0; count < 4; ++count)
	{
		int quadrant = location[order[count]];
		int name = people[order[count]];
		int time_last_seen = times[order[count]];
		time1[T1] = 0;
		int saved_person = 0;
		float time = 0;

		exit_centre(MOTOR_POWER_DRIVE, MOTOR_POWER_ROTATE, quadrant);

	 	saved_person = bouphostredon(MOTOR_POWER_DRIVE, MOTOR_POWER_ROTATE,
	 			BOUPHOSTREDON_LENGTH, BOUPHOSTREDON_WIDTH, quadrant);

	 	rotate_to_begin(quadrant, MOTOR_POWER_ROTATE);
	 	return_to_begin(MOTOR_POWER_DRIVE, MOTOR_POWER_ROTATE, quadrant);

    time = time1[T1];
    time /= 1000;

    calculate_exposure(time, time_last_seen, updated_times, order[count]);
    Output(fout, name, updated_times[order[count]], saved_person);

	}

	closeFilePC(fout);
	closeFilePC(fout);

}
