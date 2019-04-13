//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| AMatrix.cp
//|
//| This is the implementation to the AMatrix class.  A AMatrix is a two-dimensional
//| array of numbers.
//|_______________________________________________________________________________________


#include "vector.h"
#include "amatrix.h"


#include <stdlib.h>
#include <string.h>
#include <math.h>


//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| AMatrix::AMatrix
//|
//| Purpose: This method creates a AMatrix.
//|
//| Parameters: pnumRows:    number of rows in the AMatrix
//|             pnumColumns: number of columns in the AMatrix
//|_________________________________________________________________________________

AMatrix::AMatrix(long pnumRows, long pnumColumns)
{

  //  Set the number of rows and columns
  numRows = pnumRows;
  numColumns = pnumColumns;

  //  Allocate space for row pointer array		
  elements = (double **) malloc(numRows * sizeof(double *));
  ASSERT(elements != NULL);

  // Allocate space for all rows
  register long row;
  for (row = 0; row < numRows; row++) {
    elements[row] = (double *) malloc(numColumns * sizeof(double));
    ASSERT(elements[row] != NULL);
  }

}  //==== AMatrix::AMatrix() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| AMatrix::AMatrix
//|
//| Purpose: This method creates a AMatrix which is a copy of the passed AMatrix.
//|
//| Parameters: AMatrix: the AMatrix of which to make a copy
//|_________________________________________________________________________________

AMatrix::AMatrix(const AMatrix &AMatrix)
{

  // Set the number of rows and columns to the same as AMatrix
  numRows = AMatrix.numRows;
  numColumns = AMatrix.numColumns;
	
  // Allocate space for row pointer array		
  elements = (double **) malloc(numRows * sizeof(double *));
  ASSERT(elements != NULL);

  // Allocate space for all rows, and copy them from AMatrix
  register long row;
  for (row = 0; row < numRows; row++) {
    elements[row] = (double *)malloc(numColumns * sizeof(double));
    ASSERT(elements[row] != NULL);
    
    memmove(elements[row], AMatrix.elements[row], numColumns * sizeof(double));
  }

}  //==== AMatrix::AMatrix() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| AMatrix::~AMatrix
//|
//| Purpose: This method disposes of a AMatrix.
//|
//| Parameters: none
//|_________________________________________________________________________________

AMatrix::~AMatrix(void)
{

  // Free memory used by rows
  register long row;
  for (row = 0; row < numRows; row++)
    free(elements[row]);

  // Free memory used by row pointer array		
  free(elements);

} //==== AMatrix::~AMatrix() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| AMatrix::MakeIdentity
//|
//| Purpose: This method makes this AMatrix into the identity.
//|          this AMatrix should be square.
//|
//| Parameters: none
//|_________________________________________________________________________________

void AMatrix::MakeIdentity(void)
{

  // Set all elements to e(i,j)
  register long row;
  register long column;
  for (row = 0; row < numRows; row++) {

    //  Get pointer to first element of row
    register double *thisElement = elements[row];

    // Fill in this row
    for (column = 0; column < numColumns; column++) {
      *thisElement = (column == row);
      thisElement++;
    }
    
  }  // for row

} //==== AMatrix::MakeIdentity() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| AMatrix::Invert
//|
//| Purpose: This method inverts this AMatrix.  this AMatrix should be square.
//|
//| Parameters: none
//|_________________________________________________________________________________

void AMatrix::Invert(void)
{

  //  UNIMPLEMENTED
  ASSERT(0);

}  //==== AMatrix::Invert() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| AMatrix::Determinant
//|
//| Purpose: This method finds the determinant of this AMatrix.  this AMatrix
//|          should be square.
//|
//| Parameters: returns determinant
//|_________________________________________________________________________________

double AMatrix::Determinant(void)
{

  //  UNIMPLEMENTED
  ASSERT(0);

  return 0;

}  //==== AMatrix::Determinant() ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| AMatrix operator =
//|
//| Purpose: This is the AMatrix assignment operator.  this and m must be the same
//|          size.
//|
//| Parameters: m: the AMatrix to assign to this
//|_________________________________________________________________________________

