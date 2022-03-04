#include "utils/files.h"
#include <SDL.h>
#include <fmt/format.h>
#include <iostream>
#include <pf_imgui/backends/ImGuiSDLInterface.h>
#include <pf_imgui/elements/DragInput.h>
#include <pf_mainloop/MainLoop.h>
#include <toml++/toml.h>

/**
 * Load toml config located next to the exe - config.toml
 * @return
 */
toml::table loadConfig() {
  const auto configPath = pf::getExeFolder() / "config.toml";
  const auto configPathStr = configPath.string();
  fmt::print("Loading config from: '{}'\n", configPathStr);
  return toml::parse_file(configPathStr);
}
/**
 * Serialize UI, save it to the config and save the config next to the exe into config.toml
 */
void saveConfig(toml::table config, pf::ui::ig::ImGuiInterface &imguiInterface) {
  const auto configPath = pf::getExeFolder() / "config.toml";
  const auto configPathStr = configPath.string();
  fmt::print("Saving config file to: '{}'\n", configPathStr);
  imguiInterface.updateConfig();
  config.insert_or_assign("imgui", imguiInterface.getConfig());
  auto ofstream = std::ofstream(configPathStr);
  ofstream << config;
}

int main(int argc, char *argv[]) {
  using namespace pf::ui::ig;
  const auto config = loadConfig();

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fmt::print(std::cerr, "Failed to initialize SDL2\n");
    return -1;
  }
  auto destroySDL = pf::RAII{[&] { SDL_Quit(); }};

  auto window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 800, 0);
  if (window == nullptr) {
    fmt::print(std::cerr, "Failed to initialize SDL2 window\n");
    return -1;
  }
  auto destroyWindow = pf::RAII{[&] { SDL_DestroyWindow(window); }};

  auto surface = SDL_GetWindowSurface(window);
  if (surface == nullptr) {
    fmt::print(std::cerr, "Failed to initialize SDL2 surface\n");
    return -1;
  }

  auto renderer = SDL_CreateSoftwareRenderer(surface);
  if (renderer == nullptr) {
    fmt::print(std::cerr, "Failed to initialize SDL2 renderer\n");
    return -1;
  }
  auto destroyRenderer = pf::RAII{[&] { SDL_DestroyRenderer(renderer); }};

  auto imguiInterface = std::make_unique<ImGuiSDLInterface>(
      ImGuiSDLConfig{.imgui = {.flags = pf::ui::ig::ImGuiConfigFlags::DockingEnable,
                               .config = *config["imgui"].as_table(),
                               .iconFontDirectory = *(*(config["imgui"].as_table()))["path_icons"].value<std::string>(),
                               .enabledIconPacks = IconPack::ForkAwesome},
                     .windowHandle = window,
                     .renderer = renderer});

  auto &uiWindow = imguiInterface->createWindow(uniqueId(), "Window");
  uiWindow.createChild(Button::Config{.name = uniqueId(), .label = "Button", .size = Size::Auto()})
      .addClickListener([&] {
        imguiInterface->getNotificationManager().createNotification(NotificationType::Info, uniqueId(),
                                                                    "Button clicked");
      });
  uiWindow.createChild(DragInput<float>::Config{.name = uniqueId(),
                                                .label = "Float",
                                                .speed = 0.1f,
                                                .min = 0.f,
                                                .max = 1000.f,
                                                .value = 100.f,
                                                .persistent = Persistent::Yes});

  imguiInterface->setStateFromConfig();

  SDL_Event event;
  pf::MainLoop::Get()->setOnMainLoop([&](auto) {
    SDL_RenderClear(renderer);
    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_QUIT) { pf::MainLoop::Get()->stop(); }
      imguiInterface->handleSDLEvent(event);
    }
    imguiInterface->render();
    SDL_UpdateWindowSurface(window);
  });

  fmt::print("Starting main loop\n");
  pf::MainLoop::Get()->run();
  fmt::print("Main loop ended\n");

  saveConfig(config, *imguiInterface);

  return 0;
}