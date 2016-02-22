#include "DCT3D.h"
void DCT3D::init(){
	float initialData[] = {
		120,	108,	90,		75,	69,	73,	82,	89,
		127,	115,	97,		81,	75,	79,	88,	95,
		134,	122,	105,	89,	83,	87,	96,	103,
		137,	125,	107,	92,	86,	90,	99,	106,
		131,	119,	101,	86,	80,	83,	93,	100,
		117,	105,	87,		72,	65,	69,	78,	85,
		100,	88,		70,		55,	49,	53,	62,	69,
		89,		77,		59,		44,	38,	42,	51,	58, };
	memcpy_s(_data, sizeof(initialData), initialData, sizeof(initialData));

	for (int i = 0; i < Dct3D_Size; i++){
		_data[i] = 500 + (i % 8);
	}
	for (int xy = 0; xy < DCT3D_DIM; xy++){
		for (int uv = 0; uv < DCT3D_DIM; uv++){
			DCT3D_TABLE(xy, uv) = cos((2 * xy + 1) * uv * glm::pi<float>() / DCT3D_DIM / 2.0f);
		}
	}
	c0 = 1 / glm::sqrt(2);
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
							sum += cu * cv * cw / 8 * transformedData[u + (v << DCT3D_DIM_SHIFT) + (w << DCT3D_DIM_SHIFT_2)] * DCT3D_TABLE(x, u) * DCT3D_TABLE(y, v) * DCT3D_TABLE(z, w);
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
void DCT3D::quantize(int length){
	int quantizeStepper[] = { 0, 1, 8, 16, 9, 2, 3, 10, 17, 24,
		32, 25, 18, 11, 4 };
	for (int i = 0; i < Dct3D_Size; i++){
		quantizedData[i] = 0;
	}
	for (int i = 0; i < length; i++){
		quantizedData[quantizeStepper[i]] = transformedData[quantizeStepper[i]];
	}
	

}