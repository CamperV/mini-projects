#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace cv;

void getHeatMapColor(float value, float& red, float& green, float& blue);

int main(int argc, char** argv) {

  string infile = "";

  if(argc > 1) {  // input video
    infile = argv[1];
  } else {
    cout << "usage: ./mockup [input video]" << endl;
    exit(0);
  }

  VideoCapture capture(infile);
  Mat frame, fg, bg, blurred;

  capture >> frame;

  Mat zmap = Mat::zeros(frame.rows, frame.cols, CV_8U);

  BackgroundSubtractorMOG2 MOG = BackgroundSubtractorMOG2();
  Mat sE_e = getStructuringElement(MORPH_ELLIPSE, Size(3,3));
  Mat sE_d = getStructuringElement(MORPH_ELLIPSE, Size(3,3));

  int counter = 0;
  for(;;) {

    capture >> frame;
    if(frame.empty()) {
      capture.set(CV_CAP_PROP_POS_AVI_RATIO, 0.0);
      counter++;
      continue;
    }
    GaussianBlur(frame, blurred, Size(11,11), 0, 0);

    MOG(frame, fg);
    MOG.getBackgroundImage(bg);

    erode(fg, fg, Mat(), Point(-1, -1), 1);
    /*
    dilate(fg, fg, Mat(), Point(-1, -1), 1);
    erode(fg, fg, Mat(), Point(-1, -1), 1);
    */

    // remove detected shadows
    //threshold(fg, fg, 128, 255, THRESH_BINARY);

    ///////////
    // ZONES //
    ///////////

    if(counter > 0) {
      for(int c=0; c<fg.cols; c++) {
        for(int r=0; r<fg.rows; r++) {
          if( fg.at<unsigned char>(r,c) > 0 &&
              zmap.at<unsigned char>(r,c) < 255)
              zmap.at<unsigned char>(r,c) += 1;
        }
      }
    }

    ///////////////////////
    // GET HEATMAP COLOR //
    ///////////////////////

    Mat zmap_float;
    zmap.convertTo(zmap_float, CV_32F);
    zmap_float /= 255.0;

    Mat zmap_hm = Mat::zeros(zmap_float.rows, zmap_float.cols, CV_32FC3);

    for(int c=0; c<fg.cols; c++) {
      for(int r=0; r<fg.rows; r++) {
        float rd, gn, bl;
        float val = zmap_float.at<float>(r,c);
        getHeatMapColor(val, rd, gn, bl);
        zmap_hm.at<Vec3f>(r,c).val[0] = bl;
        zmap_hm.at<Vec3f>(r,c).val[1] = gn;
        zmap_hm.at<Vec3f>(r,c).val[2] = rd;
      }
    }

    /////////////////////
    // PARTITION ZONES //
    /////////////////////

    Mat zone_low_mask, zone_med_mask, zone_high_mask;
    zone_low_mask = Mat::zeros(zmap_float.rows, zmap_float.cols, CV_8U);
    zone_med_mask = Mat::zeros(zmap_float.rows, zmap_float.cols, CV_8U);
    zone_high_mask = Mat::zeros(zmap_float.rows, zmap_float.cols, CV_8U);

    for(int c=0; c<fg.cols; c++) {
      for(int r=0; r<fg.rows; r++) {
        float rd, gn, bl;
        float val = zmap_float.at<float>(r,c);
        if(val < 0.4) {
          zone_low_mask.at<unsigned char>(r,c) = 255;
        } else if(val >= 0.4 && val < 0.5) {
          zone_med_mask.at<unsigned char>(r,c) = 255;
        } else {
          zone_high_mask.at<unsigned char>(r,c) = 255;
        }
      }
    }

    Mat zone_low, zone_med, zone_high;
    blurred.copyTo(zone_low, zone_low_mask);
    frame.copyTo(zone_med, zone_med_mask);
    frame.copyTo(zone_high, zone_high_mask);

    Mat display = zone_high + zone_med + zone_low;

    // Display
    imshow("frame", frame);
    //imshow("blurred", blurred);
    imshow("fg", fg);
    imshow("bg", bg);
    //imshow("zmap_hm", zmap_hm);
    //imshow("zone_low_mask", zone_low_mask);
    //imshow("zone_med_mask", zone_med_mask);
    //imshow("zone_high_mask", zone_high_mask);
    imshow("zone_low", zone_low);
    imshow("zone_med", zone_med);
    imshow("zone_high", zone_high);
    imshow("display", display);

    if(counter > 10) counter = 1;
    cout << "\rcounter = " << counter << flush;
    if(waitKey(30) >= 0) break;
  }

  return 0;
}

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
