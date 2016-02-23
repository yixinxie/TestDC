#include "DCT3D.h"
#include "stdio.h"
void DCT3D::init(){

	for (int i = 0; i < Dct3D_Size; i++){
		_data[i] = 500 + (i % 8);
	}
	for (int xy = 0; xy < DCT3D_DIM; xy++){
		for (int uv = 0; uv < DCT3D_DIM; uv++){
			DCT3D_TABLE(xy, uv) = cos((2 * xy + 1) * uv * glm::pi<float>() / DCT3D_DIM / 2.0f);
		}
	}
	c0 = 1 / glm::sqrt(2);
	initQuantizationIndices();
	
}
void DCT3D::encode(){
	for (int w = 0; w < DCT3D_DIM; w++){
		for (int v = 0; v < DCT3D_DIM; v++){
			for (int u = 0; u < DCT3D_DIM; u++){
				float sum = 0;
				for (int z = 0; z < DCT3D_DIM; z++){
					for (int y = 0; y < DCT3D_DIM; y++){
						for (int x = 0; x < DCT3D_DIM; x++){
							//sum += inside(u, v, x, y);
							sum += _data[x + (y << DCT3D_DIM_SHIFT) + (z << DCT3D_DIM_SHIFT_2)] * DCT3D_TABLE(x, u) * DCT3D_TABLE(y, v) * DCT3D_TABLE(z, w);
						}
					}
				}
				float cu = (u == 0) ? c0 : 1;
				float cv = (v == 0) ? c0 : 1;
				float cw = (w == 0) ? c0 : 1;

				int index = u + (v << DCT3D_DIM_SHIFT) + (w << DCT3D_DIM_SHIFT_2);
				transformedData[index] = cu * cv * cw / 8 * sum;
			}
		}
	}
}
void DCT3D::decode(){
	for (int z = 0; z < DCT3D_DIM; z++){
		for (int y = 0; y < DCT3D_DIM; y++){
			for (int x = 0; x < DCT3D_DIM; x++){

				float sum = 0;
				for (int w = 0; w < DCT3D_DIM; w++){
					for (int v = 0; v < DCT3D_DIM; v++){
						for (int u = 0; u < DCT3D_DIM; u++){
							float cu = (u == 0) ? c0 : 1;
							float cv = (v == 0) ? c0 : 1;
							float cw = (w == 0) ? c0 : 1;
							sum += cu * cv * cw / 8 * quantizedData[u + (v << DCT3D_DIM_SHIFT) + (w << DCT3D_DIM_SHIFT_2)] * DCT3D_TABLE(x, u) * DCT3D_TABLE(y, v) * DCT3D_TABLE(z, w);
						}
					}
				}
				int index = x + (y << DCT3D_DIM_SHIFT) + (z << DCT3D_DIM_SHIFT_2);
				decodedData[index] = sum;
			}
		}
	}
}
float DCT3D::calcError(void){
	float err = 0;
	for (int i = 0; i < Dct3D_Size; i++){
		err += glm::pow(decodedData[i] - _data[i], 2);
	}
	return err;
}
void DCT3D::quantize(int level){
	int quantizeLength = quantizeLevels[level];

	for (int i = 0; i < Dct3D_Size; i++){
		quantizedData[i] = 0;
	}
	for (int i = 0; i < quantizeLength; i++){
		quantizedData[quantizeStepper[i]] = transformedData[quantizeStepper[i]];
	}
	

}
void DCT3D::initQuantizationIndices(){
	quantizeStepper = new int[Dct3D_Size / 2];

	for (int i = 0; i < Dct3D_Size / 2; i++){
		quantizeStepper[i] = -1;
	}

	int incr = 0;
	quantizeStepper[incr] = 0;
	incr++;
	quantizeLevels[0] = 1;
	for (int i = 1; i < DCT3D_DIM; i++){
		printf_s("%d------->\n", i);
		for (int j = 0; j <= i; j++){
			int a = i - j;
			int b = j;
			int c = 0;
			while (a >= 0){
				int index = a + (b << DCT3D_DIM_SHIFT) + (c << DCT3D_DIM_SHIFT_2);
				printf_s("%d, %d, %d-- %d\n", a, b, c, index);
				quantizeStepper[incr] = index;
				incr++;

				a--;
				c++;
			}
			
		}
		quantizeLevels[i] = incr;
	}
	for (int i = 0; i < DCT3D_DIM; i++){
		printf_s("%d, ", quantizeLevels[i]);
	}
}