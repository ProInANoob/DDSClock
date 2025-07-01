#ifndef GUIHH
#define GUIHH


#include <string>
#include "imgui.h"
namespace gui {

  extern float   dpi_scale;
  extern int     m_win_w;
  extern int     m_win_h;
  extern ImVec4  clear_color;
  extern float   m_fixedH;
  extern float   m_fixedW;
  
  const ImU32 cyan_col       = IM_COL32( 80, 230, 230, 255);
  const ImU32 green_col      = IM_COL32(150, 250, 150, 255);
  const ImU32 gray_col       = IM_COL32(150, 150, 150, 255);
  const ImU32 red_col        = IM_COL32(250, 80, 80, 255);
  const ImU32 white_col      = IM_COL32(250, 250, 250, 255);
  const ImU32 yellow_col     = IM_COL32(250, 200, 0, 255);
  const ImU32 bluegrey_col   = IM_COL32(1, 30, 50, 255);



  
  void init_backend( const char * win_title );
  void init_imgui( );
  void init_renderer( );
  void set_icon( );

  void add_system( std::string sysName );

  void loop( int domain_id );
  
  void close_renderer( );
  void close_imgui( );
  void close_backend( );

  void draw_dashboard( int domain_id );
  
  static inline bool begin_frame( const std::string & name ) {
    bool rval = ImGui::BeginChild( name.c_str(), 
                                   ImVec2( 0.0f, 0.0f ),
                                   ImGuiChildFlags_AutoResizeY + ImGuiChildFlags_FrameStyle );
    if ( rval ) {
      ImGui::PushStyleColor(ImGuiCol_Text, cyan_col );
      ImGui::Text( "%s", name.c_str() );
      ImGui::PopStyleColor();
    }
    return rval;
  }
  static inline void end_frame( ) {
    ImGui::EndChild();
  }
    

};

#endif