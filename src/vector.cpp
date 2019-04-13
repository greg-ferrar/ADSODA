//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Vector.cp
//|
//| This is the implementation of the Vector class.  An Vector is an n-dimensional vector.
//|_______________________________________________________________________________________


//#include "PtrList.h"
#include "halfspace.h"
#include "face.h"
#include "vector.h"
#include "solve.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>


//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Vector::Vector
//|
//| Purpose: This method creates a Vector.
//|
//| Parameters: dim: dimension of the Vector
//|_________________________________________________________________________________

Vector::Vector(long dim)
{

  // Use the specified dimension
  dimension = dim;

  // Allocate space for the coordinates
  coordinates = (double *) malloc(dim * sizeof(double));
  ASSERT(coordinates != NULL);

}	//==== Vector::Vector() ====\\



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Vector::Vector
//|
//| Purpose: This method creates a Vector which is a copy of the passed Vector.
//|
//| Parameters: vector: the vector of which to make a copy
//|_________________________________________________________________________________

Vector::Vector(const Vector &v)
{

  // Make the new vector the same dimension as the other one
  dimension = v.dimension;
	
  // Allocate space for the coordinates
  coordinates =	(double *) malloc(dimension * sizeof(double));
  ASSERT(coordinates != NULL);

  // Copy the coordinates
  memmove(coordinates, v.coordinates, dimension * sizeof(double));

}  //==== Vector::Vector() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Vector::~Vector
//|
//| Purpose: This method disposes of a Vector.
//|
//| Parameters: none
//|_________________________________________________________________________________

Vector::~Vector(void)
{

  // Free memory used by coordinates
  free(coordinates);
	
}	//==== Vector::~Vector() ====\\



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Vector::Dimension
//|
//| Purpose: provides read access to vector's dimension
//|
//| Parameters: returns the dimension of this Vector
//|_________________________________________________________________________________

long Vector::Dimension(void) const
{
  
  // Return the dimension of this vector
  return dimension;
	
}  //==== Vector::Dimension() ====\\



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Vector::Magnitude
//|
//| Purpose: This method finds the length of this Vector
//|
//| Parameters: returns the length of this Vector
//|_________________________________________________________________________________

double Vector::Magnitude(void) const
{
  
  // Start the sum at 0
  register double sum = 0;

  // Sum the squares of the coordinates
  register long i;
  for (i = 0; i < dimension; i++)
    sum += coordinates[i]*coordinates[i];

  //  Return square root of the sum as length  
  return sqrt(sum);
	
}	//==== Vector::Magnitude() ====\\



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Vector::Normalize
//|
//| Purpose: This method makes the length of this Vector 1.0.
//|
//| Parameters: none
//|_________________________________________________________________________________

void Vector::Normalize(void)
{

  // Find the length of this Vector
  register double length = Magnitude();

  // Divide all coordinates by the length
  register long i;
  for (i = 0; i < dimension; i++)
    coordinates[i] /= length;
	
}  //==== Vector::Normalize() ====\\



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Space::IntersectHyperplanes
//|
//| Vector:  This method finds the intersection between dimension hyperplanes.
//|          The hyperplanes are manipulated using their equations.  intersection
//|          is set to the solution of the dimension equations.  This method treats
//|          this as a point.
//|
//| Parameters: hyperplanes:  the hyperplanes to intersect
//|             returns FALSE if there is no solution, or multiple solutions
//|_________________________________________________________________________________

