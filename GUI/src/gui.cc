
#include "gui.hh"
#include <iostream>
#include <chrono>
#include <map>
#include <vector>
#include <string>
#include <array>
#define USE_DX11 // remove befreo compileing ( cmake handles os specific backend, vs is ignorant of that )

#if defined(USE_SDL2)
#include "sdl2.cc"
#elif defined(USE_GLFW)
#include "glfw.cc"
#elif defined(USE_DX11)
#include "dx11.cc"

#else
#error A imgui backend has not been defined
#endif

/*
#include "fonts.hh"
*/

#include "icon.hh"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

extern int participantNum;

float gui::dpi_scale = 0.5;
int gui::m_win_w = 0;
int gui::m_win_h = 0;
ImVec4 gui::clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
float gui::m_fixedH = 0.0f;
float gui::m_fixedW = 0.0f;

ImFont *font2;

static uint8_t *iconPx;
static int iconX;
static int iconY;

std::map<std::string, bool> controlWindows;

// -----------------------------------------------------------------------------
static void
setup_dpi_scale(float scale)
{

  // fonts::LoadFonts( scale );
  ImGuiIO &io = ImGui::GetIO();
  ImFont *font1 = io.Fonts->AddFontFromFileTTF("imgui-1.90.9/src/imgui/misc/fonts/DroidSans.ttf", 50 * scale);
  font2 = io.Fonts->AddFontFromFileTTF("imgui-1.90.9/src/imgui/misc/fonts/DroidSans.ttf", 40);
  // ImFont* largeFont = io.Fonts->AddFontFromFileTTF("C:\\Users\\Ethan\\Desktop\\Guages\\src\\src\\test_sans.ttf", 50);
  // ImFont* smallFont = io.Fonts->AddFontFromFileTTF("C:\\Users\\Ethan\\Desktop\\Guages\\src\\src\\test_sans.ttf", 5);

  // io.Fonts->AddFontDefault();

  auto &style = ImGui::GetStyle();
  style = ImGuiStyle();
  ImGui::StyleColorsDark();

  style.WindowBorderSize = 1.f * scale;
  style.FrameBorderSize = 1.f * scale;
  style.FrameRounding = 0.f * scale;
  style.ScaleAllSizes(scale);

  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(1, 1, 1, 0.03f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.45f);
}

// -----------------------------------------------------------------------------
void gui::init_backend(const char *win_title)
{
  backend::init_backend(win_title, 4000, 2000);
}

// -----------------------------------------------------------------------------
void gui::init_imgui()
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  setup_dpi_scale(gui::dpi_scale);

  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

  // ImGui::StyleColorsDark();
  ImGui::StyleColorsLight();
}

// -----------------------------------------------------------------------------
void gui::init_renderer()
{
  backend::init_renderer();
}

// -----------------------------------------------------------------------------
void gui::set_icon()
{
  iconPx = stbi_load_from_memory((const stbi_uc *)Icon_data, Icon_size,
                                 &iconX, &iconY, nullptr, 4);
  backend::set_icon(iconPx, iconX, iconY);
}

// -----------------------------------------------------------------------------
void gui::add_system(std::string sysName){
  controlWindows.insert(std::make_pair(sysName, false));
  // and hother things that I need to do I guess.. 


}

// -----------------------------------------------------------------------------
/* called each time through 'main_loop' to refresh gui
 * and collect/handle user input
 */
