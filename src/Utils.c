//
//  Utils.c
//  RigidSpheresMotion
//
//  Created by Seddik hakime on 02/09/2017.
//  Copyright © 2017 ScienceSoul. All rights reserved.
//

#include "Utils.h"

int __attribute__((overloadable)) max(int x, int y) {
    
    return (x > y) ? x : y;
}

int __attribute__((overloadable)) max(int x, int y, int z) {
    
    int m = (x > y) ? x : y;
    return (z > m) ? z : m;
}

float __attribute__((overloadable)) max(float x, float y) {
    
    return (x > y) ? x : y;
}

float __attribute__((overloadable)) max(float x, float y, float z) {
    
    float m = (x > y) ? x : y;
    return (z > m) ? z : m;
}

int __attribute__((overloadable)) min(int x, int y) {
    
    return (x < y) ? x : y;
}

int __attribute__((overloadable)) min(int x, int y, int z) {
    
    int m = (x < y) ? x : y;
    return (z < m) ? z : m;
}

float __attribute__((overloadable)) min(float x, float y) {
    
    return (x < y) ? x : y;
}

float __attribute__((overloadable)) min(float x, float y, float z) {
    
    float m = (x < y) ? x : y;
    return (z < m) ? z : m;
}

static int formatType;
void format(char * _Nullable head, char * _Nullable message, int *iValue, double *dValue);

void __attribute__((overloadable)) fatal(char head[]) {
    
    formatType = 1;
    format(head, NULL, NULL, NULL);
}

void __attribute__((overloadable)) fatal(char head[], char message[]) {
    
    formatType = 2;
    format(head, message, NULL, NULL);
}

void __attribute__((overloadable)) fatal(char head[], char message[], int n) {
    
    formatType = 3;
    format(head, message, &n, NULL);
}

void __attribute__((overloadable)) fatal(char head[], char message[], double n) {
    
    formatType = 4;
    format(head, message, NULL, &n);
}

void __attribute__((overloadable)) warning(char head[], char message[])
{
    fprintf(stdout, "%s: %s\n", head, message);
}

void __attribute__((overloadable)) warning(char head[], char message[], int n)
{
    fprintf(stdout, "%s: %s %d\n", head, message, n);
}

void __attribute__((overloadable)) warning(char head[], char message[], double n)
{
    fprintf(stdout, "%s: %s %f\n", head, message, n);
}

