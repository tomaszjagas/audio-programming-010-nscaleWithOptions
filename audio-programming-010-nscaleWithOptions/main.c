/******************************************************************************
Description: generate equal-tempered tables for N-notes to the octave (N <= 24)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
* usage nscale [-m] [-i] N startVal [outfile.txt]
* -m: sets format of startVal as MIDI note
* -i: prints the calculated interval as well as the absolute frequency
* outfile: optional text filename for output data
*/

int main(int argc, char* argv[]) {
	int notes, i;
	int isMidi = 0;
	int writeInterval = 0;
	int err = 0;
	double startVal, baseFreq, ratio;
	FILE* fp;
	double intervals[25];

	// check first argument for flag option: argc at least 2
	while (argc > 1) {
		if (argv[1][0] == '-') {
			if (argv[1][1] == 'm') {
				isMidi = 1;
			}
			else if (argv[1][1] == 'i') {
				writeInterval = 1;
			}
			else {
				printf("error: unrecognized option %s\n", argv[1]);
				return 1;
			}
			// step up to next argument
			argc--;
			argv++;
		}
		else {
			break;
		}
	}

	if (argc < 3) {
		printf("insufficient arguments\n");
		printf("usage: nscale [-m] [-i] N startVal [outfile.txt]\n");
		return 1;
	}

	// now read and check all arguments
	// we not expect argv[1] to hold N and argv[2] startVal
	notes = atoi(argv[1]);
	if (notes < 1 || notes>24) {
		printf("error: N out of range. Must be between 1 and 24.\n");
		return 1;
	}
	startVal = atof(argv[2]);
	if (isMidi) {
		if (startVal > 127.0) {
			printf("error: MIDI startVal must be <= 127.\n");
			return 1;
		}
		if (startVal < 0.0) {
			printf("error: MIDI startVal must be >= 0.\n");
		}
	}
	else { // it's frequency: must be positive number
		// check low limit
		if (startVal <= 0.0) {
			printf("error: frequency startVal must be positive.\n");
			return 1;
		}
	}

	// check for optional filename
	fp = NULL;
	if (argc == 4) {
		fp = fopen(argv[3], "w");
		if (fp == NULL) {
			printf("WARNING: unagle to create file %s\n", argv[3]);
			perror("");
		}
	}

	// all parameters ready - fill array and write to file if created
	// find base frequency if val is MIDI
	if (isMidi) {
		double c0, c5;
		// find base MIDI note
		ratio = pow(2.0, 1.0 / 12.0);
		c5 = 220.0 * pow(ratio, 3);
		c0 = c5 * pow(0.5, 5);
		baseFreq = c0 * pow(ratio, startVal);
	}
	else {
		baseFreq = startVal;
	}

	// calculate ratio from notes, and fill the array
	ratio = pow(2.0, 1.0 / notes);
	for (i = 0; i <= notes; i++) {
		intervals[i] = baseFreq;
		baseFreq *= ratio;
	}

	// finally read array, write to screen and optionally to file
	for (i = 0; i <= notes; i++) {
		if (writeInterval) {
			printf("%d:\t%.3f\t%.3f Hz\n", i, pow(ratio, i), intervals[i]);
		}
		else {
			printf("%d:\t%.3f Hz\n", i, intervals[i]);
		}
		if (fp) {
			if (writeInterval) {
				err = fprintf(fp, "%d:\t%f\t%f Hz\n", i, pow(ratio, i), intervals[i]);
			}
			else {
				err = fprintf(fp, "%d:\t%f Hz\n", i, intervals[i]);
			}
			if (err < 0) {
				break;
			}
		}
	}

	if (err < 0) {
		perror("There was an error writing the file.\n");
	}
	if (fp) {
		fclose(fp);
	}
	return 0;
}