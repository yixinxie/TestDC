#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
using namespace glm;
#define DCT_DIM 8
// discrete cosine transform
class DCT{
private:
	float _data[DCT_DIM * DCT_DIM];
	float transformedData[DCT_DIM * DCT_DIM];
	float decodedData[DCT_DIM * DCT_DIM];
	inline float inside(int u, int v, int x, int y){
		return _data[x + y * DCT_DIM] *
			cos((2 * x + 1) * u * glm::pi<float>() / 16.0f) *
			cos((2 * y + 1) * v * glm::pi<float>() / 16.0f);
	}
	inline float decode_inside(int u, int v, int x, int y){
		return transformedData[u + v * DCT_DIM] *
			cos((2 * x + 1) * u * glm::pi<float>() / 16.0f) *
			cos((2 * y + 1) * v * glm::pi<float>() / 16.0f);
	}
	float table0[DCT_DIM * DCT_DIM];
public:
	DCT(void){}
	~DCT(void){
		delete _data;
	}
	void init(void);
	void encode(void);
	void decode(void);
	
};