#include "a1.h"
#include "sorting.h"

void processImage(int inputWidth, int inputHeight,int outputWidth, int outputHeight,int filterSize, int rowsToSkip, RGB *outputimage, RGB *inputImage, int type){

	int row;
	int column;
	int filterRow;
	int filterColumn;
	int filterOffset = (filterSize-1)/2;
	int* medianArrayR = malloc (sizeof(int)*filterSize*filterSize);
	int* medianArrayG = malloc (sizeof(int)*filterSize*filterSize);
	int* medianArrayB = malloc (sizeof(int)*filterSize*filterSize);

	for (row =rowsToSkip;row<(outputHeight+rowsToSkip);row++){
		for (column=0;column<outputWidth;column++){
			int pixelMeanR=0;
			int pixelMeanG=0;
			int pixelMeanB=0;
			int numPixel=0;
			for (filterRow=-filterOffset;filterRow<=filterOffset;filterRow++){
				for (filterColumn=-filterOffset;filterColumn<=filterOffset;filterColumn++){
					
					if (row + filterRow>=0 && row + filterRow<inputHeight && column+filterColumn>=0 && column+filterColumn<inputWidth){
						const RGB *pixel = inputImage+(row+filterRow)*outputWidth+column+filterColumn;
						if(type == 1){
							pixelMeanR += pixel->r;
							pixelMeanG += pixel->g;
							pixelMeanB += pixel->b;
							numPixel++;
						}
						else{
							medianArrayR[numPixel] = pixel -> r ;
							medianArrayG[numPixel] = pixel -> g ;
							medianArrayB[numPixel] = pixel -> b ;
							numPixel++;
						}

					}
				}

			}
			RGB *p = outputimage+(column+((row-rowsToSkip)*outputWidth));
			if(type == 1){

				p->r = pixelMeanR/numPixel;
				p->g = pixelMeanG/numPixel;
				p->b = pixelMeanB/numPixel;

			}
			else {
				
				merge_sort(medianArrayR, numPixel);
				merge_sort(medianArrayG, numPixel);
				merge_sort(medianArrayB, numPixel);


				if ((numPixel % 2) == 0 ){
					p -> r = (medianArrayR[numPixel/2] + medianArrayR[numPixel/2 + 1])/2 ;
					p -> g = (medianArrayG[numPixel/2] + medianArrayG[numPixel/2 + 1])/2 ;
					p -> b = (medianArrayB[numPixel/2] + medianArrayB[numPixel/2 + 1])/2 ;
				}
				else{
					p -> r = medianArrayR[numPixel/2+1];
					p -> g = medianArrayG[numPixel/2+1];
					p -> b = medianArrayB[numPixel/2+1];
				}	
			}

		}
	}
}
