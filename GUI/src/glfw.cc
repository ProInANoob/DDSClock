/*****************************************************************
 * 
 *****************************************************************
 *
 *   Copyright(C) 2024      Twin Oaks Computing, Inc
 *   All rights reserved.   Castle Rock, CO 80108
 *
 *****************************************************************/

#if defined(USE_GLFW)

#include <stdio.h>
#include <stdlib.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#include "robobrawlClock.hh"
#include "GLFW/glfw3.h"

extern bool roboClock::done;


namespace backend {

  static GLFWwindow* window = NULL;

  // -----------------------------------------------------------------------------
  static void glfw_error_callback(int error, const char* description)
  {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
  }

  // -----------------------------------------------------------------------------
  static void set_icon( uint8_t* data, int w, int h )
  {
    GLFWimage icon;
    icon.width = w;
    icon.height = h;
    icon.pixels = data;
    glfwSetWindowIcon( window, 1, &icon );
  }

  // -----------------------------------------------------------------------------
  static void
  init_backend( const char * main_window_title,
                int width,
                int height )
  {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
      exit(-2);

    gui::dpi_scale = 0.5;
  
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    float xscale, yscale;
    glfwGetMonitorContentScale(monitor, &xscale, &yscale);
    if (xscale > 1 || yscale > 1)
      {
        gui::dpi_scale = xscale;
        glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
      }
  
    const auto envDpiScale = getenv( "MD_DPI_SCALE" );
    if( envDpiScale )
      {
        const auto cnv = atof( envDpiScale );
        if( cnv != 0 )
          gui::dpi_scale = cnv;
      }

    width = width * gui::dpi_scale;
    height = height * gui::dpi_scale;
    
    window = glfwCreateWindow( width, height, main_window_title, nullptr, nullptr);
    if (window == nullptr) {
      fprintf( stderr, "Failed to create GLFW window...\n" );
      exit(-1);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
  }

  // -----------------------------------------------------------------------------
  static void
  init_renderer()
  {
    // Setup Platform/Renderer backends
    if (window == nullptr) {
      fprintf( stderr, "null GLFW window...\n" );
      exit(-1);
    }
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();
  }


  // -----------------------------------------------------------------------------
  static void
  poll_events()
  {
    glfwPollEvents();
    if ( glfwWindowShouldClose(window) )
      roboClock::done = true;
  }

  // -----------------------------------------------------------------------------
  static void
  new_frame()
  {
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    glfwGetFramebufferSize(window, &gui::m_win_w, &gui::m_win_h );
  }


  // -----------------------------------------------------------------------------
  static void
  render()
  {
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor( gui::clear_color.x * gui::clear_color.w,
                  gui::clear_color.y * gui::clear_color.w,
                  gui::clear_color.z * gui::clear_color.w,
                  gui::clear_color.w );
    glClear(GL_COLOR_BUFFER_BIT);

    // If you are using this code with non-legacy OpenGL header/contexts (which you should not, prefer using imgui_impl_opengl3.cpp!!),
    // you may need to backup/reset/restore other state, e.g. for current shader using the commented lines below.
    //GLint last_program;
    //glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    //glUseProgram(0);
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    //glUseProgram(last_program);

    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);
  }


  // -----------------------------------------------------------------------------
  static void
  close_renderer()
  {
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();

  }

  // -----------------------------------------------------------------------------
  static void
  close_backend( )
  {
    glfwDestroyWindow(window);
    glfwTerminate();
  }

}

#endif
