#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#define DCT3D_DIM_SHIFT 3
#define DCT3D_DIM_SHIFT_2 6
#define DCT3D_DIM 8
#define DCT3D_TABLE(xyz, uvw) table0[(xyz) + ((uvw) << DCT3D_DIM_SHIFT)]
using namespace glm;
// 3d discrete cosine transform
class DCT3D{
private:
	static const int Dct3D_Size = 1 << (DCT3D_DIM_SHIFT * 3);
	float _data[Dct3D_Size];
	float transformedData[Dct3D_Size];
	float quantizedData[Dct3D_Size];
	float decodedData[Dct3D_Size];

	float table0[Dct3D_Size];
	float c0;

public:
	DCT3D(void){}
	~DCT3D(void){
	}
	void init(void);
	void encode(void);
	void decode(void);
	void quantize(int length);
	float calcError(void);
};