void format(char * _Nullable head, char * _Nullable message, int *iValue, double *dValue) {
    
    fprintf(stderr, "##                    A FATAL ERROR occured                   ##\n");
    fprintf(stderr, "##        Please look at the error log for diagnostic         ##\n");
    fprintf(stderr, "\n");
    if (formatType == 1) {
        fprintf(stderr, "%s: Program will abort...\n", head);
    } else if (formatType == 2) {
        fprintf(stderr, "%s: %s\n", head, message);
    } else if (formatType == 3) {
        fprintf(stderr, "%s: %s %d\n", head, message, *iValue);
    } else if (formatType == 4) {
        fprintf(stderr, "%s: %s %f\n", head, message, *dValue);
    }
    if (formatType == 2 || formatType == 3 || formatType == 4)
        fprintf(stderr, "Program will abort...\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "################################################################\n");
    fprintf(stderr, "################################################################\n");
    exit(-1);
}


void __attribute__((overloadable)) parseArgument(const char * _Nonnull argument, const char * _Nonnull argumentName, int * _Nonnull result, size_t * _Nonnull numberOfItems) {
    
    int idx = 0;
    *numberOfItems = 0;
    
    fprintf(stdout, "%s: parsing the parameter %s : %s.\n", PROGRAM_NAME, argumentName, argument);
    
    size_t len = strlen(argument);
    if (argument[0] != '{' || argument[len-1] != '}') fatal(PROGRAM_NAME, "input argument for network definition should start with <{> and end with <}>.");
    
    while (argument[idx] != '}') {
        if (argument[idx] == '{') {
            if (argument[idx+1] == ',' || argument[idx+1] == '{') fatal(PROGRAM_NAME, "syntax error <{,> or <{{> in imput argument for network definition.");
            idx++;
            continue;
        }
        if (argument[idx] == ',') {
            if (argument[idx+1] == '}' || argument[idx+1] == ',') fatal(PROGRAM_NAME, "syntax error <,}> or <,,> in imput argument for network definition.");
            (*numberOfItems)++;
            idx++;
            continue;
        } else {
            int digit = argument[idx] - '0';
            result[*numberOfItems] = result[*numberOfItems] * 10 + digit;
            idx++;
        }
    }
    (*numberOfItems)++;
}

void __attribute__((overloadable)) parseArgument(const char * _Nonnull argument, const char * _Nonnull argumentName, float * _Nonnull result, size_t * _Nonnull numberOfItems) {
    
    int idx = 0, fp_size = 0;
    *numberOfItems = 0;
    char floatNumber[5];
    
    fprintf(stdout, "%s: parsing the parameter %s : %s.\n", PROGRAM_NAME, argumentName, argument);
    
    size_t len = strlen(argument);
    if (argument[0] != '{' || argument[len-1] != '}') fatal(PROGRAM_NAME, "input argument for network definition should start with <{> and end with <}>.");
    
    while (1) {
        if (argument[idx] == '{') {
            if (argument[idx+1] == ',' || argument[idx+1] == '{') fatal(PROGRAM_NAME, "syntax error <{,> or <{{> in imput argument for network definition.");
            idx++;
            continue;
        }
        if (argument[idx] == ',' || argument[idx] == '}') {
            if (argument[idx] == ',') {
                if (argument[idx+1] == '}' || argument[idx+1] == ',') fatal(PROGRAM_NAME, "syntax error <,}> or <,,> in imput argument for network definition.");
            }
            result[*numberOfItems] = strtof(floatNumber,NULL);
            (*numberOfItems)++;
            if (argument[idx] == '}') {
                break;
            }
            idx++;
            fp_size = 0;
            memset(floatNumber, 0, sizeof(floatNumber));
        } else {
            floatNumber[fp_size] = argument[idx];
            fp_size++;
            idx++;
        }
    }
}

int loadParameters(float * _Nonnull gravityVector, size_t * _Nonnull vectorSize, float * _Nonnull nbBalls, float * _Nonnull radius, float * _Nonnull restitution, char detectionMethod[_Nonnull]) {
    
    // Very basic parsing of input parameters file.
    // TODO: Needs to change that to something more flexible and with better input validation
    
    FILE *f1 = fopen("parameters.dat","r");
    if(!f1) {
        f1 = fopen("./params/parameters.dat","r");
        if(!f1) {
            f1 = fopen("../params/parameters.dat","r");
            if(!f1) {
                fprintf(stdout,"%s: can't find the input parameters file.\n", PROGRAM_NAME);
                return -1;
            }
        }
    }
    
    char string[256];
    int lineCount = 1;
    int empty = 0;
    while(1) {
        fscanf(f1,"%s\n", string);
        
        if (lineCount == 1 && string[0] != '{') {
            fatal(PROGRAM_NAME, "syntax error in the file for the input parameters.");
        } else if (lineCount == 1) {
            lineCount++;
            continue;
        } else if(string[0] == '\0') {
            empty++;
            if (empty > 1000) {
                fatal(PROGRAM_NAME, "syntax error in the file for the input keys. File should end with <}>");
            }
            continue;
        }
        
        if (string[0] == '!') continue; // Comment line
        if (string[0] == '}') break;    // End of file
        
        if (lineCount == 2) {
            *nbBalls = strtof(string, NULL);
        }
        if (lineCount == 3) {
            *radius = strtof(string, NULL);
        }
        if (lineCount == 4) {
            *restitution = strtof(string, NULL);
        }
        if (lineCount == 5) {
            parseArgument(string, "gravity vector", gravityVector, vectorSize);
        }
        if (lineCount == 6) {
            strcpy(detectionMethod, string);
        }
        lineCount++;
    };
    
    return 0;
}

