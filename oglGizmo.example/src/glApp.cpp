//------------------------------------------------------------------------------
//  Copyright (c) 2018-2019 Michele Morrone
//  All rights reserved.
//
//  https://michelemorrone.eu - https://BrutPitt.com
//
//  twitter: https://twitter.com/BrutPitt - github: https://github.com/BrutPitt
//
//  mailto:brutpitt@gmail.com - mailto:me@michelemorrone.eu
//  
//  This software is distributed under the terms of the BSD 2-Clause license
//------------------------------------------------------------------------------
#include <chrono>
#include <array>
#include <vector>

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include <emscripten/html5.h>
    #include <GLES3/gl3.h>
#else
    #include "libs/glad/glad.h"
#endif

#include "glApp.h"
#include "glWindow.h"


 

// Set the application to null for the linker
mainGLApp* mainGLApp::theMainApp = 0;


#ifdef GLAPP_USE_SDL
void mainGLApp::frameInit()
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return ;
    }

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    #ifdef GLAPP_REQUIRE_OGL45
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    #else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    #endif
#endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    windowTitle+=" - SDL"; 
    mainSDLWwnd = SDL_CreateWindow(getWindowTitle(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, GetWidth(), GetHeight(), SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    gl_context = SDL_GL_CreateContext(mainSDLWwnd);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    gladLoadGL();
}

int mainGLApp::frameExit()
{
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(mainSDLWwnd);
    SDL_Quit();
    return 0;

}
int mainGLApp::getModifier() {
    SDL_Keymod key = SDL_GetModState();
    if(key & KMOD_ALT) return KMOD_ALT;
    else if(key & KMOD_SHIFT) return KMOD_SHIFT;
    else if(key & KMOD_CTRL) return KMOD_CTRL;
    else return 0;
}
#else

#ifndef __EMSCRIPTEN__
    GLFWmonitor* getCurrentMonitor(GLFWwindow *window);
    void toggleFullscreenOnOff(GLFWwindow* window);
    bool isDoubleClick(int button, int action, double x, double y, double ms);    
#endif

static void glfwErrorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //if ((key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) && action == GLFW_PRESS)
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    } //glfwSetWindowShouldClose(window,GLFW_TRUE);
     
    if(key == GLFW_KEY_PRINT_SCREEN && action == GLFW_PRESS) {
        if(scancode & GLFW_MOD_CONTROL) {} // CTRL+PrtScr -> request FileName
        if(!scancode)                   {} // CTRL+PrtScr -> TimeBased FileName
    }
    else if(key >= GLFW_KEY_F1 && key <= GLFW_KEY_F12 && action == GLFW_PRESS) {
        theWnd->onSpecialKeyDown(key, 0, 0);
#ifndef __EMSCRIPTEN__
        if(key == GLFW_KEY_F11) toggleFullscreenOnOff(window);
#endif
    } else if( key == GLFW_KEY_SPACE && action == GLFW_PRESS) { } //SPACE
    else if(action == GLFW_PRESS) {            
        theWnd->onKeyDown(key==GLFW_KEY_ENTER ? 13 : key, 0, 0);
    }
}

void glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{   

    double x,y;
    glfwGetCursorPos(window, &x, &y);

    if (action == GLFW_PRESS) {
#ifndef __EMSCRIPTEN__
        if(isDoubleClick(button, action, x , y, 300)) toggleFullscreenOnOff(window);
        else 
#endif       
            theWnd->onMouseButton(button, GLFW_PRESS, int(x), int(y)); 
        //getApp()->LeftButtonDown();
            
    } else if (action == GLFW_RELEASE) {        
#ifndef __EMSCRIPTEN__
        isDoubleClick(button, action, x , y, 300);
#endif            
        theWnd->onMouseButton(button, GLFW_RELEASE, int(x), int(y)); 
        
        //getApp()->LeftButtonUp();
    }


}


