//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| GaussianElimination.h
//|
//| This is the interface to a module which implements Gaussian Elimination to solve a
//| system of linear equations.
//|_______________________________________________________________________________________


#include "adsoda_types.h"

//#define VERY_SMALL_NUM	1E-15


//======================= Types ==========================//

//
//  An Equation is an array of doubles.  The first n elements ai are coefficients of
//  xi, and the last is the constant.  So if the array contained {4, -5, 4.5, 3} it
//  would represent the equation
//
//  4x1 - 5x2 + 4.5x3 + 3 = 0
//

typedef double *Equation;			


//======================= Prototypes ==========================//

long SolveEquations(long n, Equation *ABMATRIX, double *X);


