/*******************************************************************************************
*
YET ANOTHER NEOSENSE PROGRAM!
*
********************************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#include "raylib.h"
#include <algorithm>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <deque>
using namespace std;

//Necessary evils :`(
int waveformIterator = 0;
int numRRintervalsForAverage = 8;
double peakThreshold = 0.75;
int globalTimestamp = 0;

//waveform globals
double waveformRawData[600] = { 0 };
double waveformMovingAverage[600] = { 0 };
double waveformFiducials[600] = { -1 };
double waveformPeak[600] = { 0 };
double waveformPeakAverage[600] = { 0 };

//Pan tompkins globals
vector <double> dataStorage[5];
int movingAverageWindowSize = 50;
int peakWindowCounter = 0;
int peakAverageWindowSize = 1000; //1 second at 1000 Hz
deque <double> peakAverageValues, ECGbuffer, movingAverageBuffer, RRintervals[2];
double result = 0;
double currentPeak = 0;
double peakAverage;
double BPM;
bool buffersInitialised = false;
bool rPeakDetected = false;


void exportScreenshot() {
	static int fileIterator = 0;
	fileIterator++;
	string builder;
	builder += "screenshots/screenshot";
	builder += to_string(fileIterator);
	builder += ".png";
	const char* fileName = builder.c_str();
	TakeScreenshot(fileName);
}

void writeCSVFile()
{
	FILE* fp;   // file pointer
	fp = fopen("savedData.csv", "w");   //create/wipe file
	cout << "Writing data file: savedData.csv... " << endl;
	fprintf(fp, "ECG data, Difference, Square, Moving average, Peak average\n");//column headers
	for (size_t k = 0; k < dataStorage[0].size(); k++)
	{
		//file headers (Raw)ECGDATA,DIFFERENCE,SQUARE,MOVINGAVERAGE,peakAverage
		fprintf(fp, "%f,", dataStorage[0][k]);
		fprintf(fp, "%f,", dataStorage[1][k]);
		fprintf(fp, "%f,", dataStorage[2][k]);
		fprintf(fp, "%f,", dataStorage[3][k]);
		fprintf(fp, "%f", dataStorage[4][k]);
		fprintf(fp, "\n");
	}
	fclose(fp);
	cout << "File written: savedData.csv... " << endl;
}

double panTompkins(int timestamp, double passedData)
{
	int i = timestamp;
	ECGbuffer.push_back(passedData);

	//dataStorage
	dataStorage[0].push_back(passedData);

	//need at least 5 readings for difference equation
	if (ECGbuffer.size() > 5) {
		int T = ECGbuffer.size() - 1;
		//difference equation: y(nT) = (2x(nT) + x(nT-T)-x(nT-3T)-2x(nT-4T)) / 8 <-- 8 is just to scale output
		result = (2 * ECGbuffer[T] + ECGbuffer[T - 1] - ECGbuffer[T - 3] - 2 * ECGbuffer[T - 4]) / 8;

		dataStorage[1].push_back(result);

		//square results: y(nT)=[x(nT)]^2
		result = result * result;

		dataStorage[2].push_back(result);
		movingAverageBuffer.push_back(result);
	}

	//need <windowSize> number of readings for
	//moving window integration: y(nT) = 1/N [x(nT-(N-1)T) + [x(nT-(N-2)T) +...+ x(nT)]
	if (movingAverageBuffer.size() > movingAverageWindowSize)
	{
		movingAverageBuffer.pop_front();
		double movingAverage = 0;
		for (int i = 0; i < movingAverageWindowSize; i++) //there is a 1 off error somewhere here...
		{
			movingAverage += movingAverageBuffer[i];
		}
		result = movingAverage /= movingAverageWindowSize;
		dataStorage[3].push_back(result);
	}

	//Check for peak
	if (result > currentPeak ) currentPeak = result;
	peakWindowCounter++;

	if ( globalTimestamp > 3000)
	{
		//check if result exceeds threshold and is less than 0.5 seconds (360Hz/2) since last peak
		if (result > peakAverage* peakThreshold&& i - RRintervals[0].back() > 200 )
		{
			//peak detected!
			rPeakDetected = true;			
				RRintervals[0].push_back(i);
				RRintervals[1].push_back(result);
			
			//keep RRintervals at only last numRRintervalsForAverage values
			while (RRintervals[0].size() > numRRintervalsForAverage) {
				RRintervals[0].pop_front();
				RRintervals[1].pop_front();
			}

			//new entry, calculate average RR interval
			BPM = 0;
			for (size_t i = RRintervals[0].size() - 1; i > 0; i--)
			{
				BPM += RRintervals[0][i] - RRintervals[0][i - 1];
			}
			BPM /= RRintervals[0].size() - 1;
			//cout << "RRaverage = " << BPM << endl;

			BPM = 60000 / BPM;
			//cout << "BPM = " << BPM << endl;
		}
	}

	//every second get the peak and get the average of the last 5 seconds worth of peaks
	if (peakWindowCounter == peakAverageWindowSize )
	{
		
		if (!buffersInitialised)
		{
			peakAverageValues.push_back(currentPeak);
			if (globalTimestamp > 3000 ) buffersInitialised = true;
			RRintervals[1].push_back(currentPeak);
			RRintervals[0].push_back(i);
		}
		
		peakAverageValues.push_back(currentPeak);
		while (peakAverageValues.size() > 5) peakAverageValues.pop_front();
		peakAverage = 0;
		for (size_t i = 0; i < peakAverageValues.size() ; i++)
		{
			peakAverage += peakAverageValues[i];
		}
		peakAverage /= peakAverageValues.size();
		//cout << "Peak Average = " << peakAverage << endl;
		currentPeak = 0;
		peakWindowCounter = 0;

		//register 1st second having passed and mark first RR interval as a starting point
	}
	dataStorage[4].push_back(peakAverage);

	/*
	for (size_t i = 0; i < RRintervals[0].size() - 1; i++)
	{
		cout << "TS: " << RRintervals[0][i] << " result: " << RRintervals[1][i] << endl;
	}
	*/
	return result;
}

