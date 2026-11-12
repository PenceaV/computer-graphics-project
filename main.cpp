// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// Include GLEW and GLFW
#include "dependente\glew\glew.h"
#include "dependente\glfw\glfw3.h"

// Include GLM
#include "dependente\glm\glm.hpp"
#include "dependente\glm\gtc\matrix_transform.hpp"
#include "dependente\glm\gtc\type_ptr.hpp"

// Include our helper function for loading shaders
#include "shader.hpp"

// Global variables
GLFWwindow* window;
const int width = 1024, height = 1024;
float scaleX = 1.5f, scaleY = 0.5f, scaleZ = 0;

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		(void)getchar();
		return -1;
	}

	// Create a window and its OpenGL context, retrieve the value into the global variable
	window = glfwCreateWindow(width, height, "Red triangle", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window.");
		(void)getchar();
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		(void)getchar();
		glfwTerminate();
		return -1;
	}

	// Specify the size of the rendering viewport
	glViewport(0, 0, width, height);

	// Clear framebuffer with dark blue color
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Use our helper function to load the shaders from the specified files
	GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

	float g_vertex_buffer_data[] = {
		0.5f,  0.5f, 0.0f,   // top right
		0.5f, -0.5f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};

	unsigned int indices[] = {
		0, 3, 1,   // first triangle
		1, 3, 2,   // second triangle
	};

	// Create VAO, VBO and IBO
	GLuint vao, vbo, ibo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);

	// Bind VAO
	glBindVertexArray(vao);

	// Bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// Bind IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Describe the data in the buffer
	// This will be accesible in the shaders
	glVertexAttribPointer
	(
		0,                    // attribute 0, must match the layout location in the shader
		3,                    // size of each attribute (3 floats)
		GL_FLOAT,             // type of data
		GL_FALSE,             // should be normalized?
		3 * sizeof(GL_FLOAT), // stride for this attribute
		0					  // initial offset for array buffer
	);
	glEnableVertexAttribArray(0);
	
	// To apply transformations unto the vertex positions we will need a matrix.
	// More info about homogeneous coordinates (includes the math explanations): https://www.songho.ca/math/homogeneous/homogeneous.html
	// Another possibly useful resource: https://www.tomdalling.com/blog/modern-opengl/explaining-homogenous-coordinates-and-projective-geometry/
	glm::mat4 transform(1.0f); // This is the identity matrix (no transformations applied yet)

	// Using the helper function from GLM (OpenGL Mathematics), we can then apply a scaling transformation to the identity matrix
	transform = glm::scale(transform, glm::vec3(scaleX, scaleY, scaleZ));
	// A matrix can store multiple transformations, so we can apply multiple transformations to the same object, i.e. multiple scales, multiple rotations, multiple translations
	// We can also retrieve the individual values for transformations/scaling/rotation from the matrix if we ever need them for other calculations
	// You also have to bear in mind that the order in which we apply the transformations is important! i.e.: A * B != B * A

	// Check if the window was closed
	while (!glfwWindowShouldClose(window))
	{
		// Swap buffers
		glfwSwapBuffers(window);
		// Check for events
		glfwPollEvents();
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);
		// Use our shader
		glUseProgram(programID);

		// Send variable to shaders via a "uniform"
		// Uniforms are global shader variables that stay constant for all vertices/fragments during one draw call.
		// We first get the uniform's location (its address inside the GPU program)
		unsigned int transformLoc = glGetUniformLocation(programID, "transform");

		// Then we send the matrix data to the shader using glUniformMatrix4fv
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
		// Function naming pattern: glUniform{N}{type}[v]
		//   N     - number of values (e.g., 1, 2, 3, 4)
		//   type  - data type (f = float, i = int, ui = unsigned int)
		//   v     - indicates a pointer/array version
		// Examples:
		//   glUniform1f(...)        -> send one float
		//   glUniform4fv(...)       -> send an array of four floats (e.g., a vec4)
		//   glUniformMatrix4fv(...) -> send a 4x4 matrix of floats
		// For a full list of functions and more details check the OpenGL documentation: https://registry.khronos.org/OpenGL-Refpages/gl4/html/glUniform.xhtml

		// Bind VAO for our object
		glBindVertexArray(vao);

		// Draw call
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	} 
	
	// Cleanup
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}


