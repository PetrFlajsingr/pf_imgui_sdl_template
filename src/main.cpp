#include "imgui/ImGuiSDLInterface.h"
#include "utils/files.h"
#include <SDL.h>
#include <argparse/argparse.hpp>
#include <fmt/format.h>
#include <iostream>
#include <pf_imgui/elements/DragInput.h>
#include <pf_mainloop/MainLoop.h>
#include <spdlog/spdlog.h>
#include <toml++/toml.h>

argparse::ArgumentParser createArgParser() {
  auto parser = argparse::ArgumentParser{"SDL imgui template"};
  parser.add_description("SDL imgui template project using pf_ libraries");
  return parser;
}

/**
 * Load toml config located next to the exe - config.toml
 * @return
 */
toml::table loadConfig() {
  const auto configPath = pf::getExeFolder() / "config.toml";
  const auto configPathStr = configPath.string();
  spdlog::info("Loading config from: '{}'\n", configPathStr);
  return toml::parse_file(configPathStr);
}
/**
 * Serialize UI, save it to the config and save the config next to the exe into config.toml
 */
void saveConfig(toml::table config, pf::ui::ig::ImGuiInterface &imguiInterface) {
  const auto configPath = pf::getExeFolder() / "config.toml";
  const auto configPathStr = configPath.string();
  spdlog::info("Saving config file to: '{}'\n", configPathStr);
  imguiInterface.updateConfig();
  config.insert_or_assign("imgui", imguiInterface.getConfig());
  auto ofstream = std::ofstream(configPathStr);
  ofstream << config;
}

int main(int argc, char *argv[]) {
  auto parser = createArgParser();
  try {
    parser.parse_args(argc, argv);
  } catch (const std::runtime_error &e) {
    spdlog::error("{}", e.what());
    spdlog::info("{}", parser.help().str());
    return 1;
  }
  using namespace pf::ui::ig;
  const auto config = loadConfig();

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    spdlog::error("Failed to initialize SDL2");
    return -1;
  }
  auto destroySDL = pf::RAII{[&] { SDL_Quit(); }};

  auto window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 800, 0);
  if (window == nullptr) {
    spdlog::error("Failed to initialize SDL2 window");
    return -1;
  }
  auto destroyWindow = pf::RAII{[&] { SDL_DestroyWindow(window); }};

  auto surface = SDL_GetWindowSurface(window);
  if (surface == nullptr) {
    spdlog::error("Failed to initialize SDL2 surface");
    return -1;
  }

  auto renderer = SDL_CreateSoftwareRenderer(surface);
  if (renderer == nullptr) {
    spdlog::error("Failed to initialize SDL2 renderer");
    return -1;
  }
  auto destroyRenderer = pf::RAII{[&] { SDL_DestroyRenderer(renderer); }};

  auto imguiInterface = std::make_unique<ImGuiSDLInterface>(
      ImGuiSDLConfig{.imgui = {.flags = pf::ui::ig::ImGuiConfigFlags::DockingEnable,
                               .config = *config["imgui"].as_table()},
                     .windowHandle = window,
                     .renderer = renderer});

  auto &uiWindow = imguiInterface->createWindow(uniqueId(), "Window");
  uiWindow.createChild(Button::Config{.name = uniqueId(), .label = "Button", .size = Size::Auto()})
      .addClickListener([&] {
        [[maybe_unused]] auto &a = imguiInterface->getNotificationManager().createNotification(uniqueId(), "Button clicked");
      });
  uiWindow.createChild(DragInput<float>::Config{.name = uniqueId(),
                                                .label = "Float",
                                                .speed = 0.1f,
                                                .min = 0.f,
                                                .max = 1000.f,
                                                .value = 100.f,
                                                .persistent = true});

  imguiInterface->setStateFromConfig();

  SDL_Event event;
  pf::MainLoop::Get()->setOnMainLoop([&](auto) {
    SDL_RenderClear(renderer);
    imguiInterface->processInput();
    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_QUIT) { pf::MainLoop::Get()->stop(); }
      //imguiInterface->handleSDLEvent(event);
    }
    imguiInterface->render();
    SDL_UpdateWindowSurface(window);
  });

  spdlog::info("Starting main loop\n");
  pf::MainLoop::Get()->run();
  spdlog::info("Main loop ended\n");

  saveConfig(config, *imguiInterface);

  return 0;
}