void AMatrix::operator = (const AMatrix& m) {

  register long row;
  register long column;
  for (row = 0; row < m.numRows; row++) {

    // Get pointers to one row of AMatrix and this
    register double *mElement = m.elements[row];
    register double *thisElement = elements[row];
  
    // Copy all entries in this row
    for (column = 0; column < m.numColumns; column++) {

	*thisElement = *mElement;
	
	mElement++;
	thisElement++;

    }  // copy entries

  }  // for rows
	
} //==== AMatrix operator = ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| AMatrix operator +
//|
//| Purpose: This is the AMatrix addition operator.  m1 and m2 must be the same
//|          size.
//|
//| Parameters: m1: the AMatrix to be added to
//|             m2: the addend
//|             returns the sum of m1 and m2
//|_________________________________________________________________________________

AMatrix operator + (const AMatrix &m1, const AMatrix &m2)
{

  // Create a AMatrix to hold the sum
  AMatrix sum(m1.numRows, m1.numColumns);
	
  // Add all elements elementwise, and store the result in sum
  register long row;
  register long column;
  for (row = 0; row < m1.numRows; row++) {
    register double *m1Element = m1.elements[row];
    register double *m2Element = m2.elements[row];
    register double *sumElement = sum.elements[row];
		
    for (column = 0; column < m1.numColumns; column++) {
      *sumElement = *m1Element + *m2Element;
      
      m1Element++;
      m2Element++;
      sumElement++;

    }  // for column

  }  // for row

  return sum;
	
} //==== AMatrix operator + ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| AMatrix operator -
//|
//| Purpose: This is the AMatrix subtraction operator.  m1 and m2 must be the same
//|          size.
//|
//| Parameters: m1: the AMatrix to be subtracted from
//|             m2: the subtrahend
//|             returns the difference of m2 from m1
//|_________________________________________________________________________________

AMatrix operator - (AMatrix& m1, AMatrix& m2)
{

  //  Create a AMatrix to store the difference
  AMatrix difference(m1.numRows, m1.numColumns);
	
  // Subtract m2 from m1 elementwise, and store result in different
  register long row;
  register long column;
  for (row = 0; row < m1.numRows; row++) {

    register double *m1Element = m1.elements[row];
    register double *m2Element = m2.elements[row];
    register double *differenceElement = difference.elements[row];
		
    for (column = 0; column < m1.numColumns; column++) {

      *differenceElement = *m1Element - *m2Element;
			
      m1Element++;
      m2Element++;
      differenceElement++;

    }  // for column

  }  // for row

  return difference;
	
} //==== AMatrix operator - ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| AMatrix operator *
//|
//| Purpose: This is the AMatrix multiplication operator.  m1 must have the same
//|          number of columns as m2 has rows, and the same number of rows as m1
//|          has columns.  In other words, m1 transpose must be the same size as m2.
//|
//| Parameters: m1: the multiplier
//|             m2: the multiplicand
//|             returns the product m1*m2
//|_________________________________________________________________________________

AMatrix operator *(const AMatrix& m1, const AMatrix& m2)
{
  
  // Create a AMatrix to hold the result
  AMatrix product(m1.numRows, m2.numColumns);
	
  // Compute m1*m2 (AMatrix multiplication), and store the result in product
  register long row;
  register long column;
  for (row = 0; row < m1.numRows; row++) {
    
    for (column = 0; column < m2.numColumns; column++) {

      register double sum = 0;

      register long i;
      for (i = 0; i < m1.numColumns; i++)
	sum += m1.elements[row][i] * m2.elements[i][column];

      product.elements[row][column] = sum;

    }  // for column

  }  // for row

  return product;
	
} //==== AMatrix operator * ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| AMatrix operator *
//|
//| Purpose: This is the AMatrix by Vector multiplication operator.  The dimension
//|          of v must be the same as the number of columns in m.
//|
//| Parameters: m: the AMatrix
//|             v: the vector
//|             returns the product m*v
//|_________________________________________________________________________________

