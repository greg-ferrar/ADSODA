#ifndef HCOLOR
#define HCOLOR

class Color {

public:

  double red, green, blue;

  Color(void) {
    red = 0;
    green = 0;
    blue = 0;
  }

  Color(double pred, double pgreen, double pblue) {
    red = pred;
    green = pgreen;
    blue = pblue;
  }


};  //==== class Color ====//


typedef Color Voxel;

#endif