bool Vector::IntersectHyperplanes(vector<Face *>& hyperplanes)
{

  // Allocate memory for a system of equations
  Equation *system = (Equation *) malloc(dimension*sizeof(Equation));
  ASSERT(system != NULL);

  register long i;
  for (i = 0; i < dimension; i++) {

    // Allocate space for one equation
    system[i] = (Equation) malloc((dimension+1)*sizeof(double));
    ASSERT(system[i] != NULL);

    double *coefficients = ((Hyperplane *) hyperplanes[i])->coordinates;

    memmove(system[i], coefficients, (dimension+1)*sizeof(double));
    
  }
  
  //  Solve the equations
  bool valid = SolveEquations(dimension, system, coordinates);
	
  // Free the memory used by the system of equations
  for (i = 0; i < dimension; i++)
    free(system[i]);
  free(system);
  
  return (!valid);

}  //==== Vector::IntersectHyperplanes() ====\\



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Vector::InsideHalfspace
//|
//| Purpose: This method returns true if this is inside the halfspace.  Points which
//|          lie on or very close to the bounding hyperplane are considered to be
//|          outside the halfspace.  This method treats this as a point (not a
//|          vector).
//|
//| Parameters: halfspace: the halfspace to check point against
//|             returns true if point is inside halfspace
//|_________________________________________________________________________________

bool Vector::InsideHalfspace(Halfspace& halfspace)
{

  //
  //  The point is on the inside side of the halfspace if
  //
  //  a x  + a x  + ... + a x + k  >  0
  //   1 1    2 2          n n   
  //
  //  where all ai are the same as in the equation of the hyperplane.
  //  The following code evaluates this inequality.
  //

  double result = 0;
  long i;
  for(i = 0; i < dimension; i++)
    result += coordinates[i] * halfspace.coordinates[i];

  result += halfspace.coordinates[i];
	
  return (result > VERY_SMALL_NUM);

}  //==== Vector::InsideHalfspace() ====\\



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Vector::InsideHalfspaces
//|
//| Purpose: This method returns true if this is inside all the specified halfspaces.
//|          Points which lie on or very close to the bounding hyperplane are
//|          considered to be outside the halfspace.  This method treats this as
//|          a point (not a vector).
//|
//| Parameters: halfspaces: the halfspaces to check this against
//|             point: the point to check
//|             returns true if point is inside all halfspaces
//|_________________________________________________________________________________

bool Vector::InsideHalfspaces(vector<Halfspace *>& halfspaces) {

  // See if there's any halfspace that this vector is NOT inside; if so, return FALSE
  for (vector<Halfspace *>::iterator halfspace = halfspaces.begin(); halfspace != halfspaces.end(); halfspace++)
    if (!InsideHalfspace(**halfspace))
      return false;
  
  // Otherwise, it's inside all the halfspaces
  return true;
	
}  //==== Vector::InsideHalfspaces() ====\\



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Vector::InsideOrOnHalfspace
//|
//| Purpose: This method returns true if point  is inside the Halfspace or on the
//|          boundary.  This method treats point as a point (not a vector).
//|
//| Parameters: halfspace: the halfspace to check this against
//|             returns true if point is inside or on halfspace
//|_________________________________________________________________________________

bool Vector::InsideOrOnHalfspace(Halfspace& halfspace)
{

  //
  //  The point is on the inside side or the boundary of the halfspace if
  //
  //  a x  + a x  + ... + a x + k  >=  0
  //   1 1    2 2          n n   
  //
  //  where all ai are the same as in the equation of the hyperplane.
  //  The following code evaluates the left side of this inequality.
  //

  double result = 0;
  long i;
  for(i = 0; i < dimension; i++)
    result += coordinates[i] * halfspace.coordinates[i];

  result += halfspace.coordinates[i];
	
  return (result > -VERY_SMALL_NUM);

}  //==== Vector::InsideOrOnHalfspace() ====\\



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Vector::InsideOrOnHalfspaces
//|
//| Purpose: This method returns true if for each of the specified halfspaces,
//|          this point is either inside or on the boundary.  This method treats this
//|          as a point (not a vector).
//|
//| Parameters: halfspaces: the halfspaces to check this against
//|             returns true if point is inside or on all halfspaces
//|_________________________________________________________________________________

bool Vector::InsideOrOnHalfspaces(vector<Halfspace *>& halfspaces) {

  // See if there's any halfspace that this vector is NOT inside or on; if so, return false
  for (vector<Halfspace *>::iterator halfspace = halfspaces.begin(); halfspace != halfspaces.end(); halfspace++)
    if (!InsideOrOnHalfspace(**halfspace))
      return false;

  // Otherwise, it's inside or on all the halfspaces
  return true;
	
}  //==== Vector::InsideOrOnHalfspaces() ====\\



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Vector operator+
//|
//| Purpose: This is the Vector addition operator.  v1 and v2 must be of the same
//|          dimension.
//|
//| Parameters: v1: the vector to be added to
//|             v2: the addend
//|             returns the sum of v1 and v2
//|_________________________________________________________________________________

