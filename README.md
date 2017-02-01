# Building a Dartboard Dectector

Created using C++ with [OpenCV](http://opencv.org/).

Program was first trained of a series of negative images. After training the program can detect dartboards in images. See the included [report](report.pdf) for a full writeup.

![Ouput image](images/dart12-final.jpg "Dart board detected")

The Thresholded Gradient Magnitude, a 2D representation of the Hough space and the result of the final detection for two example Dartboard images.

![Process](images/pp.png "Process")

The following Flow diagram shows how we combined evidence from the Hough Transform and Viola-Jones detector.

![Flow diagram](images/flow-diagram.png "Flow diagram")

- We used the Viola-Jones detector to generate boxes around dartboards and other similar images. This produced lots of boxes and a high False Positive Rate (FPR).
- We combined this data with the Hough Circle Transform to remove boxes that didn’t contain a centre of a circle, because we found dartboards were usually round.
- However, to catch slanted dartboards (dartboards that were not a perfect circle) and dartboards that were partly visible we needed to combine evidence from the Hough Line Transform because we found line endings usually clustered around the centre of a dartboard.
- We kept a box if it contained a centre circle, or if it contained more than 5 clustered line endings, to get a lower FPR.