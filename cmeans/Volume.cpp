#include <stdlib.h>
#include <float.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <time.h>
#include <cuda_runtime.h>
#include <cutil.h>

#include "cmeans.h"

using namespace std;

void ReportSummary(float* clusters, int count, char* inFileName){
    char logFileName [512];
    sprintf(logFileName, "%s.summary", inFileName);
    cout << "Log file name = " << logFileName << endl;

    //ofstream myfile;
    //myfile.open(logFileName);
    FILE* myfile;
    myfile = fopen(logFileName,"w");
    
    for(int i = 0; i < count; i ++){
        fprintf(myfile,"Cluster %d: ",i);
        //myfile << "Cluster " << i << ": ";
        for(int j = 0; j < NUM_DIMENSIONS; j++){
            //myfile << clusters[i*NUM_DIMENSIONS + j] << "\t";
            fprintf(myfile,"%f ",clusters[i*NUM_DIMENSIONS + j]);
        }
        //myfile << endl;
        fprintf(myfile,"\n");
    }
    //myfile.close();
    fclose(myfile);
}

void ReportBinaryResults(float* events, float* memberships, int count, char* inFileName){
    FILE* myfile;
    char logFileName [512];
    sprintf(logFileName, "%s.results", inFileName);
    cout << "Results Log file name = " << logFileName << endl;
    myfile = fopen(logFileName,"wb");

    for(int i = 0; i < NUM_EVENTS; i++){
        for(int j = 0; j < NUM_DIMENSIONS; j++){
            fwrite(&events[i*NUM_DIMENSIONS + j],4,1,myfile);
        }
        for(int j = 0; j < count; j++){
            fwrite(&memberships[j*NUM_EVENTS+i],4,1,myfile); 
        }
    }
    //fwrite(events,4,NUM_EVENTS*NUM_DIMENSIONS,myfile);
    //fwrite(memberships,4,NUM_EVENTS*NUM_CLUSTERS,myfile);
    fclose(myfile);
}

void ReportResults(float* events, float* memberships, int count, char* inFileName){
    ofstream myfile;
    char logFileName [512];
    sprintf(logFileName, "%s.results", inFileName);
    cout << "Results Log file name = " << logFileName << endl;
    myfile.open(logFileName);

    for(int i = 0; i < NUM_EVENTS; i++){
        for(int j = 0; j < NUM_DIMENSIONS-1; j++){
            myfile << events[i*NUM_DIMENSIONS + j] << ",";
        }
        myfile << events[i*NUM_DIMENSIONS + NUM_DIMENSIONS - 1];
        myfile << "\t";
        for(int j = 0; j < count-1; j++){
            myfile << memberships[j*NUM_EVENTS+i] << ","; 
        }
        myfile << memberships[(count-1)*NUM_EVENTS+i]; 
        myfile << endl;
    }
    myfile.close();
}

float MembershipValueReduced(const float* clusters, const float* events, int clusterIndex, int eventIndex, int validClusters){
    float myClustDist = CalculateDistanceCPU(clusters, events, clusterIndex, eventIndex);
    float sum =0;
    float otherClustDist;
    for(int j = 0; j< validClusters; j++){
        otherClustDist = CalculateDistanceCPU(clusters, events, j, eventIndex); 
        if(otherClustDist < .000001)
            return 0.0;
        sum += pow((float)(myClustDist/otherClustDist),float(2/(FUZZINESS-1)));
    }
    return 1/sum;
}
