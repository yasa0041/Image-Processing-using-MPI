# Image-Processing-using-MPI

CS4F03
Kevin Ly
1144604
Yash Sapra 
001332927

Discussion:

Process 0, obtains the pixel information from IO.
every process will receive (height/number of processors) rows of pixel data, if the division is not hole, processors will obtain one additional row.

Depending on the window size given, an offset for the upper bound and lower bound must be created. Amount of extra rows needed for each processor if possible is (n-1/2) where n is the window size.

when processing the image, we start increase the image pointer to the # extra rows * width, and determine the valid pixels at N window size around the pixel. Calculate median/mean values and store it in the corresponding output image.

processed image is then sent to process 0, to be built back together.

process 0 processes its own chunk, and then recieves data from each other process and stitches them together.

process 0 produces image.

Old repository link: https://github.com/sapray/image_processing_mpi