static void glfwCharCallback(GLFWwindow* window, unsigned int c)
{
}

void glfwScrollCallback(GLFWwindow* window, double x, double y)
{
}


static void glfwMousePosCallback(GLFWwindow* window, double x, double y)
{
        //ImGui::GetIO().MousePos.x = (float) x;
        //ImGui::GetIO().MousePos.y = (float) y;

    if((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)  == GLFW_PRESS) || 
       (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) )
        theWnd->onMotion( int(x), int(y)); 
}


void glfwWindowSizeCallback(GLFWwindow* window, int width, int height)
{

    theWnd->onReshape(width,height);
}


#ifndef __EMSCRIPTEN__

bool isDoubleClick(int button, int action, double x, double y, double ms)
{
    static auto before = std::chrono::system_clock::now();
    static int oldButton = -1;
    static double oldx=-1, oldy=-1;    

    auto now = std::chrono::system_clock::now();
    double diff_ms = std::chrono::duration <double, std::milli> (now - before).count();

    bool retval = (diff_ms<ms && (oldx==x && oldy==y) && (action==GLFW_PRESS) && 
                  (oldButton == button && button == GLFW_MOUSE_BUTTON_LEFT));
        
    if(action==GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) before = now ;    
    oldButton = button;
    oldx=x, oldy=y;

    return retval;

}

void toggleFullscreenOnOff(GLFWwindow* window)
{
    static int windowed_xpos, windowed_ypos, windowed_width, windowed_height;
                
    if (glfwGetWindowMonitor(window))
    {
        glfwSetWindowMonitor(window, NULL,
                                windowed_xpos, windowed_ypos,
                                windowed_width, windowed_height, 0);
    }
    else
    {
        GLFWmonitor* monitor = getCurrentMonitor(window);
        if (monitor)
        {
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwGetWindowPos(window, &windowed_xpos, &windowed_ypos);
            glfwGetWindowSize(window, &windowed_width, &windowed_height);
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }
    }
    glfwSwapInterval(1);

}

GLFWmonitor* getCurrentMonitor(GLFWwindow *window)
{
    int nmonitors, i;
    int wx, wy, ww, wh;
    int mx, my, mw, mh;
    int overlap, bestoverlap;
    GLFWmonitor *bestmonitor;
    GLFWmonitor **monitors;
    const GLFWvidmode *mode;

    bestoverlap = 0;
    bestmonitor = NULL;
    
    glfwGetWindowPos(window, &wx, &wy);
    glfwGetWindowSize(window, &ww, &wh);
    monitors = glfwGetMonitors(&nmonitors);

    for (i = 0; i < nmonitors; i++) {
        mode = glfwGetVideoMode(monitors[i]);
        glfwGetMonitorPos(monitors[i], &mx, &my);
        mw = mode->width;
        mh = mode->height;

        overlap =
            std::max(0, std::min(wx + ww, mx + mw) - std::max(wx, mx)) *
            std::max(0, std::min(wy + wh, my + mh) - std::max(wy, my));

        if (bestoverlap < overlap) {
            bestoverlap = overlap;
            bestmonitor = monitors[i];
        }
    }

    return bestmonitor;
}
#endif


GLFWwindow *secondary;

// glfw utils
/////////////////////////////////////////////////
void mainGLApp::frameInit()
{
    glfwSetErrorCallback(glfwErrorCallback);

    if (!::glfwInit()) exit(EXIT_FAILURE);
#ifdef __EMSCRIPTEN__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    //glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API) ;
#else       
    #ifdef GLAPP_REQUIRE_OGL45
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    #else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    #endif
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
#endif    
    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 32);
    
    windowTitle+=" - GLFW"; 
    setGLFWWnd(glfwCreateWindow(GetWidth(), GetHeight(), getWindowTitle(), NULL, NULL));
    if (!getGLFWWnd())
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(getGLFWWnd());

    //Init OpenGL
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    glfwSetKeyCallback(getGLFWWnd(), glfwKeyCallback);
    glfwSetCharCallback(getGLFWWnd(), glfwCharCallback);
    glfwSetMouseButtonCallback(getGLFWWnd(), glfwMouseButtonCallback);
    glfwSetScrollCallback(getGLFWWnd(), glfwScrollCallback);
    glfwSetCursorPosCallback(getGLFWWnd(), glfwMousePosCallback);
    glfwSetWindowSizeCallback(getGLFWWnd(), glfwWindowSizeCallback);

    glfwSwapInterval(1);

}

