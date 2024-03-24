/*
AUTHOR: Mike Monokandilos
CLASS: CSE 170 Intro to Computer Graphics
DATE: 2022/10/22
*/
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <math.h>
#include <iostream>
#include "shader.h"
#include "shaderprogram.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <vector>

using namespace std;

/*=================================================================================================
DOMAIN
=================================================================================================*/

// Window dimensions
const int InitWindowWidth = 800;
const int InitWindowHeight = 800;
int WindowWidth = InitWindowWidth;
int WindowHeight = InitWindowHeight;

// Last mouse cursor position
int LastMousePosX = 0;
int LastMousePosY = 0;

// Arrays that track which keys are currently pressed
bool key_states[256];
bool key_special_states[256];
bool mouse_states[8];

// Other parameters
bool draw_wireframe = false;

/*=================================================================================================
SHADERS & TRANSFORMATIONS
=================================================================================================*/

ShaderProgram PassthroughShader;
ShaderProgram PerspectiveShader;

glm::mat4 PerspProjectionMatrix(1.0f);
glm::mat4 PerspViewMatrix(1.0f);
glm::mat4 PerspModelMatrix(1.0f);

float perspZoom = 1.0f, perspSensitivity = 0.35f;
float perspRotationX = 0.0f, perspRotationY = 0.0f;

/*=================================================================================================
OBJECTS
=================================================================================================*/

GLuint axis_VAO;
GLuint axis_VBO[2];

// torus VAO and VBO
GLuint torus_VAO;
GLuint torus_VBO[4];

//normal VAO and VBO
GLuint normalOutlines_VAO;
GLuint normalOutlines_VBO[2];




/*=================================================================================================
GLOBAL VARIABLES
=================================================================================================*/

// GLOBAL VARIABLES
#define PI 3.1415926535


// VECTORS of floats: stores all Torus Verticies, stores all Normal Verticies, and all the Normal Outlines
std::vector <float> torusVertices;
std::vector <float> normals;
std::vector <float> normalOutlines;
std::vector <float> textureCoordinates;


std::vector <float> torus_colors;

// global R, r, n, toggleNormalOutlines and the boolean isFlat, all with default values
float n = 11.0f;
float R = 0.5f;
float r = 0.25f;
float toggleNormalOutlines = 1.0f;
bool isFlat = true;
int space = 0;

//global conditions i
bool isDone = false;




string text1 = "funny1.jpeg", text2 = "funny2.jpeg", text3 = "Mouie.jpg";

unsigned int textureCount;


float axis_vertices[] = {
//x axis
-1.0f,  0.0f,  0.0f,
1.0f,  0.0f,  0.0f,
//y axis
0.0f, -1.0f,  0.0f,
0.0f,  1.0f,  0.0f,
//z axis
0.0f,  0.0f, -1.0f,
0.0f,  0.0f,  1.0f,
};

float axis_colors[] = {
//x axis
1.0f, 0.0f, 0.0f, 1.0f,
1.0f, 0.0f, 0.0f, 1.0f,
//y axis
0.0f, 1.0f, 0.0f, 1.0f,
0.0f, 1.0f, 0.0f, 1.0f,
//z axis
0.0f, 0.0f, 1.0f, 1.0f,
0.0f, 0.0f, 1.0f, 1.0f
};

/*=================================================================================================
HELPER FUNCTIONS: TO DO TORUS CALC
=================================================================================================*/




