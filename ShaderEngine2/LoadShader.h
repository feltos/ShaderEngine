#ifndef LOAD_SHADER

#endif // !LOAD_SHADER

class LoadShader
{
public:
	LoadShader();
	GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);
	~LoadShader();
};


