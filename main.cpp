#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "game.cpp"
#include <iostream>


//FUNCTION DEFINITIONS//////////////////////////////

    //declare callback func for when window is resized
        void framebuffer_size_callback(GLFWwindow* window, int width, int height){
                    glViewport(0, 0, width, height);
        }   

    //sets close window to true if esc is pressed
        void processInput(GLFWwindow *window)
        {
            if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);
        }

    
////////////////////////////////////////////////////


//DECLARE GAME STUFF///////////////////////////////////////

    //moved to game.cpp
///////////////////////////////////////////////////




Game BulletHell(window_width, window_height); //initialize game object

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
    {
        // if esc is pressed, we should close window
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (key >= 0 && key < 1024)
        {
            if (action == GLFW_PRESS) //record pressed keys in game's key array
                BulletHell.Keys[key] = true;
            else if (action == GLFW_RELEASE)
                BulletHell.Keys[key] = false;
        }
    }

int main()
{
    //start gflw window
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
        glfwWindowHint(GLFW_RESIZABLE, true); //no resizing allowed >:( jk nvm

        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //for mac (using linux so idc)
  
    //create window object
        GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Banana Duck Dodge!", NULL, NULL);

        

        //check if it failed
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
        }

        //makes our window the current window this thread uses
        glfwMakeContextCurrent(window);

    //check if glad runs first before rendering
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }

    //tell opengl window size
        glViewport(0, 0, window_width, window_height);

        //glEnable(GL_BLEND); //review later, was in game cpp, probably dont need

        
    //set callback functions

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //when window resizes, update the viewport

        glfwSetKeyCallback(window, key_callback); //when key pressed, use this callback function


    glEnable(GL_BLEND); //enable blending (for alpha channel)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //make transparency work somehow lol


    //initialize game
    BulletHell.Init();

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;


    //render loop
        while(!glfwWindowShouldClose(window))
        {
            float currentFrame = glfwGetTime(); //calculate frametimes
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            glfwPollEvents(); //polls keyboard inputs 

            //user input
            BulletHell.ProcessInput(deltaTime); // do stuff with keys pressed

            //update game state
            BulletHell.Update(deltaTime);

            //rendering

                
                glClearColor(0.2f, 0.3f, 0.3f, 1.0f); //when clearing color buffer, it will use this color
                glClear(GL_COLOR_BUFFER_BIT); //clear color buffer
                
                BulletHell.Render();

                glfwSwapBuffers(window); //swap color buffers to render

                        

            
        }



    //terminate glfw after done rendering window
    glfwTerminate();


    return 0;
}