glm::vec2 p2cTextureCalc(float x, float y) {
glm::vec2 tempTextCoordinates;
float rad = sqrt(pow(x, 2) + pow(y, 2));
float theta = atan2(y, x);

tempTextCoordinates.x = rad * cos(theta) / (2 * PI);
tempTextCoordinates.y = rad * sin(theta) / (2 * PI);
//float rad = sqrt(pow);
//coords.x = rad * cos(theta) * sin(phi);
//coords.y = rad * sin(theta) * sin(phi);

return tempTextCoordinates;
std::cout << "DEBUG+CRITICAL" << tempTextCoordinates.x << "DEBUG" << tempTextCoordinates.y << std::endl;
}
/*=================================================================================================
FUNCTIONS:FLAT SHADING, SMOOTH SHADING -> CALLED FROM CREATE TORUS VERtICIES
=================================================================================================*/
void FlatShading(float A[3], float B[3], float C[3], float D[3]) {

// temp variables + center of the triangles in order to orientate the Normal vector to point out of the center of each created triangle
glm::vec3 U, V, normalTemp, normalTemp2, normalTemp3, center, center1, center2;//center refers to center of triangle


// TOP RIGHT TRIANGLE: NORMAL CALCULATION
//C->B->A
//C->B
//B[i] - C[i]
U[0] = B[0] - C[0];
U[1] = B[1] - C[1];
U[2] = B[2] - C[2];

//B->A
//A-B
V[0] = A[0] - B[0];
V[1] = A[1] - B[1];
V[2] = A[2] - B[2];

//CROSSPRODUCT OF U = B[i]-C[i] and V= A[i]-B[i]
normalTemp = glm::cross(U, V);

//find center of triangle to 
center = { (A[0] + B[0] + C[0]) / 3, (A[1] + B[1] + C[1]) / 3, (A[2] + B[2] + C[2]) / 3 };


//push normal to the three verticies of the top right triangle
//3*4 verticies sent
for (int i = 0; i < 2; i++) {
normals.push_back(center[0]);
normals.push_back(center[1]);
normals.push_back(center[2]);
normals.push_back(1.0f);
}

//convert and scale normals into unit vectors
float magnitude = sqrt(pow(normalTemp[0], 2) + pow(normalTemp[1], 2) + pow(normalTemp[2], 2));

for (int i = 0; i < 3; i++) {
normalTemp[i] /= magnitude;
normalTemp[i] *= 0.08;
}

//calculate normal outline
for (int i = 0; i < 3; i++) {
normalOutlines.push_back(center[i]);
}
normalOutlines.push_back(1.0f);

for (int i = 0; i < 3; i++) {
normalOutlines.push_back((center[i] + normalTemp[i]));
}
normalOutlines.push_back(1.0f);


// BOTTOM LEFT TRIANGLE NORMALS
//A->D->C

//Vector U = A->D
// D-A
U[0] = D[0] - A[0];
U[1] = D[1] - A[1];
U[2] = D[2] - A[2];

//Vector V = D->C
// C - D
V[0] = C[0] - D[0];
V[1] = C[1] - D[1];
V[2] = C[2] - D[2];

//CROSSPRODUCT OF U X V      U = D[i]-A[i] and V= C[i]-D[i]
normalTemp2 = glm::cross(U, V);

center1 = { (A[0] + D[0] + C[0]) / 3,(A[1] + D[1] + C[1]) / 3, (A[2] + D[2] + C[2]) / 3 };

for (int i = 0; i < 2; i++) {
normals.push_back(center1[0]);
normals.push_back(center1[1]);
normals.push_back(center1[2]);
normals.push_back(1.0f);
}


//when finding a line segment we always have to devide the magnitude in order to get the segment itself, then we scale it so it looks like a unit vector
float magnitude1 = sqrt(pow(normalTemp2[0], 2) + pow(normalTemp2[1], 2) + pow(normalTemp2[2], 2));

for (int i = 0; i < 3; i++) {
normalTemp2[i] /= magnitude1;
normalTemp2[i] *= 0.08;
}

for (int i = 0; i < 3; i++) {
normalOutlines.push_back(center1[i]);
}
normalOutlines.push_back(1.0f);
for (int i = 0; i < 3; i++) {
normalOutlines.push_back(center1[i] + normalTemp2[i]);
}
normalOutlines.push_back(1.0f);



//REDUNDANT BOTTOM LEFT TRIANGLE NORMALS TO PUSH A SYMETRIC NUMBER OF VERTICIES
//A->D->C
//Vector U = A->D
// D-A
U[0] = D[0] - A[0];
U[1] = D[1] - A[1];
U[2] = D[2] - A[2];

//Vector V = D->C
// C - D
V[0] = C[0] - D[0];
V[1] = C[1] - D[1];
V[2] = C[2] - D[2];

//CROSSPRODUCT OF U X V      U = D[i]-A[i] and V= C[i]-D[i]
normalTemp3 = glm::cross(U, V);

center2 = { (A[0] + D[0] + C[0]) / 3,(A[1] + D[1] + C[1]) / 3, (A[2] + D[2] + C[2]) / 3 };

for (int i = 0; i < 2; i++) {
normals.push_back(center2[0]);
normals.push_back(center2[1]);
normals.push_back(center2[2]);
normals.push_back(1.0f);
}
for (int i = 0; i < 3; i++) {
normalTemp3[i] /= magnitude;
normalTemp3[i] *= 0.08;
//using dif magnitude based on the values of the triangle however this piece is redundant so 
}

for (int i = 0; i < 3; i++) {
normalOutlines.push_back(center2[i]);
}
normalOutlines.push_back(1.0f);

for (int i = 0; i < 3; i++) {
normalOutlines.push_back(center2[i] + normalTemp3[i]);
}
normalOutlines.push_back(1.0f);
}//EOF FLAT SHADDING


