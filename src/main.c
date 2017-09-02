//
//  main.c
//  RigidSpheresMotion
//
//  Created by Seddik hakime on 22/08/2017.
//  Copyright © 2017 ScienceSoul. All rights reserved.
//

#ifdef __APPLE__
    #include <Accelerate/Accelerate.h>
#else
    #include "cblas.h"
    #include "cblas_f77.h"
#endif

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "Utils.h"

const float BOUNDARY = 2.0f;

#define NBALLS      50
#define randf()     ((float)rand()/(float)RAND_MAX)

int rotate=0;
float tm, th=0.0f; // tm measures the time
const float gravity = 9.81f;
const float delta_tm = 0.005f;
const float restitution = 0.7f;

typedef struct sphere {
    float radius;
    float mass;
    float x;
    float y;
    float z;
    float vx;
    float vy;
    float vz;
    float colors[4];
} sphere;

sphere spheres[NBALLS];

void colorSphere(float colors[]) {
    int red = arc4random() % 256;
    int green = arc4random() % 256;
    int blue = arc4random() % 256;
    
    colors[0] = (float)red / 255.0f;
    colors[1] = (float)green / 255.0f;
    colors[2] = (float)blue / 255.0f;
    colors[3] = 1.0f;
}

float MinBound(float radius) {
    
    return -BOUNDARY + radius;
}

float MaxBound(float radius) {
    
    return BOUNDARY - radius;
}

void init() {
    srand((int)time(NULL));
    for (int i=0; i<NBALLS; i++) {
        spheres[i].radius = 0.09f;
        spheres[i].mass = spheres[i].radius; // Assume mass = radius for simplicity
        spheres[i].x = MinBound(spheres[i].radius) + randf()*(MaxBound(spheres[i].radius)-MinBound(spheres[i].radius));
        spheres[i].y = MinBound(spheres[i].radius) + randf()*(MaxBound(spheres[i].radius)-MinBound(spheres[i].radius));
        spheres[i].z = 1.5f;
        spheres[i].vx = -1.5f + randf() * 3.0f;
        spheres[i].vy = -1.5f + randf() * 3.0f;
        spheres[i].vz = -1.0f + randf() * 2.0f;
        while(1) { // Reject dark spheres
            colorSphere(spheres[i].colors);
            if (spheres[i].colors[0] >= 0.2f && spheres[i].colors[1] >= 0.2f &&
                spheres[i].colors[2] >= 0.2f) break;
        }
    }
    tm = 0.0;
    
    glEnable(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glClearColor(0.2f,0.2f,0.2f,0.0f);
    
    // Create light components.
    GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8, 1.0f };
    GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat position[] = { 0.0f, 0.0f, 1.0f, 1.0f };
    
    // Assign created components to GL_LIGHT0.
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0,1.0,1.0,50.0);
    glTranslatef(0.0f,0.0f,-6.5f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void spheresUpdate(int n) {
    
    spheres[n].vz += -gravity * delta_tm;
    // Assuming that velocity is nearly constant (both in direction and magnitude)
    // during the time intervall delta_tm
    spheres[n].x  += spheres[n].vx * delta_tm;
    spheres[n].y  += spheres[n].vy * delta_tm;
    spheres[n].z  += spheres[n].vz * delta_tm;
    
    if (fabs(spheres[n].x) >= MaxBound(spheres[n].radius)) {
        spheres[n].vx *= -restitution;
        spheres[n].x = (spheres[n].x < 0.0f) ? MinBound(spheres[n].radius) : MaxBound(spheres[n].radius);
    }
    if (fabs(spheres[n].y) >= MaxBound(spheres[n].radius)) {
        spheres[n].vy *= -restitution;
        spheres[n].y = (spheres[n].y < 0.0f) ? MinBound(spheres[n].radius) : MaxBound(spheres[n].radius);
    }
    if (fabs(spheres[n].z) >= MaxBound(spheres[n].radius)) {
        spheres[n].vz *= -restitution;
        spheres[n].z = (spheres[n].z < 0.0f) ? MinBound(spheres[n].radius) : MaxBound(spheres[n].radius);
    }
}

float distance(int x1, int x2, int y1, int y2, int z1, int z2) {
    
    return (sqrtf( ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * ((y1 - y2))) + ((z1 - z2) * ((z1 - z2)))));
}

float timeOfClosestApproach(int i, int j, bool *collision) {
    
    float t = 0.0f;
    
    float Pa[3] = {spheres[i].x, spheres[i].y, spheres[i].z}; // Position of sphere A
    float Pb[3] = {spheres[j].x, spheres[j].y, spheres[j].z}; // Position of sphere B
    
    float Va[3] = {spheres[i].vx, spheres[i].vy, spheres[i].vz}; // Velocity of sphere A
    float Vb[3] = {spheres[j].vx, spheres[j].vy, spheres[j].vz}; // Velocity of sphere A
    
    float Ra = spheres[i].radius; // radius of sphere A
    float Rb = spheres[j].radius; // radius of sphere B
    
    float Pab[3];
    for (int i=0; i<3; i++) {
        Pab[i] = Pa[i] - Pb[i];
    }
    
    float Vab[3];
    for (int i=0; i<3; i++) {
        Vab[i] = Va[i] - Vb[i];
    }
    float a = cblas_sdot(3, Vab, 1, Vab, 1);
    float b = 2.0f * cblas_sdot(3, Pab, 1, Vab, 1);
    float c = cblas_sdot(3, Pab, 1, Pab, 1) - (Ra + Rb) * (Ra + Rb);
    
    // The quadratic discriminant
    float discriminant = b * b - 4.0f * a * c;
    
    // Case 1:
    // If the discriminant is negative, then there are no real roots, there is no collision.
    // The time of closest approach is then given by the average of the imaginary roots,
    // which is:  t = -b / 2a
    if (discriminant < 0) {
        t = -b / (2.0f * a);
        *collision = false;
    } else {
        // Case 2 and 3:
        // If the discriminant is zero, then there is exactly one real root, meaning that the circles just grazed each other.  If the
        // discriminant is positive, then there are two real roots, meaning that the circles penetrate each other.  In that case, the
        // smallest of the two roots is the initial time of impact.  We handle these two cases identically.
        float t0 = (-b + (sqrtf(discriminant)) / (2.0f * a));
        float t1 = (-b - (sqrtf(discriminant)) / (2.0f * a));
        t = min(t0, t1);
        
        // We also have to check if the time to impact is negative.  If it is negative, then that means that the collision
        // occured in the past.  Since we're only concerned about future events, we say that no collision occurs if t < 0.
        if (t < 0) {
            *collision = false;
        } else {
            *collision = true;
        }
    }
    
    // Finally, if the time is negative, then set it to zero, because, again, we want this function to respond only to future events.
    if (t < 0) t = 0;
    
    return t;
}

