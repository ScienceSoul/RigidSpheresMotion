//
//  main.c
//  RigidSpheresMotion
//
//  Created by Seddik hakime on 22/08/2017.
//  Copyright © 2017 ScienceSoul. All rights reserved.
//


#ifdef __APPLE__
    #include <Accelerate/Accelerate.h>
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
    #include <GLUT/glut.h>
#else
    #include <GL/gl.h>
    #include <GL/glu.h>
    #include <GL/glut.h>
#endif

#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#include "Utils.h"

const float BOUNDARY = 2.0f;

#define randf()     ((float)rand()/(float)RAND_MAX)

int NBSPHERES = 0;
int rotate=0;

float tm, th=0.0f; // tm measures the time
const float delta_tm = 0.005f;
float gravity[3] = {0.0f, 0.0f, 0.0f};
float restitution = 0.0f;
float sphereRadius = 0.0f;
float totalMomentumWalls = 0.0;

char detectionMethod[50];
void (*_Nullable detectionMethodCallBack)(int n) = NULL;

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

sphere *spheres;

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
    for (int i=0; i<NBSPHERES; i++) {
        spheres[i].radius = sphereRadius;
        spheres[i].mass = sphereRadius;  // Assume mass = radius for simplicity
        spheres[i].x = MinBound(spheres[i].radius) + randf()*(MaxBound(spheres[i].radius)-MinBound(spheres[i].radius));
        spheres[i].y = MinBound(spheres[i].radius) + randf()*(MaxBound(spheres[i].radius)-MinBound(spheres[i].radius));
        spheres[i].z = 1.5f;
        
        if (gravity[0] != 0) {
            spheres[i].vx = -1.0f + randf() * 2.0f;
        } else spheres[i].vx = -1.5f + randf() * 3.0f;
        if (gravity[1] != 0) {
            spheres[i].vy = -1.0f + randf() * 2.0f;
        } else spheres[i].vy = -1.5f + randf() * 3.0f;
        if (gravity[2] != 0) {
            spheres[i].vz = -1.0f + randf() * 2.0f;
        } else spheres[i].vz = -1.5f + randf() * 3.0f;
        
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

float distance(int x1, int x2, int y1, int y2, int z1, int z2) {
    
    return (sqrtf( ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * ((y1 - y2))) + ((z1 - z2) * ((z1 - z2)))));
}

float magnitude(float vx, float vy, float vz) {
    
    return sqrtf((vx*vx) + (vy*vy) + (vz*vz));
}