Vector operator+(const Vector &v1, const Vector &v2)
{
  
  Vector sum(v1.Dimension());
	
  // Sum the vectors componentwise, and put the result in the new vector "sum"
  register long i;
  for (i = 0; i < v1.Dimension(); i++)
    sum.coordinates[i] = v1.coordinates[i] + v2.coordinates[i];

  return sum;
	
}  //==== Vector operator+ ====\\



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Vector operator-
//|
//| Purpose: This is the Vector subtraction operator.  v1 and v2 must be of the same
//|          dimension.
//|
//| Parameters: v1: the vector to be subtracted from
//|             v2: the subtrahend
//|             returns the difference of v2 from v1
//|_________________________________________________________________________________

Vector operator-(const Vector &v1, const Vector &v2)
{

  Vector difference(v1.Dimension());
	
  // Subtract the vectors componentwise, and put the result in the new vector "difference"
  register long i;
  for (i = 0; i < v1.Dimension(); i++)
    difference.coordinates[i] =	v1.coordinates[i] - v2.coordinates[i];
	
  return difference;
	
}  //==== Vector operator- ====\\



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Vector operator*
//|
//| Purpose: This is the Vector dot product operator.  v1 and v2 must be of the same
//|          dimension.
//|
//| Parameters: v1: a vector
//|             v2: a vector to be dotted with v1
//|             returns the dot product of v1 and v2
//|__________________________________________________________________________________

double operator*(const Vector &v1, const Vector &v2)
{

  // Compute the sum of the componentwise multiplications (the dot product)
  register double dotProduct = 0;
  register long i;
  for (i = 0; i < v1.Dimension(); i++)
    dotProduct += v1.coordinates[i] * v2.coordinates[i];

  return dotProduct;
	
}  //==== Vector operator* ====\\



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Vector operator-
//|
//| Purpose: This is the unary negation operator for Vector
//|
//| Parameters: v: the Vector
//|             returns the negative of v
//|_________________________________________________________________________________

Vector operator-(const Vector &v)
{
  
  Vector negative(v.Dimension());
	
  // Negate v, and put the result in negative
  register double *vCoordinate = v.coordinates;
  register double *negativeCoordinate =	negative.coordinates;

  register long i;
  for (i = 0; i < v.Dimension(); i++) {
    *negativeCoordinate = -(*vCoordinate);

    vCoordinate++;
    negativeCoordinate++;
  }

  return negative;

}  //==== Vector operator- ====\\



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| Vector operator =
//|
//| Purpose: This is the Vector assignment operator.  this and v must be the same
//|          size.
//|
//| Parameters: v: the Vector to assign to this
//|_________________________________________________________________________________

void Vector::operator = (const Vector& v)
{

  // Copy the coordinates
  memmove(coordinates, v.coordinates, dimension * sizeof(double));

}  //==== Vector operator = ====\\



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| CrossProduct3D
//|
//| Purpose: This computes the cross product of this vector cross v
//|          This vector and v must be 3D vectors.
//|
//| Parameters: v: the Vector to cross with this
//|             returns the cross product
//|_________________________________________________________________________________

Vector Vector::CrossProduct3D(const Vector& v) const {

  Vector crossProduct(3);
  crossProduct.coordinates[0] = coordinates[1]*v.coordinates[2] - coordinates[2]*v.coordinates[1];
  crossProduct.coordinates[1] = coordinates[2]*v.coordinates[0] - coordinates[0]*v.coordinates[2];
  crossProduct.coordinates[2] = coordinates[0]*v.coordinates[1] - coordinates[1]*v.coordinates[0];
  
  return crossProduct;

}  //==== Vector::CrossProduct3D() ====\\

