/**
 * climate.c
 *
 * Performs analysis on climate data provided by the
 * National Oceanic and Atmospheric Administration (NOAA).
 *
 * Input:    Tab-delimited file(s) to analyze.
 * Output:   Summary information about the data.
 *
 * Compile:  gcc -g -Wall -o climate climate.c
 *           (or run make)
 *
 * Run:      ./climate data_tn.tdv data_wa.tdv
 *
 *
 * Opening file: data_tn.tdv
 * Opening file: data_wa.tdv
 * States found: WA TN
  * -- State: WA --
 * Number of Records: 48357
 * Average Humidity: 61.3%
 * Average Temperature: 52.9F
 * Max Temperature: 125.7F on Sun Jun 28 17:00:00 2015
 * Min Temperature: -18.7F on Wed Dec 30 04:00:00 2015
 * Lightning Strikes: 1190
 * Records with Snow Cover: 1383
 * Average Cloud Cover: 54.5%
 * -- State: TN --
 * Number of Records: 17097
 * Average Humidity: 49.4%
 * Average Temperature: 58.3F
 * Max Temperature: 110.4F on Mon Aug  3 11:00:00 2015
 * Min Temperature: -11.1F on Fri Feb 20 04:00:00 2015
 * Lightning Strikes: 781
 * Records with Snow Cover: 107
 * Average Cloud Cover: 53.0%

 *
 * TDV format:
 *
 * CA 1428300000000 9prc 93.0 0.0 100.0 0.0 95644.0  277.58716
 * CA 1430308800000 9prc 4.0  0.0 100.0 0.0 99226.0  282.63037
 * CA 1428559200000 9prr 61.0 0.0 0.0   0.0 102112.0 285.07513
 * CA 1428192000000 9prk 57.0 0.0 100.0 0.0 101765.0 285.21332
 * CA 1428170400000 9prd 73.0 0.0 22.0  0.0 102074.0 285.10425
 * CA 1429768800000 9pr6 38.0 0.0 0.0   0.0 101679.0 283.9342
 * CA 1428127200000 9prj 98.0 0.0 100.0 0.0 102343.0 285.75
 * CA 1428408000000 9pr4 93.0 0.0 100.0 0.0 100645.0 285.82413
 *
 * Each field is separated by a tab character \t and ends with a newline \n.
 *
 * Fields:
 *      state code (e.g., CA, TX, etc),
 *      timestamp (time of observation as a UNIX timestamp),
 *      geolocation (geohash string),
 *      humidity (0 - 100%),
 *      snow (1 = snow present, 0 = no snow),
 *      cloud cover (0 - 100%),
 *      lightning strikes (1 = lightning strike, 0 = no lightning),
 *      pressure (Pa),
 *      surface temperature (Kelvin)
 */

#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_STATES 50

/* TODO: Add elements to the climate_info struct as necessary. */
struct climate_info
{
    char code[3];
    unsigned long numRecords;
    long double humidity;
    long snowCount;
    double cloudCover;
    long double pressure;
    long double surfaceTemp;
    long double avgTemperature;
    long double avgHumidity;
    long double maxTemp;
    long double minTemp;
    double lightningCount;
    long double avgCloudCover;
    long timeStamp;
    long maxTimeStamp;
    long minTimeStamp;
    
};


void analyze_file(FILE *file, struct climate_info *states[], int num_states);
void print_report(struct climate_info *states[], int num_states);

