#ifndef H_STATE
#define H_STATE

//#include <bool.h>
#include "space.h"

typedef struct {

  Space *demoSpace;
  Space *draw1DSpace;
  Space *draw2DSpace;
  Space *draw3DSpace;

  bool outlinePolys;
  bool fillPolys;
  int dimension;
  bool draw1D;
  bool draw2D;
  bool draw3D;
  bool removeHidden2D;
  bool removeHidden3D;
  bool removeHidden4D;
  bool rotate2D;
  bool rotate3D;
  bool rotate4D;
  bool demoInitialized;
  bool drawcubeFlag;
  
  double theta;
  double rho;
  double phi; 

} State;

#endif
