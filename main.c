#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include "a1.h"

int main(int argc, char** argv)
{
  RGB *image;
  RGB *output;

  int         my_rank;       /* rank of process      */
  int         p;             /* number of processes  */
  int         source;        /* rank of sender       */
  int         dest;          /* rank of receiver     */
  int         tag = 0;       /* tag for messages     */
  char        message[100];  /* storage for message  */
  MPI_Status  status;        /* status for receive   */

  int width, height, max;
  int *rows, *meta;

  meta = malloc(sizeof(int)*3);
  int i = 0, offset = 0;



  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  int pixelsAbove, pixelsBelow;
  int n = atoi(argv[3]);
  
  int filter;

  // filter == 1 => mean
  // filter == 2 => median 

  if (strcmp(argv[4],"M") == 0)
    filter = 2;
  else 
    filter = 1;

  int buffersize= 0;

  rows = (int*)malloc(sizeof(int)*p);

  // Rank 0 will read the file, and distribute the pixel data to the corresponding processors
  if (my_rank==0){
    image = readPPM(argv[1], &width, &height, &max);
    calcRows(rows, p, height);

    offset = width * rows[0];
    for (i = 1; i < p; i++){
      meta[0]=width;
      meta[1]=height;
      meta[2]=max;

      MPI_Send(meta, 3, MPI_INT, i, 0, MPI_COMM_WORLD);
      MPI_Send(rows, p, MPI_INT, i, 0, MPI_COMM_WORLD);

      pixelsAbove = calculateOffsetAbove(offset, n, width);
      pixelsBelow = calculateOffsetBelow(offset, n, width, height, rows[i]);

      buffersize = rowLength(width, rows[i] + pixelsAbove/width + pixelsBelow/width);

      MPI_Send(image + offset - pixelsAbove, buffersize, MPI_CHAR, i, 0, MPI_COMM_WORLD);
      offset += width * rows[i];  
    } 
  }
  // non rank 0 proccesses will recieve data
  else {

    // recieve meta data
    MPI_Recv(meta, 3, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(rows, p, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    width=meta[0];
    height=meta[1];
    max=meta[2];

    // generate offset
    offset = 0;
    for (i = 0 ; i < my_rank ; i++)
      offset += rows[i];
    offset *= width;

    // calculate window offset
    pixelsAbove = calculateOffsetAbove(offset, n, width);
    pixelsBelow = calculateOffsetBelow(offset, n, width, height, rows[my_rank]);

    // generate the size for the image chunk
    buffersize   = sizeof(RGB)*(rows[my_rank]*width +  pixelsAbove + pixelsBelow);
    image = (RGB*)malloc(buffersize);

    // recieve chunk
    MPI_Recv(image, rowLength(width, rows[my_rank] + pixelsAbove/width + pixelsBelow/width), MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }


  // not rank 0 processes will proccess the image, and send it back to proccessor 0
  if(my_rank!=0)
  {
    RGB* outputImage = (RGB*)malloc(sizeof(RGB)*rows[my_rank]*width);
    

    // calculate
    int rowsAbove = pixelsAbove/width;
    int rowsBelow = pixelsBelow/width;

    processImage(width , rows[my_rank] + rowsAbove + rowsBelow ,width,rows[my_rank], n,rowsAbove,outputImage,image, filter);
    MPI_Send(outputImage, rowLength(width, rows[my_rank]), MPI_CHAR, 0, 0, MPI_COMM_WORLD);

  }
  else{
    //init offset 
    offset = 0;

    //preparing intial image
    RGB *finalImage;
    finalImage = (RGB*)malloc(sizeof(RGB)*height*width);
    
    pixelsAbove = calculateOffsetAbove(offset, n, width);
    pixelsBelow = calculateOffsetBelow(offset, n, width, height, rows[my_rank]);

    int rowsAbove = pixelsAbove/width;
    int rowsBelow = pixelsBelow/width;

    processImage(width , rows[my_rank] + rowsAbove + rowsBelow ,width,rows[my_rank], n,rowsAbove,finalImage,image, filter);

    offset += rows[my_rank]*width;

    //gathering chunks
    for(i = 1; i < p ; i++){
      MPI_Recv(image, rowLength(width, rows[my_rank]), MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      stitch(finalImage, image, rowLength(width, rows[i]), offset);
      offset += width * rows[i];
    }
    writePPM(argv[2], width, height, max, finalImage);
    free(finalImage);
  }
  
  free(image);
  free(rows);

  MPI_Finalize();
  return(0);
}

void calcRows(int* rows, int p, int height) {
  int i = 0;
  for ( i = 0; i < p; i++)
    rows[i] = height/p;

  for ( i = 0; i < height % p; i++)
    rows[i] += 1; 
}

int rowLength(int width, int height) {
  return (width*height*3);
}

void stitch(RGB* dest, RGB* source, int size, int offset){
  memcpy(dest+offset,source,size);
}

int calculateOffsetAbove (int offset, int windowSize, int width){
  int pixelsNeeded = (windowSize -1)/2 *width;

  if ( offset - pixelsNeeded > 0)
    return pixelsNeeded;
  else
    return offset;
}

int calculateOffsetBelow (int offset, int windowSize, int width, int height, int chunkRows){
  int pixelsNeeded = (windowSize -1)/2 *width;

  if (width*height - offset - pixelsNeeded - (chunkRows * width) > 0)
    return pixelsNeeded;
  else
    return (width * height - offset - (chunkRows *width));
}

