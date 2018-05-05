#define GLEW_STATIC
#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include "LoadShader.h"
#include "BMP.h"
#include "ObjLoader.h"

void MouseCallback(sf::Window* window, double xpos, double ypos);
GLuint Loadcubemap(std::vector<std::string> faces);

LoadShader loader;
BMP bmp;
ObjLoader objLoader;

glm::vec3 cameraPos = glm::vec3(0, 0, 10);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 1920.0 / 2.0;
float lastY = 1080.0 / 2.0;
bool firstMouse = true;

std::vector<std::string> faces
{
	"right.jpg",
	"left.jpg",
	"top.jpg",
	"bottom.jpg",
	"front.jpg",
	"back.jpg"
};
GLuint cubemapTexture = Loadcubemap(faces);

int main()
{
	
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 3;
	settings.minorVersion = 3;
	float width = 1920;
	float height = 1080;

	sf::Window window(sf::VideoMode(width, height), "OpenGL", sf::Style::Default, settings);
	glewExperimental = true;
	glewInit();
	window.setVerticalSyncEnabled(true);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	
	GLuint Texture = bmp.loadBMP_custom("Datas/background_papier.bmp");

	std::vector< glm::vec3 > vertices;
	std::vector< glm::vec2 > uvs;
	std::vector< glm::vec3 > normals; // Won't be used at the moment.

	std::vector< glm::vec3 > vertices2;
	std::vector< glm::vec2 > uvs2;
	std::vector< glm::vec3 > normals2;

	bool obj1 = objLoader.loadOBJ("Datas/sphere.obj", vertices, uvs, normals);
	bool obj2 = objLoader.loadOBJ("Datas/sphere.obj", vertices2, uvs2, normals2);

	glm::vec3 lightPos(1.5f, 1.0f, 0.3f);
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint vertexbuffer2;
	glGenBuffers(1, &vertexbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
	glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(glm::vec3), &vertices2[0], GL_STATIC_DRAW);

	GLuint textureBuffer;
	glGenBuffers(1, &textureBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	GLuint normalBuffer;
	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	// activation de la fenêtreTranslated previous changes of the OpenGL tutorial
	window.setActive(true);
	bool running = true;

	// Create and compile our GLSL program from the shaders
	GLuint programID = loader.LoadShaders("VertexShader.txt", "FragmentShader.txt");
	GLuint programID2 = loader.LoadShaders("LampVertexShader.txt", "LampFragmentShader.txt");

	while (running)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
		glm::mat4 Projection = glm::perspective(glm::radians(30.0f), (float)width / (float)height, 0.1f, 100.0f);

		// Camera matrix
		glm::mat4 View = glm::lookAt(
			cameraPos, cameraPos + cameraFront, cameraUp
		);

		// Model matrix : an identity matrix (model will be at the origin)
		glm::mat4 Model = glm::mat4(1.0f);

		// Our ModelViewProjection : multiplication of our 3 matrices
		glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around
		GLuint MatrixID = glGetUniformLocation(programID, "MVP");
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

		GLuint ModelID = glGetUniformLocation(programID, "Model");
		glUniformMatrix4fv(ModelID, 1, GL_FALSE, &Model[0][0]);

		GLuint lightUniformID = glGetUniformLocation(programID, "lightColor");
		glUniform3fv(lightUniformID, 1, &lightColor[0]);

		GLuint lightPosID = glGetUniformLocation(programID, "lightPos");
		glUniform3fv(lightPosID, 1, &lightPos[0]);

		GLuint CameraPosID = glGetUniformLocation(programID, "cameraPos");
		glUniform3fv(CameraPosID, 1, &cameraPos[0]);

		//VERTEX
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//TEXTURE
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//NORMALS
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

		glDrawArrays(GL_TRIANGLES, 0,vertices.size()); // Starting from vertex 0; 3 vertices total -> 1 triangle
		glDisableVertexAttribArray(0);


		//LIGHT OBJECT
		glUseProgram(programID2);

		glm::mat4 LightModel = glm::mat4(1.0f);
		LightModel = glm::mat4();
		LightModel = glm::translate(LightModel, lightPos);
		LightModel = glm::scale(LightModel, glm::vec3(0.5f));

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			lightPos.x -= 0.1f;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			lightPos.x += 0.1f;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			lightPos.y += 0.1f;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			lightPos.y -= 0.1f;
		}

		glm::mat4 mvp2 = Projection * View * LightModel; // Remember, matrix multiplication is the other way around
		GLuint MatrixID2 = glGetUniformLocation(programID2, "MVP");
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &mvp2[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		
		glDrawArrays(GL_TRIANGLES, 0, vertices2.size());
		glDisableVertexAttribArray(0);

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == event.MouseMoved)
			{
				MouseCallback(&window, event.mouseMove.x, event.mouseMove.y);
			}
			window.setMouseCursorVisible(false);
			float cameraSpeed = 0.08f;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			{
				cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			{
				cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			{
				cameraPos += cameraSpeed * cameraFront;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			{
				cameraPos -= cameraSpeed * cameraFront;
			}
			if (event.type == sf::Event::Closed)
			{
				// on stoppe le programme
				running = false;
			}
			else if (event.type == sf::Event::Resized)
			{
				// on ajuste le viewport lorsque la fenêtre est redimensionnée
				glViewport(0, 0, event.size.width, event.size.height);
			}
		}
		// termine la trame courante (en interne, échange les deux tampons de rendu)
		window.display();
	}

	return 0;
}

void MouseCallback(sf::Window* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.3f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

GLuint Loadcubemap(std::vector<std::string> faces)
{
	GLuint cubeMapBuffer;
	glGenTextures(1, &cubeMapBuffer);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapBuffer);

	for (GLuint i = 0; i < faces.size(); i++)
	{
		sf::Image cubeMapPart;
		if (!cubeMapPart.loadFromFile(faces[i].c_str()))
		{
			std::cout << "cubeMapPart " << i << "could not be loaded \n ";
			return;
		}

		const sf::Uint8 * data = cubeMapPart.getPixelsPtr();
		const sf::Vector2u imageSize = cubeMapPart.getSize();
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, imageSize.x, imageSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

}


