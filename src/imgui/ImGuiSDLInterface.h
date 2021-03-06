//
// Created by Petr on 26.10.2021.
//

#ifndef IMGUI_SW_RENDERER__IMGUISDLINTERFACE_H
#define IMGUI_SW_RENDERER__IMGUISDLINTERFACE_H

#include <pf_imgui/ImGuiInterface.h>
#include "impl/imgui_impl_sdl.h"
#include "impl/imgui_impl_sdlrenderer.h"

namespace pf::ui::ig {

struct ImGuiSDLConfig {
  ImGuiConfig imgui;
  SDL_Window *windowHandle;
  SDL_Renderer *renderer;
};

class ImGuiSDLInterface final : public ImGuiInterface {
 public:
  ImGuiSDLInterface(ImGuiSDLConfig config);

  virtual ~ImGuiSDLInterface();

  void updateFonts() override;

  void processInput() override;

 protected:
  void newFrame_impl() override;
  void renderDrawData_impl(ImDrawData *drawData) override;
};

}  // namespace pf::ui::ig

#endif  //IMGUI_SW_RENDERER__IMGUISDLINTERFACE_H