void spheresUpdate(int n) {
    
    // The gravity
    spheres[n].vx += -gravity[0] * delta_tm;
    spheres[n].vy += -gravity[1] * delta_tm;
    spheres[n].vz += -gravity[2] * delta_tm;
    
    // Assuming that velocity is nearly constant (both in direction and magnitude)
    // during the time intervall delta_tm
    spheres[n].x  += spheres[n].vx * delta_tm;
    spheres[n].y  += spheres[n].vy * delta_tm;
    spheres[n].z  += spheres[n].vz * delta_tm;
    
    bool hit1 = false;
    bool hit2 = false;
    bool hit3 = false;
    
    float v1 = magnitude(spheres[n].vx, spheres[n].vy, spheres[n].vz);
    
    if (fabs(spheres[n].x) >= MaxBound(spheres[n].radius)) {
        hit1 = true;
        spheres[n].vx *= -restitution;
        spheres[n].x = (spheres[n].x < 0.0f) ? MinBound(spheres[n].radius) : MaxBound(spheres[n].radius);
    }
    if (fabs(spheres[n].y) >= MaxBound(spheres[n].radius)) {
        hit2 = true;
        spheres[n].vy *= -restitution;
        spheres[n].y = (spheres[n].y < 0.0f) ? MinBound(spheres[n].radius) : MaxBound(spheres[n].radius);
    }
    if (fabs(spheres[n].z) >= MaxBound(spheres[n].radius)) {
        hit3 = true;
        spheres[n].vz *= -restitution;
        spheres[n].z = (spheres[n].z < 0.0f) ? MinBound(spheres[n].radius) : MaxBound(spheres[n].radius);
    }
    
    if (hit1 || hit2 || hit3) {
        float mv1 = v1 * spheres[n].mass;
        float v2 = magnitude(spheres[n].vx, spheres[n].vy, spheres[n].vz);
        float mv2 = v2 * spheres[n].mass;
        
        if (gravity[0] != 0.0f) {
            if (hit2 || hit3) {
                totalMomentumWalls += fabsf(mv1 - mv2);
            }
            if (hit1) {
                float xx = spheres[n].x + spheres[n].vx * delta_tm;
                if ((roundf((xx+spheres[n].radius) * 100) / 100) > (MinBound(spheres[n].radius)+spheres[n].radius)) {
                    totalMomentumWalls += fabsf(mv1 - mv2);
                }
            }
        } else if (gravity[1] != 0.0f) {
            if (hit1 || hit3) {
                totalMomentumWalls += fabsf(mv1 - mv2);
            }
            if (hit2) {
                float yy = spheres[n].y + spheres[n].vy * delta_tm;
                if ((roundf((yy+spheres[n].radius) * 100) / 100) > (MinBound(spheres[n].radius)+spheres[n].radius)) {
                    totalMomentumWalls += fabsf(mv1 - mv2);
                }
            }
        } else {
            if (hit1 || hit2) {
                totalMomentumWalls += fabsf(mv1 - mv2);
            }
            if (hit3) {
                float zz = spheres[n].z + spheres[n].vz * delta_tm;
                if ((roundf((zz+spheres[n].radius) * 100) / 100) > (MinBound(spheres[n].radius)+spheres[n].radius)) {
                    totalMomentumWalls += fabsf(mv1 - mv2);
                }
            }
        }
    }
}


void calculateNewVelocities1(int i, int j) {
    
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

void calculateNewVelocities2(int i, int j) {
    
    float Pa[3];
    Pa[0] = spheres[i].x;
    Pa[1] = spheres[i].y;
    Pa[2] = spheres[i].z;
    
    float Va[3];
    Va[0] = spheres[i].vx;
    Va[1] = spheres[i].vy;
    Va[2] = spheres[i].vz;
    
    float Pb[3];
    Pb[0] = spheres[j].x;
    Pb[1] = spheres[j].y;
    Pb[2] = spheres[j].z;
    
    float Vb[3];
    Vb[0] = spheres[j].vx;
    Vb[1] = spheres[j].vy;
    Vb[2] = spheres[j].vz;
    
    // First, find the normalized vector n from the center of
    // circle1 to the center of circle2
    float N[3];
    for (int i=0; i<3; i++) {
        N[i] = Pa[i] - Pb[i];
    }
    for (int i=0; i<3; i++) {
        N[i] = N[i] /  magnitude(N[0], N[1], N[2]);
    }
    
    // Find the length of the component of each of the movement
    // vectors along n.
    // a1 = v1 . n
    // a2 = v2 . n
#ifdef __APPLE__
    float a1 = cblas_sdot(3, Va, 1, N, 1);
    float a2 = cblas_sdot(3, Vb, 1, N, 1);
#else
    float a1 = 0.0f;
    float a2 = 0.0f;
    for (int i=0; i<3; i++) {
        a1 = a1 + Va[i]*N[i];
        a2 = a2 + Vb[i]*N[i];
    }
#endif
    
    // Using the optimized version,
    // optimizedP =  2(a1 - a2)
    //              -----------
    //                m1 + m2
    float optimizedP = (2.0f * (a1-a2)) / (spheres[i].mass + spheres[j].mass);
    
    // Calculate va', the new movement vector of sphere A
    // va' = va - optimizedP * m2 * n
    float Va_prime[3];
    for (int i=0; i<3; i++) {
        Va_prime[i] = Va[i] - optimizedP * spheres[j].mass * N[i];
    }
    // Calculate vb', the new movement vector of sphere B
    // vb' = vb + optimizedP * m1 * n
    float Vb_prime[3];
    for (int i=0; i<3; i++) {
        Vb_prime[i] = Vb[i] + optimizedP * spheres[i].mass * N[i];
    }
    
    spheres[i].vx = Va_prime[0]; // + Vb_prime[0];
    spheres[i].vy = Va_prime[1]; // + Vb_prime[1];
    spheres[i].vz = Va_prime[2]; // + Vb_prime[2];
    
    spheres[j].vx = Vb_prime[0];
    spheres[j].vy = Vb_prime[1];
    spheres[j].vz = Vb_prime[2];
}

// Detect collision between two spheres by first using AABB (axis-aligned bounding box) to check
// if they are near each other. Then by determinig the distance between the spheres' centers
// and check it against the sum of the radii of the spheres, collision is detected
void detectCollisionAABB(int n) {
    for (int i=0; i<NBSPHERES; i++) {
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
                    calculateNewVelocities2(n, i);
                }
            }
        }
    }
}

