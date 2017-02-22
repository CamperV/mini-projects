import sys
import subprocess
import os
import numpy as np

# take in args
try:
  indir = sys.argv[1]
except:
  print "usage: python gistance.py [indir]"
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
# processing
for infile in sorted(listing):
  fr = frames_path+infile
  bg = bgs_path+infile
  sh = shadows_path+infile

  fr_gist = np.zeros((960,))
  bg_gist = np.zeros((960,))

  # run with frame
  process = subprocess.Popen(["./bin/gistparams", fr], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

  # should be 1 line only
  for line in process.stdout:
    stripline = line.strip("\n")
    fr_gist = np.fromstring(stripline, dtype=float, sep=" ")

  # run with bg
  process = subprocess.Popen(["./bin/gistparams", bg], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

  # should be 1 line only
  for line in process.stdout:
    stripline = line.strip("\n")
    bg_gist = np.fromstring(stripline, dtype=float, sep=" ")

  # find Euclidean distance between the two GIST descriptors
  gistance = np.linalg.norm(fr_gist-bg_gist)
  print gistance