int mainGLApp::frameExit()
{

    glfwDestroyWindow(getGLFWWnd());
    glfwTerminate();

// need to test exit glfw ... now 0!
    return 0;

}

int mainGLApp::getModifier() {
    GLFWwindow* window = getGLFWWnd();
    if((glfwGetKey(window,GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) || (glfwGetKey(window,GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS))
            return GLFW_MOD_CONTROL;
    else if((glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window,GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
            return GLFW_MOD_SHIFT;
    else if((glfwGetKey(window,GLFW_KEY_LEFT_ALT) == GLFW_PRESS) || (glfwGetKey(window,GLFW_KEY_RIGHT_ALT) == GLFW_PRESS))
            return GLFW_MOD_ALT;
    else return 0;
}
#endif

mainGLApp::mainGLApp() 
{    
    // Allocation in main(...)
    mainGLApp::theMainApp = this;
    glEngineWnd = new glWindow; 
}

mainGLApp::~mainGLApp() 
{
    onExit();

    delete glEngineWnd;
}

void mainGLApp::onInit() 
{

    xPosition = yPosition = -1;
    width = 512; height = 512;
    windowTitle = "VirtualGizmo3D";

//Init OpenGL


// Imitialize both FrameWorks
    frameInit();

// Imitialize both GL engine
    glEngineWnd->onInit();


}

int mainGLApp::onExit()  
{
    glEngineWnd->onExit();
// Exit from both FrameWorks

    frameExit();

// need to test returns code... now 0!        
    return 0;
}


void newFrame()
{
#ifdef __EMSCRIPTEN__
    glfwPollEvents();
#endif

    theWnd->onIdle();
    theWnd->onRender();


#ifdef GLAPP_USE_SDL
    SDL_GL_SwapWindow(theApp->getSDLWWnd());
#else
    //glfwMakeContextCurrent(theApp->getGLFWWnd());
    glfwSwapBuffers(theApp->getGLFWWnd());
#endif

}


void mainGLApp::mainLoop() 
{
#ifdef GLAPP_USE_SDL

        // Main loop
    bool done = false;
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(mainSDLWwnd))
                done = true;
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                theWnd->onMouseButton(event.button.button, SDL_MOUSEBUTTONDOWN, event.button.x, event.button.y); 
            }
            if (event.type == SDL_MOUSEBUTTONUP) {
                theWnd->onMouseButton(event.button.button, SDL_MOUSEBUTTONUP, event.button.x, event.button.y);
            }
            if (event.type == SDL_MOUSEMOTION) {
                theWnd->onMotion(event.motion.x, event.motion.y);
            }


        }
        newFrame();
    }

#else

    while (!glfwWindowShouldClose(getGLFWWnd())) {          

        glfwPollEvents();
        if(!glfwGetWindowAttrib(getGLFWWnd(), GLFW_ICONIFIED)) {
            newFrame();

        }
    }
#endif
}

/////////////////////////////////////////////////
// classic entry point
int main(int argc, char **argv)
{
//Initialize class e self pointer
    theApp = new mainGLApp;    

    theApp->onInit();

/////////////////////////////////////////////////
// Enter in GL main loop
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(newFrame,0,true);
#else
    theApp->mainLoop();
#endif

/////////////////////////////////////////////////
// Exit procedures called from theApp destructor
   
    return 0;

}