// Detect collision between two spheres using a more precise Dynamic-Static approach. This is done by first changing the velocity
// frame of reference to one sphere only.
bool detectCollisionDynamicStatic(int i, int j) {
    
    float Pa[3];
    Pa[0] = spheres[i].x;
    Pa[1] = spheres[i].y;
    Pa[2] = spheres[i].z;
    
    float Va[3];
    Va[0] = spheres[i].vx;
    Va[1] = spheres[i].vy;
    Va[2] = spheres[i].vz;
    
    float Pb[3];
    Pb[0] = spheres[j].x;
    Pb[1] = spheres[j].y;
    Pb[2] = spheres[j].z;
    
    float Vb[3];
    Vb[0] = spheres[j].vx;
    Vb[1] = spheres[j].vy;
    Vb[2] = spheres[j].vz;
    
    // Change frame of reference to one sphere for the velocity
    float VV[3];
    for (int i=0; i<3; i++) {
        VV[i] = Va[i] - Vb[i];
    }
    
    float dist = distance(Pa[0], Pb[0], Pa[1], Pb[1], Pa[2], Pb[2]);
    float sumRadii = spheres[i].radius + spheres[j].radius;
    dist -= sumRadii;
    if (magnitude(VV[0], VV[1], VV[2]) < dist) {
        return false;
    }
    
    // Normalize the velocity vector
    float N[3];
    for (int i=0; i<3; i++) {
        N[i] = VV[i] / magnitude(VV[0], VV[1], VV[2]);
    }
    
    // Find C, the vector from the center of the sphere A
    // to center of B
    float C[3];
    for (int i=0; i<3; i++) {
        C[i] = Pb[i] - Pa[i];
    }
    
    // D = N \dot C
#ifdef __APPLE__
    float D = cblas_sdot(3, N, 1, C, 1);
#else
    float D = 0.0f;
    for (int i=0; i<3; i++) {
        D = D + N[i]*C[i];
    }
#endif
    
    // Another early escape: make sure that A is moving towards B.
    // If the dot product between the velocity vector and B.center - A.center
    // is less than ot equal to O, A is not moving towards B
    if (D <= 0) {
        return false;
    }
    
    // Find the length of vector C
    float lengthC = magnitude(C[0], C[1], C[2]);
    
    double F = (lengthC * lengthC) - (D * D);
    
    // Escape test: if the closest that A will get to B is more than
    // the sum of their radii, there's no way they are going to collide
    float sumRadiiSquared = sumRadii * sumRadii;
    if (F >= sumRadiiSquared) {
        return false;
    }
    
    // We now have F and sumRadii, two sides of a right triangle.
    // Use these to find the third side, sqrt(T)
    double T = sumRadiiSquared - F;
    
    // If there is no such right triangle with sides length of
    // sumRadii and sqrt(f), T will probably be less than 0.
    // Better to check now than perform a square root of a
    // negative number.
    if (T < 0) {
        return false;
    }
    
    // Therefore the distance the circle has to travel along
    // velocity vector is D - sqrt(T)
    float distanceDT = D - sqrt(T);
    
    // Get the magnitude of the movement vector
    float mag = magnitude(VV[0], VV[1], VV[2]);
    
    // Finally, make sure that the distance A has to move
    // to touch B is not greater than the magnitude of the
    // velocity vector.
    if (mag < distanceDT) {
        return false;
    }
    
    return true;
}

