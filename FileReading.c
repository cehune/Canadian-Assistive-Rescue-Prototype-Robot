#include "PC_FileIO.c"

task main()
{
TFileHandle fin;
bool fileOkay = openReadPC(fin,"filename.txt");
TFileHandle fout;
bool fileooo = openWritePC(fout, "fileWrite.txt");

string s[2] = {"aa", "bb"};

readTextPC(fin, s[0]);
readTextPC(fin, s[1]);
writeTextPC(fout, s[1]);
writeEndlPC(fout);
writeTextPC(fout, s[0]);
writeEndlPC(fout);


	nMotorEncoder[motorB] = 0;
	motor[motorB]= -5;
	while(nMotorEncoder[motorB] > -105)
	{}
	motor[motorB]= 0;
	wait1Msec(1000);
	motor[motorB]= 7;
	while(nMotorEncoder[motorB] < 15)
	{}

}