void takeConstantReadings()
{
	string filename = "mit/x104.csv";
	ifstream file(filename);
	string line;
	const char* c_string;
	double reading;
	int isItTen = 0;
	double PTresult;

	while (1) {
		for (size_t i = 0; i < 1805555; i++)
		{
			//GET READING, to be replaced with PI ADC function
			getline(file, line);
			c_string = line.c_str();
			reading = atof(c_string);
			//END GET READING

			PTresult = panTompkins(globalTimestamp, reading);

			globalTimestamp++;
			isItTen++;

			if (isItTen == 10)
			{
				waveformIterator++;
				if (waveformIterator >= 600) waveformIterator = 0;
				waveformRawData[waveformIterator] = reading;
				waveformMovingAverage[waveformIterator] = PTresult * 10;
				waveformPeak[waveformIterator] = currentPeak * 10;
				waveformPeakAverage[waveformIterator] = peakAverage * 10;
				isItTen = 0;

				if (rPeakDetected)
				{
					waveformFiducials[waveformIterator] = reading + 100; //need to scale this properly
					rPeakDetected = false;
				}
				else
				{
					waveformFiducials[waveformIterator] = -1;
				}
			}
			this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}

int main(int argc, char* argv[])
{
	// Initialization
	vector <double> waveForm1, waveForm2, waveForm3, waveFormFiducials, mitECGdata;
	Font font = GetFontDefault();

	Rectangle PTboxes[8] = {
		{625,40,50,50},
		{725,40,50,50},
		{625,130,50,50},
		{725,130,50,50},
		{625,205,50,50},
		{625,270,50,50},
		{625,335,50,50},
		{625,400,50,50}
	};
	Rectangle PTcheckTextBox[4] = {
		{680,205,110,50},
		{680,270,110,50},
		{680,335,110,50},
		{680,400,110,50}
	};
	bool PTboxesState[8] = { 0,0,0,0,1,0,1,0 };

	//raylib init
	int screenWidth = 800;
	int screenHeight = 480;
	InitWindow(screenWidth, screenHeight, "NeoSense V2");
	SetTargetFPS(60);
	thread readingThread(takeConstantReadings);

	//load graphics resources
	Texture2D texturePlay = LoadTexture("res/play.png");
	Texture2D texturePause = LoadTexture("res/pause.png");
	Texture2D textureReset = LoadTexture("res/reset.png");
	Texture2D textureReturn = LoadTexture("res/return.png");
	Texture2D textureSettings = LoadTexture("res/settings.png");
	Texture2D textureStamp = LoadTexture("res/stamp.png");
	Texture2D textureExit = LoadTexture("res/exit.png");
	Texture2D textureECG = LoadTexture("res/ecg.png");
	Texture2D textureTick = LoadTexture("res/tick.png");
	Texture2D textureCsv = LoadTexture("res/csv.png");
	Texture2D texturePlus = LoadTexture("res/plus.png");
	Texture2D textureMinus = LoadTexture("res/minus.png");

	// Main loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		//Input handling
		//Keyboard
		if (IsKeyPressed(KEY_S)) exportScreenshot();
		if (IsKeyPressed(KEY_W)) writeCSVFile();

		//mouse & touchscreen
		Vector2 mouse = GetMousePosition();
		if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, PTboxes[0]) && numRRintervalsForAverage > 2) numRRintervalsForAverage--;
		if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, PTboxes[1])) numRRintervalsForAverage++;
		if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, PTboxes[2])) peakThreshold -= 0.01;
		if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, PTboxes[3])) peakThreshold += 0.01;
		for (int i = 4; i < 8; i++) {
			if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, PTboxes[i])) PTboxesState[i] = !PTboxesState[i];
		}

		// Draw
		BeginDrawing();
		ClearBackground(BLACK);

		//draw waveform
		for (size_t i = 0; i < waveformIterator; i++)
		{
			DrawLine(i, 400 - waveformRawData[i], i + 1, 400 - waveformRawData[i + 1], WHITE);
			DrawLine(i, 400 - waveformMovingAverage[i], i + 1, 400 - waveformMovingAverage[i + 1], RED);
			DrawLine(i, 400 - waveformPeak[i], i + 1, 400 - waveformPeak[i + 1], BLUE);
			DrawLine(i, 400 - waveformPeakAverage[i], i + 1, 400 - waveformPeakAverage[i + 1], GREEN);

			if (waveformFiducials[i] != -1)
			{
				double crossX = i;
				double crossY = waveformFiducials[i];
				DrawLine(crossX - 5, 400 - crossY + 5, crossX + 5, 400 - crossY - 5, PINK);
				DrawLine(crossX - 5, 400 - crossY - 5, crossX + 5, 400 - crossY + 5, PINK);
			}
		}
		int thresholdIndicator = 400 - peakAverage * 10 * peakThreshold;
		//draw threshold
		DrawLine(0, thresholdIndicator, 600, thresholdIndicator, YELLOW);

		//draw other GUI elements
		DrawLine(600, 0, 600, 480, DARKGRAY);
		DrawLine(0, 400, 600, 400, DARKGRAY);
		for (unsigned int i = 0; i < 8; i++) {
			DrawRectangleLinesEx(PTboxes[i], 1, DARKGRAY);
			if (PTboxesState[i] == 1 && i > 3) {
				DrawTexture(textureTick, PTboxes[i].x, PTboxes[i].y, WHITE);
			}
		}
		for (int i = 0; i < 4; i++)
		{
			if (i % 2 != 0) DrawTexture(texturePlus, PTboxes[i].x, PTboxes[i].y, WHITE);
			else DrawTexture(textureMinus, PTboxes[i].x, PTboxes[i].y, WHITE);
			DrawText("CheckText", PTcheckTextBox[i].x, PTcheckTextBox[i].y + 20, 10, WHITE);
		}
		DrawText("RR intervals for BPM:", 625, 20, 10, WHITE);
		DrawText("Threshold Percentage:", 625, 110, 10, WHITE);
		DrawText(FormatText("%i", numRRintervalsForAverage - 1), 690, 55, 20, WHITE);
		DrawText(FormatText("%.0f", peakThreshold * 100), 690, 145, 20, WHITE);
		DrawText(FormatText("BPM: %.1f", BPM), 50, 410, 20, WHITE);
		DrawText(FormatText("Raw Data: %.0f", ECGbuffer.back()), 50, 430, 20, WHITE);
		DrawText(FormatText("Moving Average: %.2f", waveformMovingAverage[waveformIterator]), 200, 410, 20, WHITE);
		DrawText(FormatText("peakAverage: %.2f", peakAverage), 200, 430, 20, WHITE);
		EndDrawing();
	}

	// De-Initialization
	CloseWindow();        // Close window and OpenGL context
	readingThread.detach(); //detach 2nd thread
	return 0;
}