void collisionAABB(int n) {
    detectCollisionAABB(n);
}

void collisionDynamicStatic(int n) {
    
    bool collision;
    for (int i=0; i<NBSPHERES; i++) {
        if (n != i) {
            collision = detectCollisionDynamicStatic(n, i);
            if (collision) {
                // Spheres have collided
                calculateNewVelocities2(n, i);
            }
        }
    }
}

void update() {
    
    tm += delta_tm;
    for (int i=0; i<NBSPHERES; i++) {
        spheresUpdate(i);
    }
    
    for (int i=0; i<NBSPHERES; i++) {
        detectionMethodCallBack(i);
    }
    
    if (rotate) {
        th += 0.2f;
        if (th > 360.0f) th -= 360.0f;
    }
    glutPostRedisplay();
}

void printCharactersScene(float x, float y, char *string) {
    
    glColor3f(1.0, 1.0, 1.0);
    glDisable(GL_LIGHTING);
    glRasterPos2f(x, y);
    int len = (int)strlen(string);
    
    // Display character by character
    for (int i=0; i<len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
    }
    glEnable(GL_LIGHTING);
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
    for (int i=0; i<NBSPHERES; i++) {
        glPushMatrix();
        glTranslatef(spheres[i].x, spheres[i].y, spheres[i].z);
        glColor3f(spheres[i].colors[0], spheres[i].colors[1], spheres[i].colors[2]);
        glutSolidSphere(spheres[i].radius,50,50);
        glPopMatrix();
    }
    glPopMatrix();
    char output[50];
    snprintf(output, 50, "Total momentum given to walls: %f", totalMomentumWalls);
    printCharactersScene(-2.9f, 3.3f, output);
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
    
    size_t vectorSize;
    float buff;
    // Load the program parameters
    if (loadParameters(gravity, &vectorSize, &buff, &sphereRadius, &restitution, detectionMethod) != 0) {
        fatal(PROGRAM_NAME, "an error occured during reading of input parameters file.");
    }
    if (vectorSize != 3) {
        fatal(PROGRAM_NAME, "incorrect dimension of input gravity vector. Should be three.");
    }
    
    NBSPHERES = (int)buff;
    fprintf(stdout, "Program parameters: \n");
    fprintf(stdout, "Number of spheres: %d.\n", NBSPHERES);
    fprintf(stdout, "Spheres radius: %f.\n", sphereRadius);
    fprintf(stdout, "Coefficient of restitution: %f.\n", restitution);
    fprintf(stdout, "Gravity vector: %f %f %f.\n", gravity[0], gravity[1], gravity[2]);
    
    fprintf(stdout, "Spheres collision detection method: %s.\n", detectionMethod);
    // Set-up here the collision detection function we will use during the simulation
    if (strcmp(detectionMethod, "AABB") == 0) {
        detectionMethodCallBack = collisionAABB;
    } else if (strcmp(detectionMethod, "Dynamic-Static") == 0) {
        detectionMethodCallBack = collisionDynamicStatic;
    } else {
        fatal(PROGRAM_NAME, "collision detection method not recognized. Should be < AABB > or < Dynamic-Static >.");
    }
    
    spheres = (sphere *)malloc(NBSPHERES * sizeof(sphere));
    
    glutInit(&argc,(char **)argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800,800);
    glutCreateWindow("Rigid Spheres Motion");
    glutIdleFunc(update);
    glutDisplayFunc(drawScene);
    glutMouseFunc(mouse);
    init();
    glutMainLoop();
    
    free(spheres);
    return 0;
}