//START OF SMOOTH SHADING
void SmoothShading(float A[3], float B[3], float C[3], float D[3], float increment, float phi) {
//A starts at(Theta, phi + increment)
//B starts at(zeroTheta, phi + increment)
//C starts at(zeroTheta, phi)
//D starts at(Theta, phi)
//ORDERING: C -> B -> A -> A -> D -> C;
//DECLARE TEMPS
glm::vec3 CenterA, CenterB, CenterC, CenterD;
glm::vec3 NormalA, NormalB, NormalC, NormalD;
glm::vec3 normalTempA, normalTempB, normalTempC, normalTempD;

//Centers used to create the Normal Verticies
//A
CenterA[0] = (R + r) * cos(phi + increment) - r;
CenterA[1] = (R + r) * sin(phi + increment) - r;
CenterA[2] = 0;

// B
CenterB[0] = (R + r) * cos(phi + increment) - r;
CenterB[1] = (R + r) * sin(phi + increment) - r;
CenterB[2] = 0;
//C
CenterC[0] = (R + r) * cos(phi) - r;
CenterC[1] = (R + r) * sin(phi) - r;
CenterC[2] = 0;
//D
CenterD[0] = (R + r) * cos(phi) - r;
CenterD[1] = (R + r) * sin(phi) - r;
CenterD[2] = 0;

/*
///CALCULATE NORMAL AT A
//B -> A && A -> D
glm::vec3 uA = { B[0] - C[0], B[1] - C[1], B[2] - C[2] };
glm::vec3 vA = { A[0] - B[0], A[1] - B[1], A[2] - B[2] };

normalTempA = glm::cross(uA, vA);

//convert and scale normals into unit vectors
float  magnitudeA = sqrt(pow(normalTempA[0], 2) + pow(normalTempA[1], 2) + pow(normalTempA[2], 2));

for (int i = 0; i < 3; i++) {
normalTempA[i] /= magnitudeA;
normalTempA[i] *= 0.08;
}

//CALCULATE NORMAL AT B
//C -> B && B -> A
//C->B && B -> A
//VA= B[i] - C[i] && VB = A[i] - C[i]
glm::vec3 uB = { B[0] - C[0], B[1] - C[1], B[2] - C[2] };
glm::vec3 vB = { A[0] - C[0], A[1] - C[1], A[2] - C[2] };

normalTempB = glm::cross(uB, vB);

//convert and scale normals into unit vectors
float magnitudeB = sqrt(pow(normalTempB[0], 2) + pow(normalTempB[1], 2) + pow(normalTempB[2], 2));

for (int i = 0; i < 3; i++) {
normalTempB[i] /= magnitudeB;
normalTempB[i] *= 0.08;
}


//CALCULATE NORMAL AT C
//D -> C && C -> B
// C-D &&& B -C
//normalTempC
glm::vec3 uC = { C[0] - D[0], C[1] - D[1], C[2] - D[2] };
glm::vec3 vC = { B[0] - C[0], B[1] - C[1], B[2] - C[2] };

normalTempC = glm::cross(uC, vC);

//convert and scale normals into unit vectors
float magnitudeC = sqrt(pow(normalTempC[0], 2) + pow(normalTempC[1], 2) + pow(normalTempC[2], 2));

for (int i = 0; i < 3; i++) {
normalTempC[i] /= magnitudeC;
normalTempC[i] *= 0.08;
}

//CALCULATE NORMAL AT D
//A-> D && D->C
//normalTempD
//A->D->C
//  uD= D-A && vD =C - D
glm::vec3 uD = { D[0] - A[0], D[1] - A[1], D[2] - A[2] };
glm::vec3 vD = { C[0] - D[0], C[1] - D[1], C[2] - D[2] };

normalTempD = glm::cross(uD, vD);

//convert and scale normals into unit vectors
float magnitudeD = sqrt(pow(normalTempD[0], 2) + pow(normalTempD[1], 2) + pow(normalTempD[2], 2));

for (int i = 0; i < 3; i++) {
normalTempD[i] /= magnitudeD;
normalTempD[i] *= 0.08;
}
*/
//Calculate the Normals that will be used to push into the Normal Verticies buffer
NormalA = { A[0] - CenterA[0],A[1] - CenterA[1], A[2] - CenterA[2] };
NormalB = { B[0] - CenterB[0],B[1] - CenterB[1], B[2] - CenterB[2] };
NormalC = { C[0] - CenterC[0],C[1] - CenterC[1], C[2] - CenterC[2] };
NormalD = { D[0] - CenterD[0],D[1] - CenterD[1], D[2] - CenterD[2] };

//for (int i = 0; i < 3; i++) {
//std::cout << "DEBUG+++++++++++NORMALA" << NormalA[i] << "DONE" << std::endl;
// }


// define all the loops to push back Normals and their outlines
// I try to push normals before their outlines because we call the normalOUtlines VAO after the torus VAO
//ORDERING FOR NORMAL Verticie push: C -> B -> A -> A -> D -> C;
//for the outlines we have to create the bottom vertice thats on the triangle and then from theri we add our unit vector in CCW facing out 

for (int i = 0; i < 3; i++) {
normals.push_back(NormalC[i]);
normalOutlines.push_back(C[i]);
}
normals.push_back(1.0f);
normalOutlines.push_back(1.0f);

for (int i = 0; i < 3; i++) {
normals.push_back(NormalB[i]);
normalOutlines.push_back(C[i] + normalTempC[i]);
}
normals.push_back(1.0f);
normalOutlines.push_back(1.0f);

for (int i = 0; i < 3; i++) {
normals.push_back(NormalA[i]);
normalOutlines.push_back(B[i]);
}
normals.push_back(1.0f);
normalOutlines.push_back(1.0f);

for (int i = 0; i < 3; i++) {
normals.push_back(NormalA[i]);
normalOutlines.push_back(B[i] + normalTempB[i]);
}
normals.push_back(1.0f);
normalOutlines.push_back(1.0f);

for (int i = 0; i < 3; i++) {
normals.push_back(NormalD[i]);
normalOutlines.push_back(A[i]);
}
normals.push_back(1.0f);
normalOutlines.push_back(1.0f);

for (int i = 0; i < 3; i++) {
normals.push_back(NormalC[i]);
normalOutlines.push_back(A[i] + normalTempA[i]);
}
normals.push_back(1.0f);
normalOutlines.push_back(1.0f);
}//EOF OF SMOOTH SHADING


