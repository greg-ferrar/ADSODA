//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| GaussianElimination.cp
//|
//| This implements Gaussian Elimination to solve a system of linear equations.
//|_______________________________________________________________________________________

#include "solve.h"


//
//  Test SolveEquations
//

int test_ge(void)
{

  long n = 2;
  
  double eq1[3] = {2, 1, 1};			//  2x1 + x2 + 1 = 0
  double eq2[3] = {1, -4, 1};			//  x1 - 2x2 + 1 = 0
  
  Equation system[2];
  system[0] = eq1;					//  Set up system of equations
  system[1] = eq2;
  
  double solution[2];
  
  long solution_type = SolveEquations(n, system, solution);
  
  return 0;
  
}	//==== test_ge() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//|
//| Procedure SolveEquations
//|
//| Purpose: This procedure solves n simultaneous equations in n unknowns
//|
//|          This is represented in matrix form as
//|
//|             A * X = B
//|
//|          where A is an n x n matrix of coefficents, X is an n x 1
//|          column vector of the variables xi, and B is an n x 1 column
//|          vector of the constants in the equations.  Expanded, the
//|          matrix equations looks like this:
//|
//|             [ a11 a12 ... a1n ]   [ x1 ]     [ b1 ]
//|             [ a21 a22 ... a2n ]   [ x2 ]     [ b2 ]
//|             [  .     .        ]   [  . ]     [  . ]
//|             [  .       .      ] * [  . ]  =  [  . ]
//|             [  .         .    ]   [  . ]     [  . ]
//|             [ an1 an2 ... ann ]   [ xn ]     [ bn ]
//|
//|          This system of equations is solved, and the values of
//|          xi which make the equation true are returned.
//|
//| Parameters: n:        the number of equations and unknowns
//|             AB:       a matrix of pointers to n Equations.
//|             X:        receives the solution, if any
//|
//|             returns one_solution if there is only one solution
//|                     no_solution if there is no solution
//|                     many_solutions if there are many solutions
//|_________________________________________________________________________________

#define ABS(x)		(((x) > 0) ? (x) : (-x))

enum {
  one_solution,
  no_solutions,
  many_solutions
};

long SolveEquations(register long n, register Equation *AB, register double *X)
{

  //
  //  This code is based on the Gaussian Elimination code on pp. 539-540 of
  //  _Algorithms_, Second Edition, by Robert Sedgewick.
  //
  
  //
  //  The first phase reduces the matrix to an upper diagonal matrix,
  //  that is, a matrix with only zeros below the diagonal.
  //
  
  //  Loop through each row except last
  register long i, j, k, max;
  for (i = 0; i <= n-2; i++) {
    
    //  Assume this row has largest pivot
    max = i;
    
    //  Find row with largest pivot
    for (j = i+1; j <= n-1; j++)
      if (ABS(AB[j][i]) > ABS(AB[max][i]))
	max = j;						
    
    //  Swap row i with largest pivot row
    register Equation temp;
    temp = AB[i];
    AB[i] = AB[max];
    AB[max] = temp;
    
    //  Make pivot zero in all but row i
    for (j = i+1; j <= n-1; j++) {
			
      for (k = n; k >= i; k--)
	AB[j][k] -= AB[i][k]*AB[j][i]/AB[i][i];
      
    }	//  end for j
    
  }	//  end for i


  //
  //  The second phase uses the lower-diagonal matrix to find
  //  a solution.
  //
  
  //  Solve from bottom to top
  for (j = n-1; j >= 0; j--) {
    
    //  Detect singular matrix-- there are many solutions.
    if ((AB[j][j] < VERY_SMALL_NUM) && (AB[j][j] > -VERY_SMALL_NUM))
      return many_solutions;
    
    //  Compute sum of all variables and coefficients
    register double sum = 0.0;
    for (k = j+1; k <= n-1; k++)
      sum += AB[j][k]*X[k];
		
    //  Compute this variable's value.
    X[j] = (-AB[j][n] - sum) / AB[j][j];
    
  }	//  end for j

  //  If we made it this far, there is only one solution, and it is now in X.
  return one_solution;
  
}  //==== SolveEquations() ====//


