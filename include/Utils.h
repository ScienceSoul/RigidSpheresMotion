//
//  Utils.h
//  RigidSpheresMotion
//
//  Created by Seddik hakime on 02/09/2017.
//  Copyright © 2017 ScienceSoul. All rights reserved.
//

#ifndef Utils_h
#define Utils_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#endif /* Utils_h */

int __attribute__((overloadable)) max(int x, int y);
int __attribute__((overloadable)) max(int x, int y, int z);
float __attribute__((overloadable)) max(float x, float y);
float __attribute__((overloadable)) max(float x, float y, float z);

int __attribute__((overloadable)) min(int x, int y);
int __attribute__((overloadable)) min(int x, int y, int z);
float __attribute__((overloadable)) min(float x, float y);
float __attribute__((overloadable)) min(float x, float y, float z);

void __attribute__((overloadable)) fatal(char head[_Nonnull]);
void __attribute__((overloadable)) fatal(char head[_Nonnull], char message[_Nonnull]);
void __attribute__((overloadable)) fatal(char head[_Nonnull], char message[_Nonnull], int n);
void __attribute__((overloadable)) fatal(char head[_Nonnull], char message[_Nonnull], double n);
void __attribute__((overloadable)) warning(char head[_Nonnull], char message[_Nonnull]);
void __attribute__((overloadable)) warning(char head[_Nonnull], char message[_Nonnull], int n);
void __attribute__((overloadable)) warning(char head[_Nonnull], char message[_Nonnull], double n);


int loadParameters(float * _Nonnull gravityVector, size_t * _Nonnull vectorSize, float * _Nonnull nbBalls, float * _Nonnull radius, float * _Nonnull restitution, char detectionMethod[_Nonnull]);

void __attribute__((overloadable)) parseArgument(const char * _Nonnull argument, const char * _Nonnull argumentName, int * _Nonnull result, size_t * _Nonnull numberOfItems);
void __attribute__((overloadable)) parseArgument(const char * _Nonnull argument, const char * _Nonnull argumentName, float * _Nonnull result, size_t * _Nonnull numberOfItems);
