#ifndef CANVAS_H
#define CANVAS_H

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


namespace Canvas
{
	class Canvas
	{
	public:
		Canvas(unsigned int canvasWidth,
			unsigned int canvasHeight,
			unsigned int screenResWidth = 800,
			unsigned int screenResHeight = 600);
		~Canvas();

		bool RenderLoop(); // return false for error, true otherwise
		void SetPixel(int witdhIndex, int heightIndex, GLubyte r, GLubyte g, GLubyte b);

	private:

		void initialize(int screenResWidth, int screenResHeight);
		void allocateCanvasPixelArr();
		void deallocateCanvasPixelArr();

		int m_canvasWidth;
		int m_canvasHeight; 
		GLFWwindow* m_window;		
		GLubyte* m_canvasPixelMat;

		int m_nrChannels = 3;	// Since it supports RGB
		unsigned int m_VBO, m_VAO, m_EBO, m_PBO, m_texture;
	};
}


#endif	// !CANVAS_H
