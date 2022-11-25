#include "../PC_FileIO.c"

/*void Input (TFileHandle & fin, char *name, int *location, int *time_last_seen);
void Output (TFileHandle & fout, char name, float time_taken, int found_person);
void calculate_exposure (float time_to_save, int time_last_seen, float *updated_times, int patient_num);*/
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


void Input (TFileHandle & fin, int & id, int & location, int & time_last_seen)
{

		/*
				Creating variables to read the file info into.
				Then we set the name, location, and time_last_seen
				arrays at the index of count as the variables we just
				created.
		*/
		int current_id = 0;
		int current_location = 0;
		int time = 0;

		readIntPC(fin, current_id);
		id = current_id;

		readIntPC(fin, current_location);
		location = current_location;

		readIntPC(fin, time);
		time_last_seen = time;
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


void Output (TFileHandle & fout, int & id, int & time_taken, const int found_person)
{
	if (found_person == 1)
	{
		string exposure_sentence = " was exposed for ";
		string minutes_word = "minutes";
		char space = ' ';
		writeLongPC(fout, id);
		writeTextPC(fout, exposure_sentence);
		writeLongPC(fout, time_taken);
		writeCharPC(fout, space);
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
			writeLongPC(fout, id);
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
void calculate_exposure (const int time_to_save, const int time_last_seen, int & updated_times)
{
	updated_times = time_last_seen + time_to_save;
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
	int  current_dir = (SensorValue(S4));
	int next_angle = current_dir + angle;
	//If it is turning clockwise, the gyro will increase.
	//Therefore, the sensor value turns while it is less than the sum
	//of current_dir and the angle.
	if (dir == 0)
	{
				motor[motorA]=-motor_power;
			motor[motorD]=motor_power;
			while (SensorValue[S4] < next_angle)
			{	}
	}

	//Opposite as the comment above.
	else
	{

				motor[motorA]=motor_power;
	motor[motorD]=-motor_power;
			while ((SensorValue[S4]) > next_angle)
			{	}
	}


	motor[motorA]=0;
	motor[motorD]=0;
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

*/
void manouver_obstacle(int motor_power_drive, int motor_power_rotate)
{
		int x = 15;
		int y = 55;

		wait1Msec(500);


		//right turn and drive
		rotate(0, motor_power_rotate, 85);
		drive_dist(x, motor_power_drive);


		//left turn and drive past obstacle
		rotate(1, motor_power_rotate, -85);
		drive_dist(y, motor_power_drive);


		//left turn to return to path
		rotate(1, motor_power_rotate, -85);
		drive_dist(x, motor_power_drive);


		//right turn, faces its original path
		rotate(0, motor_power_rotate, 85);
		motor[motorA] = motor[motorD] = 0;
		wait1Msec(2000);


	return;
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
	motor[motorA] = motor[motorD] = 0;
	wait1Msec(1000);

	nMotorEncoder[motorB] = 0;
	motor[motorB]= -5;
	while(nMotorEncoder[motorB] > -110)
	{}
	motor[motorB]= 0;
	wait1Msec(1000);

	//drives forward until the person is inside of the safety bed.
	nMotorEncoder[motorA] = 0;
	int const distance = 30;
	int const distance_counts = distance * 180 / (2.75 * PI);

	motor[motorA] = motor[motorD] = motor_power;
	while(SensorValue[S1] > 2 && nMotorEncoder[motorA] < distance_counts)
	{}

		motor[motorA] = motor[motorD] = 0;
	wait1Msec(500);
	//gate closes.
	motor[motorB]= 7;
	while(nMotorEncoder[motorB] < 20)
	{}

	motor[motorB] = 0;
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
				manouver_obstacle function. We were only able to make this work
				by returning prematurely, so there absolutely cannot be a person behind
				an obstacle on the same stretch.

				If its a human, it will call the catch_person
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

			//If the ir sensor senses a something close, but the ultrasonic doesnt
			//Then we know it is a human, because the humans are shorter than the
			//height of the ultrasonic sensor.

			if (SensorValue[S1] <= x && SensorValue[S2] >= 80)
			{
					catch_person(motor_power_drive);
					return 1;
			}

			if (SensorValue[S2] <= 50)
			{
				//checks for obstacles using the ultrasonic sensor
				motor[motorA] = motor[motorD] = 0;
				wait1Msec(500);
				int current = nMotorEncoder[motorA];
				int to_travel = ((distance * TO_COUNTS) - (50 * TO_COUNTS) - current) * (2.80 * PI) / 180;

				manouver_obstacle(motor_power_drive, motor_power_rotate);
				nMotorEncoder[motorA] = current;
				wait1Msec(500);

				drive_dist(to_travel, motor_power_drive);
				return 0;
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
				Because we can never tell what the direction the robot will be facing when it
				finds a person, we used a relative gyro degree to determine the direction that
				the robot is facing in comparison to its starting position.

				We set the gyro at 0 when it is in the center directly looking down the y axis.

				This means that if its facing any direction, it just needs to spin until the gyro
				reaches 0 or any scalar multiple  of 360 if it wanted to face that same start direction.

				Just by nature of the bouphostredon path, the gyro degrees should never exceed 270 or go
				below -270, but we catch for those situations anyway, in case there are innacuracies.

				If it is in quadrant 1 or 2, the x axis is below the robot, and it must spin until
				it reaches 180 degrees, facing down. If it is in quadrant 3 or 4, it will rotate
				until it faces upwards, the same direction as the starting reference angle.
*/

void rotate_to_begin(int quadrant, int motor_power)
{
	wait1Msec(2000);
	//Tracks the current angle
	int current_dir = SensorValue[S4];
	const int DOWNWARD = 170;

			if (current_dir > DOWNWARD)
			{
					//get lowest angle below 180 that faces the same direction.
					SensorValue[S4] = current_dir - 360;
			}
	//The x axis is below the robot, and it must face 180 or 180 +- a scalar multiple
	//of 260.
	if (quadrant == 1 || quadrant == 2)
	{
			/*Either the angle is below 170 or below */


			motor[motorA] = -motor_power;
			motor[motorD] = motor_power;

			if (current_dir < -DOWNWARD)
			{
					while(SensorValue[S4] < -DOWNWARD)
					{}
				motor[motorA] = motor[motorD] = 0;
					SensorValue[S4] = DOWNWARD;
			}
			else if (current_dir < DOWNWARD)
			{

					/*the gyro is above 0 degrees, should spin until it faces
					directly down, which is any scalar multiple of 180*/
					while(SensorValue[S4] < DOWNWARD)
					{}
				motor[motorA] = motor[motorD] = 0;
			}
	}

	//x axis is above the robot
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
				motor[motorA] = motor[motorD] = 0;
			}
			else if (current_dir < 360)
			{
					while(SensorValue[S4] < 360)
					{}
					motor[motorA] = motor[motorD] = 0;
					SensorValue[S4] = 0;
			}
	}

	wait1Msec(500);

	return;
}

/*
Drive until it hits the x axis then returns to the centre.

Parameters
		int motor_power_drive: The motor power for driving forward.
		int motor_power_rotate: The motor power for rotation
		int quadrant: The quadrant that the person was in

Notes
		This is called right after the rotate_to_begin function, so it is
		already facing the x axis. The robot drives until the color sensor senses the
		associated border colour.

		After, it turns to face the y axis. If its in quadrant 1 or 3, it must turn counter
		clock wise. If its in quadrant 2 or 4, then it must turn clockwise.

		Then it drives until the colour sensor detects black, the center colour.

		Once it does, it drives a set distance forward, because we know the size of the center.

		It then rotates to look upward along the y axis, facing the original reference direction.

*/
void return_to_begin (int motor_power_drive, int motor_power_rotate, int quadrant)
{

		int x = 50;

		int border = 4;
		int center = 1;
		int rotation_dir = 0;
		int rotation_angle = 90;
		/* for which direction it turns to face the center once
		it reaches the x axis. If its 1 or 3, then it should turn right. It should turn left
		if it is on quadrant 2 or 4. rotation should be 1 for counter clockwise and 0 for clockwise.

		*/

		if (quadrant == 1)
		{
				border = 3; // int for the colour green
				center = 5;

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
		else if (quadrant == 4)
		{
				border = 2; // int for the colour blue
				rotation_dir = 1;
				rotation_angle = -90;
		}

		nMotorEncoder[motorA] = 0;
		motor[motorA] = motor[motorD] = motor_power_drive;

		while(SensorValue[S3] != 5)
		{}

		motor[motorA] = motor[motorD] = 0;
		wait1Msec(3000);
		drive_dist(10, motor_power_drive);

		//rotates to face the center
		rotate(rotation_dir, motor_power_rotate, rotation_angle);
		wait1Msec(1000);
		motor[motorA] = motor[motorD] = motor_power_drive;

		/*
				The int corresponds to the colour black, which is how we're marking the center;
		*/
		while(SensorValue[S3] != 3)
		{}

		motor[motorA] = motor[motorD] = 0;
		wait1Msec(1000);
		nMotorEncoder[motorA] = 0;
		drive_dist(15, motor_power_drive);
		wait1Msec(500);

		/*
				depending on the direction its facing, it needs to turn until it faces the
				reference angle 0.
		*/
		if (quadrant == 1)
		{
					motor[motorA] = -motor_power_rotate;
					motor[motorD] = motor_power_rotate;

					while(SensorValue[S4] < 360)
					{}

					SensorValue[S4] = 0;

					/*
							The gyro degrees should be ~270 at this point, but it only needs to
							turn 90 to face the reference direction. Therefore, we make it turn to
							an angle of 360, then just reset the gyro value to 0;
					*/
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
					/*
							Coming from either quadrant 2 or 3, the robot will have a gyro degree
							of 90. Therefore, it will rotate the same direction.
					*/
					motor[motorA] = motor_power_rotate;
					motor[motorD] = -motor_power_rotate;

					while(SensorValue[S4] > 0)
					{}
		}
		motor[motorA] = motor[motorD] = 0;
		return;
}

void release_person()
{
		nMotorEncoder[motorB] = 0;
	motor[motorB]= -5;
	while(nMotorEncoder[motorB] > -110)
	{}
	motor[motorB]= 0;
	wait1Msec(8000);


	//gate closes.
	motor[motorB]= 7;
	while(nMotorEncoder[motorB] < 20)
	{}
	motor[motorB]= 0;
	return;
}



/*
		Exits the center.

		Parameters
				int motor_power_drive: The motor power for driving forward.
				int motor_power_rotate: The motor power for rotation
				int quadrant: The quadrant that the person is in

		Notes
				If the person is in quadrant 3 or 4, the robot does a 180 degree turn first. Then it drives
				forward, and turns again to face the quadrant the person is in.


				This function leads directly into the bouphostredon function.
*/

void exit_centre(int motor_power_drive, int motor_power_rotate, int quadrant)
{

	SensorValue[S4] = 0;

	if (quadrant == 3 || quadrant == 4)
	{
				rotate(0, motor_power_rotate, 180);
	}

	motor[motorA] = motor[motorD] = motor_power_drive;
	while(SensorValue[S3] != 5 && SensorValue[S3] != 3)
	{}

	motor[motorA] = motor[motorD] = 0;
	wait1Msec(500);



	if (quadrant == 1 || quadrant == 3)
	{
			rotate(0, motor_power_rotate, 85);

	}
	else if (quadrant == 2 || quadrant == 4)
	{

			rotate(1, motor_power_rotate, -85);
	}


	motor[motorA] = motor[motorD] = 0;
	wait1Msec(1000);
	nMotorEncoder[motorA] = 0;

	drive_dist(10, motor_power_drive);


//leads into the bouphostredon function
	return;
}



/*
		Full Bouphostredon path to traverse the quadrant

		Parameters
				int motor_power_drive: Motor power for driving forward
				int motor_power_rotate: Motor power for rotating
				int length: length of the bouphostredon
				int width: width of the bouphostredon path
				int quadrant: The quadrant the person is in
*/
int bouphostredon(const int motor_power_drive, const int motor_power_rotate,
									const int length, const int width, const int quadrant)
{
		int found_person = 0;

		/*
				Using 2 count variables to track the amount of turns the robot will take.

				We wanted the robot to just generally turn a certain direction based on
				the value of count, and whether it was even or odd. Unfortunately,
				if we just set it to 0 or 1 depending on the quadrant, and kept the value  of
				count as the loop condition, then the qaudrants where count was set as 1 would
				end one iteration early. Therefore, we created another count variable to mediate
				this issue.
		*/
		int count_13 = 1;
		int count_24 = 0;
		if (quadrant == 2 || quadrant == 4)
		{
				count_13 = 0;
				count_24 = 1;
		}


		while( count_13 < 3 && count_24 < 3)
		{
				if (count_13 % 2 == 1)
				{

							found_person = drive_path(length, motor_power_drive, motor_power_rotate);



							if (found_person == 1) return 1;
							rotate(1, motor_power_rotate, -85);



							found_person = drive_path(width, motor_power_drive,  motor_power_rotate);


							if (found_person == 1) return 1;
							rotate(1, motor_power_rotate, -85);
				}
				else
				{
							wait1Msec(1000);
							found_person = drive_path(length, motor_power_drive, motor_power_rotate);

							wait1Msec(1000);
							if (found_person == 1) return 1;
							rotate(0, motor_power_rotate, 85);

							wait1Msec(1000);

							found_person = drive_path(width, motor_power_drive, motor_power_rotate);

							wait1Msec(1000);
							if (found_person == 1) return 1;
							rotate(0, motor_power_rotate, 85);
				}
				++count_13;
				++count_24;
		}
drive_path(length, motor_power_drive, motor_power_rotate);
		wait1Msec(1000);
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
	const int MOTOR_POWER_DRIVE = 30;
	const int MOTOR_POWER_ROTATE = 5;
	const int BOUPHOSTREDON_LENGTH = 70;
	const int BOUPHOSTREDON_WIDTH = 17;

	//all data arrays
	int patient_id = 0;
	int quadrant = 0;
	int time_last_seen = 0;
	//int order[4] = {0, 1, 2, 3};
	int updated_time = 0;

	configure_all_sensors();

	// Opening and error handling the reading file
	TFileHandle fin;
	bool ReadCheck = openReadPC(fin, "rescue_three.txt");

	if (!ReadCheck)
	{
		displayString(5, "Error: Fail to open file");
		wait1Msec(5000);
	}

	Input(fin, patient_id, quadrant, time_last_seen);
	//calculate_order(times, order);

	// Error handling and opening the writing file
	TFileHandle fout;
	bool WriteCheck = openWritePC(fout, "report_three.txt");

	if (!WriteCheck)
	{
		displayString(5, "Error: Fail to open file");
		wait1Msec(5000);
	}
	displayString(7, "Ready to start!");


	//The program will only start if you press the enter button
	while(!getButtonPress(buttonEnter))
	{}
	while(getButtonPress(buttonEnter))
	{}





		/*int index = order[count];
		int quadrant = location[index];
		int name = people[index];
		int time_last_seen = times[index];
		time1[T1] = 0;
		int saved_person = 0;
		float time = 0;
*/
		//int index = order[count - 1];


		time1[T1] = 0;
		exit_centre(30, MOTOR_POWER_ROTATE, quadrant);
	 	int saved_person = bouphostredon(MOTOR_POWER_DRIVE, MOTOR_POWER_ROTATE,
	 			BOUPHOSTREDON_LENGTH, BOUPHOSTREDON_WIDTH, quadrant);

	 	rotate_to_begin(quadrant, MOTOR_POWER_ROTATE);
	 	return_to_begin(MOTOR_POWER_DRIVE, MOTOR_POWER_ROTATE, quadrant);
		if (saved_person)
		{
			release_person();
		}



	int time = time1[T1];
	time /= 1000;




   calculate_exposure(time, time_last_seen, updated_time);
   Output(fout, patient_id, updated_time, saved_person);





//Output(fout, patient_id, updated_time, saved_person);
	closeFilePC(fin);
	closeFilePC(fout);

}
