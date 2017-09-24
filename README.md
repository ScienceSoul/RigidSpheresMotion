# RigidSpheresMotion

### Description:

This program simulates the motion of rigid spheres in a cube with collision detection between spheres. The inputs of the program are specified in the file parameters.dat, which is located in the directory params. The inputs are:
* The number of spheres used in the simulation.
* The radius of the spheres.
* The coefficient of restitution.
* The gravity vector.
* The method used to compute the collision between the spheres. 

Two methods of collision detection are supported:
1. AABB: Detect the collision between two spheres by first using AABB (axis-aligned bounding box) in order to check if they are near each other. Then by determining the distance between the spheres' centers, and check it against the sum of the radii of the spheres, collision is detected.
2. Dynamic-Static: Detect the collision between two spheres using a more precise Dynamic-Static approach. This is done by first changing the velocity frame of reference to one sphere only.


* Language: C
* Compiler: Clang/LLVM
* Platform: masOS/Linux 
* Required library(ies): OpenGL, GLUT and BLAS/LAPACK

Compilation:

```
cd src
make depend 
make
```

Run with:

```
./RigidSpheresMotion
```
