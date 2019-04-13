#include <iostream.h>
#include <glut.h>
#include "state.h"


void drawcube(void);
void drawDemoFrame(void);


#if 0
void drawall(void){

  drawDemoFrame(*nooshState);
  
}
#endif


void drawcube(void) {

  static int first=1;
  static float vq[8][3];
  static int fr[]= {0,1,5,1,3,7,3,2,6,2,0,4,5,7,6,4};
  if (first) {
    for (int ii = 0; ii < 8; ii++) {
      for(int jj = 0; jj < 3; jj++) {
	vq[ii][jj] = (ii & (1 << jj)) ? 1. : -1.;
	first=0;
      }
    }
  }

  glLineWidth(4);
  glColor3f(1, 1, 1);
  glBegin(GL_LINE_LOOP);
  for (int ii = 0; ii < 16; ii++)
    glVertex3f(vq[fr[ii]][0], vq[fr[ii]][1], vq[fr[ii]][2]);
  glEnd();

} /* end drawcube */

