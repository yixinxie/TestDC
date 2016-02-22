#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
using namespace glm;
#define DCT_DIM_SHIFT 3
#define DCT_DIM 8
#define DCT_TABLE(xy, uv) table0[(xy) + (uv) * DCT_DIM]
// discrete cosine transform
class DCT{
private:
	float _data[DCT_DIM * DCT_DIM];
	float transformedData[DCT_DIM * DCT_DIM];
	float quantizedData[DCT_DIM * DCT_DIM];
	float decodedData[DCT_DIM * DCT_DIM];
	//inline float inside(int u, int v, int x, int y){
	//	return _data[x + y * DCT_DIM] *
	//		cos((2 * x + 1) * u * glm::pi<float>() / 16.0f) *
	//		cos((2 * y + 1) * v * glm::pi<float>() / 16.0f);
	//}
	//inline float decode_inside(int u, int v, int x, int y){
	//	return transformedData[u + v * DCT_DIM] *
	//		cos((2 * x + 1) * u * glm::pi<float>() / 16.0f) *
	//		cos((2 * y + 1) * v * glm::pi<float>() / 16.0f);
	//}
	float table0[DCT_DIM * DCT_DIM];
	float c0;
	
public:
	DCT(void){}
	~DCT(void){
	}
	void init(void);
	void encode(void);
	void decode(void);
	void quantize(int length);
	float calcError(void);
};