/*=================================================================================================
CREATE TORUS VERTICIES
=================================================================================================*/


// function used to generate the Torus Verticies and Invoke either Flat or Smooth shading
void CreateTorusVerticies(float R, float r, float n) {

// Here we define the four Verticies of our segment
float A[3], B[3], C[3], D[3];
glm::vec2 textureA, textureB, textureC, textureD;



// defines increment used in the calculation of Theta and PHi in order to parameterize the torus
float increment = (2 * PI) / n;
float zeroTheta = 0.0f, theta = increment, phi = increment;

for (float segments = 0; segments < n; segments += 1) {
// rotation around tube
for (float rings = 0; rings < n; rings += 1) {

/*
A starts at (Theta,phi + increment )
B starts at (zeroTheta,phi + increment )
C starts at (zeroTheta,phi )
D starts at (Theta,phi )







textureCoordinates.push_back(coord2);
textureCoordinates.push_back(coord1);
textureCoordinates.push_back(coord4);


textureCoordinates.push_back(coord4);
textureCoordinates.push_back(coord3);
textureCoordinates.push_back(coord2);


ORDERING: C -> B -> A -> A -> D -> C;




TOP RHS triangle starts with -> BOTTOM right = C then TOP Right = B then TOP LEft  = A
C->B B->A  this is the order for getting the normals in CCW

BOTTOM LHS triangle start with-> TOP LEft  = A then BOTTOM Left = D then Bottom Right = C
A->D D->C  this is the order for getting the normals in CCW
*/

// top left vertex A
//X-COORDINATE:  COS(Theta) 
// STARTS FROM: theta = increment::2*PI /N
// NEXT-STEP : theta += increment::4*PI/N -> Which will give the next LHS side x VALUE(used in A, D calculations)
//       IMPORTANT: the x-coordinate of A has to be the same x-coordinate as D
// In order to create the RHS side of the box
//Y-COORDINATE:  PHI + increment = 4*PI /N
//
A[0] = (R + r * cos(theta)) * cos(phi + increment);
A[1] = (R + r * cos(theta)) * sin(phi + increment);
A[2] = r * sin(theta);

//top right vertexB
//X-COORDINATE: COS(zeroTheta)
// STARTS FROM: zeroTheta = 0
// NEXT-STEP : zeroTheta += theta -> Which will give the next RHS side x VALUE
//       the x-coordinate of B has to be the same x-coordinate as C
// In order to create the RHS side of the box
//Y-COORDINATE: PHI + increment = 4*PI /N
// we set the upper bounds of our rectangle to be PHI + increment which is in line with the Top Left vertex A[] which also starts at PHI + increment
B[0] = (R + r * cos(zeroTheta)) * cos(phi + increment);
B[1] = (R + r * cos(zeroTheta)) * sin(phi + increment);
B[2] = r * sin(zeroTheta);


//BOTTOM RIGHT VERTEX C
//X-COORDINATE: COS(zeroTheta)
// STARTS FROM: zeroTheta = 0
// NEXT-STEP : zeroTheta += theta -> Which will give the next RHS side x VALUE
//       the x-coordinate of B has to be the same x-coordinate as C
// In order to create the RHS side of the box
//Y-COORDINATE: phi = increment = 2*PI /N
// Here we set the phi to be in line with D[] so that we can create the lower bound of the rectangle
//
C[0] = (R + r * cos(zeroTheta)) * cos(phi);
C[1] = (R + r * cos(zeroTheta)) * sin(phi);
C[2] = r * sin(zeroTheta);


//BOTTOM LEFT VERTEX D
//X-COORDINATE: COS(zeroTheta)
//       the x-coordinate of B has to be the same x-coordinate as C
// In order to create the RHS side of the box
//Y-COORDINATE: phi = increment = 2*PI /N
// Here we set the phi to be in line with C[] so that we can create the lower bound of the rectangle
D[0] = (R + r * cos(theta)) * cos(phi);
D[1] = (R + r * cos(theta)) * sin(phi);
D[2] = r * sin(theta);
/*
for (int i = 0; i < 3; i++) {
std::cout << "DEBUG+++++++CRITITCAL++++++A"<<  A[i] << std::endl;
}
*/

/*
USING POLAR-> to coord in order to map the texture in (CYLINDRICAL COORDINATES)
*/
textureA = p2cTextureCalc(theta, phi + increment);
textureB = p2cTextureCalc(zeroTheta, phi + increment);
textureC = p2cTextureCalc(zeroTheta, phi);
textureD = p2cTextureCalc(theta, phi);

textureCoordinates.push_back(textureC.x);
textureCoordinates.push_back(textureC.y);
textureCoordinates.push_back(1.0f);
textureCoordinates.push_back(1.0f);
textureCoordinates.push_back(textureB.x);
textureCoordinates.push_back(textureB.y);
textureCoordinates.push_back(1.0f);
textureCoordinates.push_back(1.0f);
textureCoordinates.push_back(textureA.x);
textureCoordinates.push_back(textureA.y);
textureCoordinates.push_back(1.0f);
textureCoordinates.push_back(1.0f);

textureCoordinates.push_back(textureA.x);
textureCoordinates.push_back(textureA.y);
textureCoordinates.push_back(1.0f);
textureCoordinates.push_back(1.0f);

textureCoordinates.push_back(textureD.x);
textureCoordinates.push_back(textureD.y);
textureCoordinates.push_back(1.0f);
textureCoordinates.push_back(1.0f);
textureCoordinates.push_back(textureC.x);
textureCoordinates.push_back(textureC.y);
textureCoordinates.push_back(1.0f);
textureCoordinates.push_back(1.0f);



// top right triangle C -> B -> A for CCW :: MUST BE CCW
for (int i = 0; i < 3; i++)
torusVertices.push_back(C[i]);
torusVertices.push_back(1.0f);

for (int i = 0; i < 3; i++)
torusVertices.push_back(B[i]);
torusVertices.push_back(1.0f);

for (int i = 0; i < 3; i++)
torusVertices.push_back(A[i]);
torusVertices.push_back(1.0f);

// bottom left Triangle A-> D -> C :: MUST BE CCW
for (int i = 0; i < 3; i++)
torusVertices.push_back(A[i]);
torusVertices.push_back(1.0f);

for (int i = 0; i < 3; i++)
torusVertices.push_back(D[i]);
torusVertices.push_back(1.0f);

for (int i = 0; i < 3; i++)
torusVertices.push_back(C[i]);
torusVertices.push_back(1.0f);

//END OF TORUS VERTICIES
// if isFlat is true -> enables flat shading and normals in the middles of the triangles
// if isFlat is false -> enables smooth shading and normals on the verticies
if (isFlat == true) {
FlatShading(A, B, C, D);
}
else if (isFlat == false) {
SmoothShading(A, B, C, D, increment, phi);
}


// increment the Phi value
phi += increment;
}
// store the previous index of theta value
zeroTheta = theta;

// increment the Theta value
theta += increment;
}
}

