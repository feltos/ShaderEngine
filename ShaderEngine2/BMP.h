#ifndef BMP

#endif // !BMP

#define GLEW_STATIC
#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <vector>

class BMP
{
public:
	BMP();
	GLuint loadBMP_custom(const char * imagepath);
	~BMP();
};

