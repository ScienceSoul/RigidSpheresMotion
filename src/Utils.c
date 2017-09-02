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
