//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| AMatrix.h
//|
//| This is the interface to the AMatrix class.  A matrix is a two-dimensional
//| array of numbers.
//|__________________________________________________________________________________

#ifndef HAMATRIX
#define HAMATRIX

class Vector;

class AMatrix
{
  
public:
  
  AMatrix(long pnumRows, long pnumColumns);
  AMatrix(const AMatrix& matrix);
  ~AMatrix();

  void CreateRotationMatrix(long axis1, long axis2, double angle);
  void CreateScaleMatrix(const Vector& scaleVector);
  
  double Element(long row, long column) { return elements[row][column]; }
  void SetElement(long row, long column, double value) { elements[row][column] = value; }
  
  void MakeIdentity();
  void Invert(void);
  double Determinant(void);

  void operator= (const AMatrix &m);
  
  friend AMatrix operator+(const AMatrix &m1, const AMatrix &m);
  friend AMatrix operator-(const AMatrix &m1, const AMatrix &m);
  friend AMatrix operator*(const AMatrix &m1, const AMatrix &m);
  friend Vector operator*(const AMatrix &m, const Vector &v);
  friend AMatrix operator*(double s, const AMatrix &m);
  
  friend AMatrix operator-(const AMatrix &m);
  
//protected:
  
  // An array of pointers to the rows
  // (each row is an array of doubles)
  double **elements;
  
  long	numRows;
  long	numColumns;
  
};

#endif

