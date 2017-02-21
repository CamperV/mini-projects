/*
 * Returns a linear-scaling heatmap color for float 0.0-1.0
 */

#include "heatmap.hpp"

// only works with floats
void getHeatMapColor(float value, float& red, float& green, float& blue) {

  const int NUM_COLORS = 5;
  static float color[NUM_COLORS][3] = { {0,0,1}, {0,1,1}, {0,1,0}, {1,1,0}, {1,0,0} };

  int idx1;
  int idx2;
  float fractBetween = 0;

  if(value <= 0) idx1 = idx2 = 0;
  else if(value >= 1) idx1 = idx2 = NUM_COLORS-1;
  else {
    value = value * (NUM_COLORS-1);        // Will multiply value by 3
    idx1  = floor(value);                  // Our desired color will be after this index.
    idx2  = idx1+1;                        // ... and before this index (inclusive).
    fractBetween = value - float(idx1);    // Distance between the two indexes (0-1).
  }

  red   = (color[idx2][0] - color[idx1][0])*fractBetween + color[idx1][0];
  green = (color[idx2][1] - color[idx1][1])*fractBetween + color[idx1][1];
  blue  = (color[idx2][2] - color[idx1][2])*fractBetween + color[idx1][2];
}
