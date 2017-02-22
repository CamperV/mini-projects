import sys
import subprocess
import os
import numpy as np
import cv
import cv2

# take in args
try:
  indir = sys.argv[1]
  grid_arg = sys.argv[2] # true/false
except:
  print "usage: python gistance.py [indir] (grid=true,false)"
  print "NOTE: use shadowdetection/ samples!"
  print "ex: python gistance.py ../shadowdetection/samples/PETs1"
  sys.exit(0)

# build paths
frames_path = indir + "/frames/"
bgs_path = indir + "/bgs/"
shadows_path = indir + "/shadows/"

# init listdir
listing = os.listdir(frames_path)

print "Processing..."
# this loops functions on the assumption that bg name = frame name (including ext)
for infile in sorted(listing):
  fr = frames_path+infile
  bg = bgs_path+infile
  sh = shadows_path+infile

  # split into grid
  # save current grid image into file
  # run gistparams on grid fragment
  # get gistance per each grid
  # reassemble into image and visualize with intensity for gistance

  fr_img = cv2.imread(fr)
  bg_img = cv2.imread(bg)
  rows,cols,channels = fr_img.shape
  roi_w = cols/10   # 64 px
  roi_h = rows/10   # 48 px

  # mask for visualizing gistance
  intensity_mask = np.zeros((rows,cols,channels), np.uint8)

  #for x in range(0,cols-roi_w,roi_w):
    #  for y in range(0,rows-roi_h,roi_h):
  for x in range(0,cols,roi_w):
      for y in range(0,rows,roi_h):
          roi_fr = fr_img[y:y+roi_h, x:x+roi_w]
          roi_bg = bg_img[y:y+roi_h, x:x+roi_w]

          fr_sstream = "fr_"+str(x)+"_"+str(y)+".jpg"
          bg_sstream = "bg_"+str(x)+"_"+str(y)+".jpg"

          cv2.imwrite(fr_sstream, roi_fr)
          cv2.imwrite(bg_sstream, roi_bg)

          fr_gist = np.zeros((960,))
          bg_gist = np.zeros((960,))

          # run with frame
          process = subprocess.Popen(["./bin/gistparams", fr_sstream], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
          for line in process.stdout:   # should be 1 line only
              stripline = line.strip("\n")
              fr_gist = np.fromstring(stripline, dtype=float, sep=" ")

          # run with bg
          process = subprocess.Popen(["./bin/gistparams", bg_sstream], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
          for line in process.stdout:   # should be 1 line only
              stripline = line.strip("\n")
              bg_gist = np.fromstring(stripline, dtype=float, sep=" ")

          # find Euclidean distance between the two GIST descriptors
          gistance = np.linalg.norm(fr_gist-bg_gist)
          #print gistance

          os.remove(fr_sstream)
          os.remove(bg_sstream)

          intensity_mask[y:y+roi_h, x:x+roi_w] = (0,0,200*gistance)

  im_sstream = "im_"+str(infile)
  intensity_img = fr_img + intensity_mask
  cv2.addWeighted(fr_img, 0.6, intensity_mask, 0.4, 0, intensity_img)
  cv2.imwrite(im_sstream, intensity_img)