Vector operator * (const AMatrix& m, const Vector& v)
{
  
  // Create a new vector to store the product
  Vector product(m.numRows);
	
  // Multiply m*v, and store the result in product
  register long row;
  for (row = 0; row < m.numRows; row++) {

    register double sum = 0;
    
    register long i;
    for (i = 0; i < v.Dimension(); i++)
      sum += m.elements[row][i] * v.coordinates[i];
    
    product.coordinates[row] = sum;
    
  }  // for row
  
  return product;
	
}  //==== AMatrix operator * ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| AMatrix operator *
//|
//| Purpose: This is the scalar by AMatrix multiplication operator.
//|
//| Parameters: s: the scalar
//|             m: the AMatrix
//|             returns the product s*m
//|_________________________________________________________________________________

AMatrix operator * (double s, const AMatrix& m)
{

  // Create a AMatrix to hold the product
  AMatrix product(m.numRows, m.numColumns);
	
  // Compute s*m, and put result in product
  register long row;
  register long column;
  for (row = 0; row < m.numRows; row++) {
    
    register double *mElement = m.elements[row];
    register double *productElement = product.elements[row];
    
    for (column = 0; column < m.numColumns; column++) {
      
      *productElement = s * (*mElement);
      
      mElement++;
      productElement++;
      
    }  // for column
    
  }  // for row
  
  return product;
  
}  //==== AMatrix operator* ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| AMatrix operator - 
//|
//| Purpose: This is the unary negation operator for AMatrix
//|
//| Parameters: m: the AMatrix
//|             returns the negative of m
//|_________________________________________________________________________________

AMatrix operator - (const AMatrix& m)
{
  
  // Create a new AMatrix to hold the negative of this one
  AMatrix negative(m.numRows, m.numColumns);
	
  // Negate m, and put the result in negative
  register long row;
  register long column;
  for (row = 0; row < m.numRows; row++) {
    
    register double *mElement = m.elements[row];
    register double *negativeElement = negative.elements[row];
		
    for (column = 0; column < m.numColumns; column++) {

      *negativeElement = -(*mElement);
			
      mElement++;
      negativeElement++;

    }  // for column

  }  // for row

  return negative;

} //==== AMatrix operator - ====//



//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| AMatrix::CreateRotationMatrix
//|
//| Purpose: This method makes this matrix into a rotation matrix.  The rotation
//|          is described by the indices of two standard basis vectors (defining
//|          the plane parallel to the rotation axis) and an angle.  For instance,
//|          if axis1 == 1, axis2 == 2, and angle == Pi/2, the rotation will be
//|          in the x1x2 plane (the xy plane) around the x3 (z) axis, Pi/3 radians
//|          counterclockwise. 
//|
//| Parameters: axis1: one of the axes in the plane of rotation
//|             axis2: another axis in the plane of rotation
//|             angle: the number of radians to rotate (counterclockwise)
//|_________________________________________________________________________________

void AMatrix::CreateRotationMatrix(long axis1, long axis2, double angle)
{
  
  // Make this the identity matrix
  MakeIdentity();
  
  register double sin_angle = sin(angle);
  register double cos_angle = cos(angle);

  // Change four elements to make this a standard rotation matrix
  elements[axis1-1][axis1-1] = cos_angle;
  elements[axis2-1][axis2-1] = cos_angle;
  elements[axis1-1][axis2-1] = sin_angle;
  elements[axis2-1][axis1-1] = -sin_angle;
  
}  //==== AMatrix::CreateRotationMatrix() ====//
 


//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| AMatrix::CreateScaleMatrix
//|
//| Purpose: This method makes this matrix into a scaling matrix.  Multiplication
//|          by the resulting matrix will scale a vector in the xi direction
//|          according to the ith coordinate of scaleVector. 
//|
//| Parameters: scaleVector: a Vector, each coordinate indicating the scale factor
//|                          for the corresponding axis
//|_________________________________________________________________________________

void AMatrix::CreateScaleMatrix(const Vector& scaleVector)
{
 
  register long row;
  register long column;
  for (row = 0; row < numRows; row++) {

    // Get pointer to first element of row
    register double *thisElement = elements[row];
  
    // Make the diagonals scale factors
    for (column = 0; column < numColumns; column++) {
      if (column == row)
	*thisElement = scaleVector.coordinates[column];

      // Make the off-diagonals 0
      else
	*thisElement = 0;
   
      thisElement++;
    }
  }

} //==== AMatrix::CreateScaleMatrix() ====//

