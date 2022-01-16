#include "canvas.h"
#include "shader.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include "root_directory.h"

namespace Canvas
{
	#define BUFFER_OFFSET(i) ((char *)NULL + (i))

	static void processInput(GLFWwindow* window)
	{
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }			
	}

	static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

    static Shader* getShader()
    {
        static Shader* shader;
        static bool isConstucted = false;

        if (!isConstucted)
        {
            std::string textureVsPath = 
                std::string(canvasDir) + "/texture.vs";
            std::string textureFsPath = 
                std::string(canvasDir) + "/texture.fs";

            shader = new Shader(textureVsPath.c_str(), textureFsPath.c_str());
            isConstucted = false;
        }

        return shader;
    }

    void Canvas::allocateCanvasPixelArr() 
    {
        // [Height][Width][Channels]

        m_canvasPixelMat = new GLubyte[m_canvasHeight * m_canvasWidth * m_nrChannels];

        memset(m_canvasPixelMat, 0, m_canvasHeight * m_canvasWidth * m_nrChannels * sizeof(GLubyte));
    }

    void Canvas::deallocateCanvasPixelArr()
    {
        delete[] m_canvasPixelMat;
    }

	void Canvas::initialize(int screenResWidth, int screenResHeight)
	{
        // glfw: initialize and configure
        // ------------------------------
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

        // glfw window creation
        // --------------------
        m_window = glfwCreateWindow(screenResWidth, screenResHeight, "Canvas", NULL, NULL);
        if (m_window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            std::abort();
        }
        glfwMakeContextCurrent(m_window);
        glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);

        // glad: load all OpenGL function pointers
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            std::abort();
        }

        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        float vertices[] =
        {
            // positions          // colors           // texture coords
             1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
             1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
            -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
            -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
        };

        unsigned int indices[] =
        {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
        };

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);

        glBindVertexArray(m_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // texture coord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);


        // load and create a texture 
        // -------------------------
        glGenTextures(1, &m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


        glGenBuffers(1, &m_PBO);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_PBO);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, m_canvasWidth * m_canvasHeight * m_nrChannels, NULL, GL_STREAM_DRAW);
        void* mappedBuffer = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);

        allocateCanvasPixelArr();
        memcpy(mappedBuffer, m_canvasPixelMat, m_canvasWidth * m_canvasHeight * m_nrChannels * sizeof(GLubyte));

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_canvasWidth, m_canvasHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, mappedBuffer);
        glGenerateMipmap(GL_TEXTURE_2D);

        /*
        
            TODO: May not be necessary
        
        */
        // after reading is complete back on the main thread
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_PBO);
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_canvasWidth, m_canvasHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

        getShader()->use();

        /*
            
            TODO: Use shader class function instead

        */
        glUniform1i(glGetUniformLocation(getShader()->ID, "texture1"), 0);
	}

    bool Canvas::RenderLoop() 
    {
        processInput(m_window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture);

        static void* mappedBuffer;

        // ensure we don't try and read data before the transfer is complete
        // then regularly check for completion
        GLsync sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);        
        GLint result;

        /*
            
            TODO: This may not be blocking call

        */
        do 
        {
            glGetSynciv(sync, GL_SYNC_STATUS, sizeof(result), NULL, &result);
            if (result == GL_SIGNALED)
            {
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_PBO);
                glBufferData(GL_PIXEL_UNPACK_BUFFER,
                    m_canvasWidth * m_canvasHeight * m_nrChannels,
                    NULL, GL_STREAM_DRAW);

                mappedBuffer = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
                //now mapped buffer contains the pixel data


                memcpy(mappedBuffer, m_canvasPixelMat, m_canvasWidth * m_canvasHeight * m_nrChannels * sizeof(GLubyte));

                glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_canvasWidth, m_canvasHeight,
                    GL_RGB, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));

                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

                break;
            }
        } while (result != GL_SIGNALED);

        getShader()->use();
        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(m_window);
        glfwPollEvents();   


        return !glfwWindowShouldClose(m_window);
    }

    void Canvas::SetPixel(int witdhIndex, int heightIndex, GLubyte r, GLubyte g, GLubyte b)
    {
        m_canvasPixelMat[heightIndex * m_nrChannels * m_canvasWidth + witdhIndex * m_nrChannels + 0] = r;
        m_canvasPixelMat[heightIndex * m_nrChannels * m_canvasWidth + witdhIndex * m_nrChannels + 1] = g;
        m_canvasPixelMat[heightIndex * m_nrChannels * m_canvasWidth + witdhIndex * m_nrChannels + 2] = b;
    }
	
	Canvas::Canvas(unsigned int canvasWidth, 
                   unsigned int canvasHeight,
                   unsigned int screenResWidth, 
                   unsigned int screenResHeight)
	{
        m_canvasWidth = canvasWidth;
        m_canvasHeight = canvasHeight;

        initialize(screenResWidth, screenResHeight);
	}

	Canvas::~Canvas()
	{
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);

        glfwTerminate();

        delete(getShader());
	}
}