void calculateNewVelocities(int i, int j) {
    
    float mass1 = spheres[i].mass;
    float mass2 = spheres[j].mass;
    
    float vx1 = spheres[i].vx;
    float vy1 = spheres[i].vy;
    float vz1 = spheres[i].vz;
    
    float vx2 = spheres[j].vx;
    float vy2 = spheres[j].vy;
    float vz2 = spheres[j].vz;
    
    float new_vx1 = (vx1 * (mass1 - mass2) + (2 * mass2 * vx2)) / (mass1 + mass2);
    float new_vy1 = (vy1 * (mass1 - mass2) + (2 * mass2 * vy2)) / (mass1 + mass2);
    float new_vz1 = (vz1 * (mass1 - mass2) + (2 * mass2 * vz2)) / (mass1 + mass2);
    
    float new_vx2 = (vx2 * (mass2 - mass1) + (2 * mass1 * vx1)) / (mass1 + mass2);
    float new_vy2 = (vy2 * (mass2 - mass1) + (2 * mass1 * vy1)) / (mass1 + mass2);
    float new_vz2 = (vz2 * (mass2 - mass1) + (2 * mass1 * vz1)) / (mass1 + mass2);
    
    spheres[i].vx = new_vx1;
    spheres[i].vy = new_vy1;
    spheres[i].vz = new_vz1;
    
    spheres[j].vx = new_vx2;
    spheres[j].vy = new_vy2;
    spheres[j].vz = new_vz2;
    
    spheres[i].x += spheres[i].vx * delta_tm;
    spheres[i].y += spheres[i].vy * delta_tm;
    spheres[i].z += spheres[i].vz * delta_tm;
    
    spheres[j].x += spheres[j].vx * delta_tm;
    spheres[j].y += spheres[j].vy * delta_tm;
    spheres[j].z += spheres[j].vz * delta_tm;
}

void detectCollisions(int n) {
    for (int i=0; i<NBALLS; i++) {
        if (n != i) {
            if (spheres[n].x + spheres[n].radius + spheres[i].radius > spheres[i].x &&
                spheres[n].x < spheres[i].x + spheres[n].radius + spheres[i].radius &&
                spheres[n].y + spheres[n].radius + spheres[i].radius > spheres[i].y &&
                spheres[n].y < spheres[i].y + spheres[n].radius + spheres[i].radius &&
                spheres[n].z + spheres[n].radius + spheres[i].radius > spheres[i].z &&
                spheres[n].z < spheres[i].z + spheres[n].radius + spheres[i].radius
                ) { //AABBs are overlapping
                if (distance(spheres[n].x, spheres[i].x, spheres[n].y, spheres[i].y, spheres[n].z, spheres[i].z) < spheres[n].radius + spheres[i].radius) {
                    // Spheres have collided
                    calculateNewVelocities(n, i);
                }
            }
        }
    }
}

void update() {
    tm += delta_tm;
    for (int i=0; i<NBALLS; i++) {
        spheresUpdate(i);
    }
    for (int i=0; i<NBALLS; i++) {
        detectCollisions(i);
    }
    
    if (rotate) {
        th += 0.2f;
        if (th > 360.0f) th -= 360.0f;
    }
    glutPostRedisplay();
}

void drawCube(void) {
    float colors[3];
    colors[0] = 1.0f;
    colors[1] = 1.0f;
    colors[2] = 1.0f;
    glColor3f(colors[0], colors[1], colors[2]);
    glutWireCube(4.0);
}

void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glRotatef(th,0.0f,1.0f,0.0f);
    glRotatef(-90.0f,1.0f,0.0f,0.0f);
    drawCube();
    for (int i=0; i<NBALLS; i++) {
        glPushMatrix();
        glTranslatef(spheres[i].x, spheres[i].y, spheres[i].z);
        glColor3f(spheres[i].colors[0], spheres[i].colors[1], spheres[i].colors[2]);
        glutSolidSphere(spheres[i].radius,50,50);
        glPopMatrix();
    }
    glPopMatrix();
    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y) {
    switch (button) {
        case GLUT_LEFT_BUTTON:
            rotate = !state;
            break;
        case GLUT_RIGHT_BUTTON:
            init();
            break;
    }
}

int main(int argc, const char * argv[]) {
    glutInit(&argc,(char **)argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800,800);
    glutCreateWindow("Rigid Spheres Motion");
    glutIdleFunc(update);
    glutDisplayFunc(drawScene);
    glutMouseFunc(mouse);
    init();
    glutMainLoop();
    return 0;
}
