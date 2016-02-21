#include "DCT.h"
void DCT::init(){
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

	for (int i = 0; i < DCT_DIM * DCT_DIM; i++){
		transformedData[i] = 0;
	}
	for (int i = 0; i < DCT_DIM; i++){
		for (int j = 0; j < DCT_DIM; j++){
			int index = j + i * DCT_DIM;
			table0[index] = cos((2 * i + 1) * j * glm::pi<float>() / 16.0f);

		}
	}
}
void DCT::encode(){
	for (int v = 0; v < DCT_DIM; v++){
		for (int u = 0; u < DCT_DIM; u++){

			float sum = 0;
			for (int y = 0; y < DCT_DIM; y++){
				for (int x = 0; x < DCT_DIM; x++){
					sum += inside(u, v, x, y);
				}
			}
			float cu = (u == 0) ? 1 / glm::sqrt(2) : 1;
			float cv = (v == 0) ? 1 / glm::sqrt(2) : 1;
			

			int index = u + v * DCT_DIM;

			transformedData[index] = cu * cv / 4 * sum;
		}
	}
}
void DCT::decode(){
	for (int y = 0; y < DCT_DIM; y++){
		for (int x = 0; x < DCT_DIM; x++){

			float sum = 0;
			for (int v = 0; v < DCT_DIM; v++){
				for (int u = 0; u < DCT_DIM; u++){
					float cu = (u == 0) ? 1 / glm::sqrt(2) : 1;
					float cv = (v == 0) ? 1 / glm::sqrt(2) : 1;
					sum += cu * cv / 4 * decode_inside(u, v, x, y);
				}
			}
			int index = x + y * DCT_DIM;
			decodedData[index] = sum;
		}
	}
}