/*=================================================================================================
HELPER FUNCTIONS
=================================================================================================*/

void window_to_scene(int wx, int wy, float& sx, float& sy) {
sx = (2.0f * (float)wx / WindowWidth) - 1.0f;
sy = 1.0f - (2.0f * (float)wy / WindowHeight);
}

/*=================================================================================================
SHADERS
=================================================================================================*/

void CreateTransformationMatrices(void) {
// PROJECTION MATRIX  <- used to project 3D point onto the screen
PerspProjectionMatrix = glm::perspective<float>(glm::radians(60.0f), (float)WindowWidth / (float)WindowHeight, 0.01f, 1000.0f);

// VIEW MATRIX <- used to transform the model's verticies from world-space into view-space
glm::vec3 eye(0.0, 0.0, 2.0);
glm::vec3 center(0.0, 0.0, 0.0);
glm::vec3 up(0.0, 1.0, 0.0);

PerspViewMatrix = glm::lookAt(eye, center, up);

// MODEL MATRIX <- contains every translation, rotation, or scaling applied to an object 
PerspModelMatrix = glm::mat4(1.0);
PerspModelMatrix = glm::rotate(PerspModelMatrix, glm::radians(perspRotationX), glm::vec3(1.0, 0.0, 0.0));
PerspModelMatrix = glm::rotate(PerspModelMatrix, glm::radians(perspRotationY), glm::vec3(0.0, 1.0, 0.0));
PerspModelMatrix = glm::scale(PerspModelMatrix, glm::vec3(perspZoom));
}

void CreateShaders(void) {
// Renders without any transformations
PassthroughShader.Create("./shaders/simple.vert", "./shaders/simple.frag");

// Renders using perspective projection
//PerspectiveShader.Create("./shaders/persp.vert", "./shaders/persp.frag");

// Renders using prespective lighting projection
// Renders using prespective lighting projection
//PerspectiveShader.Create("./shaders/persplight.vert", "./shaders/persplight.frag");

//texpersplight
PerspectiveShader.Create("./shaders/texpersplight.vert", "./shaders/texpersplight.frag");

}

/*=================================================================================================
BUFFERS
=================================================================================================*/

void CreateAxisBuffers(void) {
glGenVertexArrays(1, &axis_VAO);
glBindVertexArray(axis_VAO);

glGenBuffers(2, &axis_VBO[0]);

glBindBuffer(GL_ARRAY_BUFFER, axis_VBO[0]);
glBufferData(GL_ARRAY_BUFFER, sizeof(axis_vertices), axis_vertices, GL_STATIC_DRAW);
glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

glBindBuffer(GL_ARRAY_BUFFER, axis_VBO[1]);
glBufferData(GL_ARRAY_BUFFER, sizeof(axis_colors), axis_colors, GL_STATIC_DRAW);
glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
glEnableVertexAttribArray(1);

glBindVertexArray(0);
}