int main(int argc, char *argv[])
{
    /* TODO: fix this conditional. You should be able to read multiple files. */
    if (argc < 1)
    {
        printf("Usage: %s tdv_file1 tdv_file2 ... tdv_fileN \n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Let's create an array to store our state data in. As we know, there are
     * 50 US states. */
    struct climate_info *states[NUM_STATES] = {NULL};

    int i;
    for (i = 1; i < argc; ++i)
    {
        /* TODO: Open the file for reading */
        /* TODO: If the file doesn't exist, print an error message and move on
         * to the next file. */

        FILE *fp = fopen(argv[i], "r");
        if(fp == NULL)
        {
            fprintf(stderr, "Error opening file %s\n", argv[i]);
            return EXIT_FAILURE;
        }
        printf("opening file: %s\n", argv[i]);

        /* TODO: Analyze the file */

         analyze_file(fp, states, NUM_STATES);
    }

    /* Now that we have recorded data for each file, we'll summarize them: */
    print_report(states, NUM_STATES);
    return 0;
}


void analyze_file(FILE *file, struct climate_info **states, int num_states)
{
    const int line_sz = 100;
    char line[line_sz];
    int i = 0;
    int val = 0;
    while (fgets(line, line_sz, file) != NULL){
        char* token = strtok(line, "\t");
        char* tokens[9];
        int tokenIndex = 0;
        while (token != NULL && tokenIndex <9){
            tokens[tokenIndex++] = token;
            token = strtok(NULL, "\t");
        }
        //struct
        struct climate_info *state = malloc(sizeof(struct climate_info));
        if (state == NULL){
            perror("fopen"); //perror prints the actual error
            break;
            //printf("malloc did not work");
            //break;
        }
        strcpy(state->code, tokens[0]);
        state->timeStamp = (atol(tokens[1]))/1000;
        state->humidity = atof(tokens[3]);
        state->snowCount = atof(tokens[4]);
        state->cloudCover = atof(tokens[5]);
        state->lightningCount = atof(tokens[6]);
        state->surfaceTemp = ((atof(tokens[8]))*9/5 - 459.67);

        for(i = 0; i < num_states;i++){
            if (states[i] == NULL){
                states[i] = state;
                states[i]->numRecords = 1;
                break;
            } else{
                val = strcmp(states[i]->code, state->code);
                if (val == 0){
                    double long minTemp = state->surfaceTemp;//temp variable
                    double long maxTemp = state->surfaceTemp;//temp variable
                    
                    states[i]->numRecords++;
                    states[i]->humidity += state->humidity;
                    states[i]->cloudCover += state->cloudCover;
                    states[i]->surfaceTemp += state->surfaceTemp;
                    //the average lightning count
                    states[i]->lightningCount += state->lightningCount;
                    //the average temp
                    states[i]->avgTemperature = ((states[i]->surfaceTemp)/states[i]->numRecords);
                    //the average humidity
                    states[i]->avgHumidity = ((states[i]-> humidity)/states[i]->numRecords);
                    //the average snow count
                    states[i]->snowCount += state->snowCount;
                    //min temp
                    if (minTemp<states[i]->minTemp){
                        states[i]->minTimeStamp = state->timeStamp;
                        states[i]->minTemp = state->surfaceTemp;
                    }
                    //max temp
                    if (maxTemp >states[i]->maxTemp){
                        states[i]->maxTimeStamp = state->timeStamp;
                        states[i]->maxTemp = state->surfaceTemp;
                    }
            
                    //the average cloud cover
                    states[i]->avgCloudCover = (states[i]->cloudCover) /states[i]->numRecords;
                    break;
                }
            }
        }
    }
}
//printing out all the information
void print_report(struct climate_info *states[], int num_states)
{
    printf("States found: ");
    int i;
    for (i = 0; i < num_states; ++i)
    {
        if (states[i] != NULL)
        {
            struct climate_info *info = states[i];
            printf("%s ", info->code);}}
    printf("\n");
    for (i = 0; i < num_states; ++i)
    {
        if (states[i] != NULL){
            printf("-- State: %s --\n", states[i]->code);
            printf("Number of Records: %lu\n",states[i]->numRecords);
            printf("Average Humidity: %0.1Lf%s\n", states[i]->avgHumidity, "%");
            printf("Average Temperature: %0.1Lf%s\n", states[i]->avgTemperature,"F");
            printf("Max Temperature: %0.1Lf%s", states[i]->maxTemp,"F\n");
            printf("Max Temperature on: %s", ctime(&states[i]->maxTimeStamp));
            printf("Min Temperature: %0.1Lf%s", states[i]->minTemp,"F\n");
            printf("Min Temperature on: %s", ctime(&states[i]->minTimeStamp));
            printf("Lightning Strikes: %f\n", states[i]->lightningCount);
            printf("Records with Snow Cover: %ld\n", states[i]->snowCount);
            printf("Average Cloud Cover: %0.1Lf%s\n", states[i]->avgCloudCover, "%");
            printf("%s\n", "---------------------------");
        }
    }
}







