#ifndef INCLUDED_A1_H
#define INCLUDED_A1_H

void merge_sort (int *a, int n);


typedef struct {
  unsigned char r,g,b;
} RGB;

RGB * readPPM(char* file, int* width, int* height, int* max);
void writePPM(char* file, int width, int height, int max, const RGB *image);

void processImage(int inputWidth, int inputHeight,int outputWidth, int outputHeight, int filterSize, int rowsToSkip, RGB *outputimage, RGB *inputImage, int type);



#endif
