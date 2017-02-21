#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <stdio.h>
#include <iostream>

#include "heatmap.hpp"

using namespace std;
using namespace cv;

int main(int argc, char** argv) {

  string infile = "";

  if(argc > 1) {  // input video
    infile = argv[1];
  } else {
    cout << "usage: ./impl [input video]" << endl;
    exit(0);
  }

  VideoCapture capture(infile);
  Mat frame;

  capture >> frame;

  for(;;) {

    capture >> frame;
    if(frame.empty()) {
      capture.set(CV_CAP_PROP_POS_AVI_RATIO, 0.0);
      continue;
    }

    //frame.convertTo(frame, CV_BGR2GRAY);
    cvtColor(frame, frame, CV_BGR2GRAY);

    ///////////////////////
    // GET HEATMAP COLOR //
    ///////////////////////

    Mat hmap;
    frame.convertTo(hmap, CV_32F);
    hmap /= 255.0;

    Mat hmap_hm = Mat::zeros(hmap.rows, hmap.cols, CV_32FC3);

    for(int c=0; c<frame.cols; c++) {
      for(int r=0; r<frame.rows; r++) {
        float rd, gn, bl;
        float val = hmap.at<float>(r,c);
        getHeatMapColor(val, rd, gn, bl);
        hmap_hm.at<Vec3f>(r,c).val[0] = bl;
        hmap_hm.at<Vec3f>(r,c).val[1] = gn;
        hmap_hm.at<Vec3f>(r,c).val[2] = rd;
      }
    }

    // Display
    imshow("frame", frame);
    imshow("hmap_hm", hmap_hm);

    if(waitKey(30) >= 0) break;
  }

  return 0;
}