void gui::loop(int domain_id)
{

  static int counter = 0;

  // Poll and handle events (inputs, window resize, etc.)
  // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard
  //  flags to tell if dear imgui wants to use your inputs.
  // - When io.WantCaptureMouse is true, do not dispatch mouse input
  //   data to your main application, or clear/overwrite your copy of
  //   the mouse data.
  // - When io.WantCaptureKeyboard is true, do not dispatch keyboard
  //   input data to your main application, or clear/overwrite your
  //   copy of the keyboard data.
  // Generally you may always pass all inputs to dear imgui, and hide
  // them from your application based on those two flags.
  backend::poll_events();

  backend::new_frame();
  ImGui::NewFrame();

  if (counter > 2)
  {
    // ImGui::PushFont( fonts::s_fixedWidth );
    gui::m_fixedW = ImGui::CalcTextSize("X").x;
    gui::m_fixedH = ImGui::CalcTextSize("X").y;
    // ImGui::PopFont( );

    // fill the entire 'window'
#ifdef IMGUI_HAS_VIEWPORT
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->GetWorkPos());
    ImGui::SetNextWindowSize(viewport->GetWorkSize());
    ImGui::SetNextWindowViewport(viewport->ID);
#else
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
#endif

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::Begin("StackLight",
                 nullptr,
                 ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoScrollWithMouse |
                     ImGuiWindowFlags_NoBringToFrontOnFocus |
                     ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoFocusOnAppearing |
                     ImGuiWindowFlags_NoMove |
                     // ImGuiWindowFlags_NoDocking |
                     ImGuiWindowFlags_NoNavFocus);

    // GUI CONTENTS:
    gui::draw_dashboard(domain_id);

    ImGui::End();
    ImGui::PopStyleVar(1);
  }
  else
  {
    counter++;
  }

  // ImGui::ShowDemoWindow();
  // ImPlot::ShowDemoWindow();

  // Rendering
  ImGui::Render();
  backend::render();
}

// -----------------------------------------------------------------------------
void gui::close_renderer()
{
  backend::close_renderer();
}

// -----------------------------------------------------------------------------
void gui::close_imgui()
{
  ImGui::DestroyContext();
}

// -----------------------------------------------------------------------------
void gui::close_backend()
{
  backend::close_backend();
}

void putTextInBox(ImDrawList *drawlist, ImVec2 pMin, ImVec2 pMax, const char text[], ImU32 color = IM_COL32(0, 0, 0, 255))
{
  ImVec2 pos = pMin;
  pos.y += ((pMax.y - pMin.y) / 2) - (ImGui::CalcTextSize(text).y / 2);
  pos.x += (pMax.x - pMin.x) / 2 - ImGui::CalcTextSize(text).x / 2;

  drawlist->AddText(pos, color, text);
}

void gui::draw_dashboard(int domain_id)
{
  //std::vector<std::string> ids = roboClock::known_devices.known_device_ids();

  //for(std::vector<std::string>::iterator it = ids.begin(); it != ids.end(); it++){
  //  std::cout << "AHHHHHH";
  //  ImGui::Text((*it).c_str());
  //}
  //std::cout << "\n";

  // I dont have any devices tooo test this against so imma start on the sesps.
  // i Think im gonna do this wiht beins 
  std::map<std::string, std::map<DeviceRole, std::vector<std::string>*>> org = roboClock::known_devices.getOrg();
  ImGui::Begin("Systems");
  {
    
    for(auto & pair : org){
      ImGui::Text(pair.first.c_str());
      ImGui::SameLine();
      std::string buttonText = pair.first + " control";
      if(ImGui::Button(buttonText.c_str())){
        controlWindows[pair.first] = !controlWindows[pair.first];
      }

    }
  }

  for(auto & pair : controlWindows){
    if(pair.second){
      // window is open 
      
      //list items
      ImGui::Text(" BUTTONS: ");
      for(auto it = org[pair.first][DeviceRole::ROLE_BUTTON]->begin(); it != org[pair.first][DeviceRole::ROLE_BUTTON]->end(); it++){
        ImGui::Text((*it).c_str());
        // add buttons and stuff for the buttons. ( to send sys change message at least.. nick probably too.)
      }

      for(auto it = org[pair.first][DeviceRole::ROLE_CLOCK]->begin(); it != org[pair.first][DeviceRole::ROLE_CLOCK]->end(); it++){
        ImGui::Text((*it).c_str());
        // add button fro clock control ( like sysname nick, not start stop stuf. thats in this sys control menu.)
      } 



      if(ImGui::Button("Close")){
        pair.second = false; // not sure this will stay in the map. 
      }
    }
  }


  

}