void CreateNormsOutlineBuffer(void) {

std::vector <float> normals_color;

for (int i = 0; i < normalOutlines.size(); i++) {
normals_color.push_back(1.0f);
normals_color.push_back(1.0f);
normals_color.push_back(0.0f);
normals_color.push_back(1.0f);
}

glGenVertexArrays(1, &normalOutlines_VAO);
glBindVertexArray(normalOutlines_VAO);

glGenBuffers(2, &normalOutlines_VBO[0]);
// here we pass the outlines of the normals-> where and how we want to orientate them
//e.g., in relation to middle of triangle for flat shading
glBindBuffer(GL_ARRAY_BUFFER, normalOutlines_VBO[0]);
glBufferData(GL_ARRAY_BUFFER, sizeof(normalOutlines[0]) * normalOutlines.size(), &normalOutlines[0], GL_STATIC_DRAW);
glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

glBindBuffer(GL_ARRAY_BUFFER, normalOutlines_VBO[1]);
glBufferData(GL_ARRAY_BUFFER, sizeof(normals_color[0]) * normals_color.size(), &normals_color[0], GL_STATIC_DRAW);
glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
glEnableVertexAttribArray(1);

glBindVertexArray(0);
isDone = true;

//std::cout << "DEBUG torusVertices" << torusVertices.size() << "DEBUG normals" << normals.size() << "DEBUG normalOutlines" << normalOutlines.size() << std::endl;
}

// TORUS BUFFER call verticies function and send verticies to buffer
void CreateTorusBuffer(void) {
// clear old verticies
// 
//std::cout << "DEBUG torusVertices" << torusVertices.size() << "DEBUG normals" << normals.size() << "DEBUG normalOutlines" << normalOutlines.size() << std::endl;
torusVertices.clear();
normals.clear();
normalOutlines.clear();
textureCoordinates.clear();

//std::cout << "DEBUG UNO torusVertices" << torusVertices.size() << "DEBUG normals" << normals.size() << "DEBUG normalOutlines" << normalOutlines.size() << std::endl;
// compute verticies


CreateTorusVerticies(R, r, n);

//std::cout << "CRITICAL++URGENT" << torusVertices.size() << "DEBUG normals" << normals.size() << "DEBUG normalOutlines" << normalOutlines.size() << std::endl;
//std::cout << "2222DEBUGHELPtorusVertices" << w1 << "DEBUG normals" << h1 << "DEBUG normalOutlines" << c1 << "DEBUG textureVerticies" << std::endl;

int size = static_cast<int>(torusVertices.size());





//array/sizeof(array[0])
for (int i = 0; i < torusVertices.size(); i++) {
torus_colors.push_back(0.5f);
torus_colors.push_back(1.0f);
torus_colors.push_back(0.5f);
torus_colors.push_back(0.0f);

}

//std::cout << "THISISTHEONE" << w1 << "DEBUG normals" << h1 << "DEBUG normalOutlines" << c1 << "DEBUG textureVerticies" << std::endl;
//std::cout << "TESTING THE CONVERSIONS" << w1 << "DEBUG normals" << h1<< "DEBUG normalOutlines" << c1 << "DEBUG textureVerticies" << std::endl;
/*
CODE INVOKING TEXTURE LOGIC


*/

glGenTextures(1, &textureCount);
glBindTexture(GL_TEXTURE_2D, textureCount);

int w1 = 0;
int h1 = 0;
int c1 = 0;
unsigned char* data1 = stbi_load(text1.c_str(), &w1, &h1, &c1, 0);

if (data1) {
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w1, h1, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
glGenerateMipmap(GL_TEXTURE_2D);
}
stbi_image_free(data1);

// set the texture wrapping parameters
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
// set texture filtering parameters
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


glGenVertexArrays(1, &torus_VAO);
glBindVertexArray(torus_VAO);

glGenBuffers(4, &torus_VBO[0]);

// TORUS verticies
glBindBuffer(GL_ARRAY_BUFFER, torus_VBO[0]);
glBufferData(GL_ARRAY_BUFFER, sizeof(torusVertices[0]) * torusVertices.size(), &torusVertices[0], GL_STATIC_DRAW);
glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

// colors for the TORUS
glBindBuffer(GL_ARRAY_BUFFER, torus_VBO[1]);
glBufferData(GL_ARRAY_BUFFER, sizeof(torus_colors[0]) * torus_colors.size(), &torus_colors[0], GL_STATIC_DRAW);
glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
glEnableVertexAttribArray(1);

//here we pass the normal vectors in relation to the Torus Verticies
glBindBuffer(GL_ARRAY_BUFFER, torus_VBO[2]);
glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), &normals[0], GL_STATIC_DRAW);
glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
glEnableVertexAttribArray(2);


glBindBuffer(GL_ARRAY_BUFFER, torus_VBO[3]);
glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinates[0]) * textureCoordinates.size(), &textureCoordinates[0], GL_STATIC_DRAW);
glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
glEnableVertexAttribArray(3);

std::cout << "DEBUG torusVertices" << torusVertices.size() << "DEBUG normals" << normals.size() << "DEBUG normalOutlines" << normalOutlines.size() << "DEBUG textureVerticies" << textureCoordinates.size() << std::endl;

glBindVertexArray(0);
CreateNormsOutlineBuffer();

if (isDone == true) {
w1 = 0;
h1 = 0;
c1 = 0;
}


// std::cout << "DEBUG torusVertices" << torusVertices.size() << "DEBUG normals" << normals.size() << "DEBUG normalOutlines" << normalOutlines.size() << std::endl;
//vector.size, normals.size and finally normalOutlines.size all equal 2904
}

/*=================================================================================================
CALLBACKS
=================================================================================================*/

//-----------------------------------------------------------------------------
// CALLBACK DOCUMENTATION
// https://www.opengl.org/resources/libraries/glut/spec3/node45.html
// http://freeglut.sourceforge.net/docs/api.php#WindowCallback
//-----------------------------------------------------------------------------

void idle_func() {
//uncomment below to repeatedly draw new frames
glutPostRedisplay();
}

void reshape_func(int width, int height) {
WindowWidth = width;
WindowHeight = height;

glViewport(0, 0, width, height);
glutPostRedisplay();
}

void keyboard_func(unsigned char key, int x, int y) {

key_states[key] = true;

switch (key) {
case 'p': {
printf("'P' : show / do not show normal wireFrame(toggle) \n");
draw_wireframe = !draw_wireframe;
if (draw_wireframe == true)
std::cout << "Wireframes on.\n";
else
std::cout << "Wireframes off.\n";
break;
}
case 'q': {
printf("'q' : increment the number of triangles \n");
n += 1.0f;
CreateTorusBuffer();
break;
}
case 'a': {
printf("'a' : decrement the number of triangles \n");
n -= 1.0f;
CreateTorusBuffer();
break;
}
case 'w': {
printf("'w' : increment the r radius(by a small value \n");
r += 0.1f;
CreateTorusBuffer();
break;
}
case 's': {
printf("'s' : decrement the r radius(by a small value \n");
r -= 0.1f;
CreateTorusBuffer();
break;
}
case 'e': {
printf("'e' : increment the R radius(by a small value \n");
R += 0.1f;
CreateTorusBuffer();
break;
}
case 'd': {
printf("'d' : decrement the R radius(by a small value \n");
R -= 0.1f;
CreateTorusBuffer();
break;
}
case 'z': {
printf("'z' : show / do not show normal vectors(toggle) \n");
isFlat = true;
CreateTorusBuffer();
break;
}
case 'x': {
printf("'x' : to enable Smooth Shading \n");
isFlat = false;
CreateTorusBuffer();
break;
}
case 'c': {
printf("'c' : show / do not show normal vectors(toggle) \n");
toggleNormalOutlines *= -1.0f;
CreateTorusBuffer();
break;
}
case ' ': {
printf("' ' : change your textures (toggle) \n");

if (space == 2) {
space = 0;
}
else {
space++;
}

if (space == 0) {
text2 = "funny2.jpeg";
//text1 = "funny1.jpeg";
printf("space0");
}
else if (space == 1) {
//text2 = "funny2.jpeg";
text1 = "funny1.jpeg";
printf("space 1 \n");
}
else {
text3 = "Mouie.jpg";
printf("space2 \n");
}

CreateTorusBuffer();
break;
}


case '\x1B': {
exit(EXIT_SUCCESS);
break;
}
}
}
void key_released(unsigned char key, int x, int y) {
key_states[key] = false;
}

void key_special_pressed(int key, int x, int y) {
key_special_states[key] = true;
}

void key_special_released(int key, int x, int y) {
key_special_states[key] = false;
}

void mouse_func(int button, int state, int x, int y) {
// Key 0: left button
// Key 1: middle button
// Key 2: right button
// Key 3: scroll up
// Key 4: scroll down

if (x < 0 || x > WindowWidth || y < 0 || y > WindowHeight)
return;

float px, py;
window_to_scene(x, y, px, py);

if (button == 3) {
perspZoom += 0.03f;
}
else if (button == 4) {
if (perspZoom - 0.03f > 0.0f)
perspZoom -= 0.03f;
}

mouse_states[button] = (state == GLUT_DOWN);

LastMousePosX = x;
LastMousePosY = y;
}

void passive_motion_func(int x, int y) {
if (x < 0 || x > WindowWidth || y < 0 || y > WindowHeight)
return;

float px, py;
window_to_scene(x, y, px, py);

LastMousePosX = x;
LastMousePosY = y;
}

void active_motion_func(int x, int y) {
if (x < 0 || x > WindowWidth || y < 0 || y > WindowHeight)
return;

float px, py;
window_to_scene(x, y, px, py);

if (mouse_states[0] == true) {
perspRotationY += (x - LastMousePosX) * perspSensitivity;
perspRotationX += (y - LastMousePosY) * perspSensitivity;
}

LastMousePosX = x;
LastMousePosY = y;
}

/*=================================================================================================
RENDERING
=================================================================================================*/

void display_func(void) {

glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

CreateTransformationMatrices();

PerspectiveShader.Use();
PerspectiveShader.SetUniform("projectionMatrix", glm::value_ptr(PerspProjectionMatrix), 4, GL_FALSE, 1);
PerspectiveShader.SetUniform("viewMatrix", glm::value_ptr(PerspViewMatrix), 4, GL_FALSE, 1);
PerspectiveShader.SetUniform("modelMatrix", glm::value_ptr(PerspModelMatrix), 4, GL_FALSE, 1);
PerspectiveShader.SetUniform("texId", textureCount);


glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, textureCount);



if (draw_wireframe == true)
glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


glBindVertexArray(axis_VAO);
glDrawArrays(GL_LINES, 0, 6);
glBindVertexArray(0);


glBindVertexArray(torus_VAO);
glDrawArrays(GL_TRIANGLES, 0, torusVertices.size() / 4);
glBindVertexArray(0);

//shows normal outlines if toggleNormalOutlines is positive, they can be drawn for either smooth or flat shading
if (toggleNormalOutlines > 0 && isFlat == true) {
glBindVertexArray(normalOutlines_VAO);
glDrawArrays(GL_LINES, 0, normalOutlines.size() / 4);
glBindVertexArray(0);
}
else if (toggleNormalOutlines > 0 && isFlat == false) {
glBindVertexArray(normalOutlines_VAO);
glDrawArrays(GL_LINES, 0, normalOutlines.size() / 4);
glBindVertexArray(0);
}




glutSwapBuffers();
}

/*=================================================================================================
INIT
=================================================================================================*/

void init(void) {
// Print some info
std::cout << "Vendor:         " << glGetString(GL_VENDOR) << "\n";
std::cout << "Renderer:       " << glGetString(GL_RENDERER) << "\n";
std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << "\n";
std::cout << "GLSL Version:   " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n\n";

// Set OpenGL settings
glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // background color
glEnable(GL_DEPTH_TEST); // enable depth test
glEnable(GL_CULL_FACE); // enable back-face culling

// Create shaders
CreateShaders();

// Create buffers
CreateAxisBuffers();
CreateTorusBuffer();

std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
std::cout << " 'P' : show / do not show normal wireFrame(toggle)" << std::endl;
std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
std::cout << " 'q' : increment the number of triangles" << std::endl;
std::cout << " 'a' : decrement the number of triangles" << std::endl;
std::cout << " 'w' : increment the r radius(by a small value)" << std::endl;
std::cout << " 's' : decrement the r radius(by a small value)" << std::endl;
std::cout << " 'e' : increment the R radius(by a small value)" << std::endl;
std::cout << " 'd' : decrement the R radius(by a small value)" << std::endl;
std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
std::cout << " 'z' : to enable Flat Shading  " << std::endl;
std::cout << " 'x' : to enable Smooth Shading" << std::endl;
std::cout << " 'c' : show / do not show normal vectors(toggle)" << std::endl;
std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
std::cout << "Finished initializing...\n\n";
}




/*=================================================================================================
MAIN
=================================================================================================*/

int main(int argc, char** argv) {

srand(time(NULL));
glutInit(&argc, argv);

glutInitWindowPosition(100, 100);
glutInitWindowSize(InitWindowWidth, InitWindowHeight);
glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

glutCreateWindow("CSE-170 Computer Graphics");

// Initialize GLEW
GLenum ret = glewInit();
if (ret != GLEW_OK) {
std::cerr << "GLEW initialization error." << std::endl;
glewGetErrorString(ret);
return -1;
}

glutDisplayFunc(display_func);
glutIdleFunc(idle_func);
glutReshapeFunc(reshape_func);
glutKeyboardFunc(keyboard_func);
glutKeyboardUpFunc(key_released);
glutSpecialFunc(key_special_pressed);
glutSpecialUpFunc(key_special_released);
glutMouseFunc(mouse_func);
glutMotionFunc(active_motion_func);
glutPassiveMotionFunc(passive_motion_func);

init();

glutMainLoop();

torusVertices.clear();
normals.clear();
normalOutlines.clear();
textureCoordinates.clear();
torus_colors.clear();


return EXIT_SUCCESS;
}


/*
https://www.tjhsst.edu/~dhyatt/supercomp/n310.html
Some Example Lighting Scenarios

If the normal points toward the light source, the polygon will receive the full light value and will be drawn in the brightest color. Mathematically, the cosine of zero degrees is 1.0, and thus the polygon will receive 100% of the light.

If the normal of the polygon is at some angle greater than 0 but less than 90 degrees, the polygon will receive less light and thus will be drawn a darker shade. Some example angles are:
cos(30) = 0.866 or 87% of the light
cos(45) = 0.707 or 71% of the light
cos(60) = 0.500 or 50% of the light
cos(75) = 0.259 or 26% of the light

If the polygon normal vector and the light source are at 90 degrees, the polygon will be in shadow since the cosine of 90 degrees is zero (0).

Angles between 90 degrees and 270 degrees will return negative values. Thus, the dot product for vectors in this range will be negative and the surface will be colored dark as being in the shadow.

As the angle exceeds 270 degrees and approaches 360, the cosine will again be positive and the light will once again be hitting the polygon.

*/
