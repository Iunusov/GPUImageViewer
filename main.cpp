#include <chrono>
#include <cstdlib>
#include <thread>
#include <vector>

#include "Config.hpp"
#include "Renderer2D.hpp"
#include "Scroller.hpp"
#include "VideoContextSDL.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/x.H>

void window_callback(Fl_Widget *) {

  if (Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Escape) {
    // don't do anything when the Escape key is pressed
  } else {
    // exit when the user closes the window
    exit(0);
  }
}

#undef main
int main(int argc, char **argv) {
  Fl::scheme("gtk+");

  int w;
  int h;
  SDL_Init(SDL_INIT_VIDEO);
  VideoContextSDL::getPrimaryDisplayResolution(w, h);
  SDL_Quit();

  // Create main window
  auto window{Fl_Window(w, h, "GPUImageViewer")};
  window.callback(window_callback);
  window.color(FL_BLACK);
  window.resizable(window);
  window.fullscreen();
  window.show();

  Fl_Native_File_Chooser chooser;
  chooser.title("Select the image");
  chooser.type(Fl_Native_File_Chooser::BROWSE_FILE);

  std::vector<std::string> photos;
  if (argc >= 2) {
    photos.emplace_back(argv[1]);
  }

  if (!photos.size())
    switch (chooser.show()) {
    case -1:
      exit(0);
      break;
    case 1:
      exit(0);
      break;
    default:
      photos.clear();
      photos.emplace_back(chooser.filename());
      break; // FILE CHOSEN
    }

  // SDL rendering thread
  std::thread t([&window, &photos]() {
    VideoContextSDL::Create(fl_xid(&window));
    VideoContextSDL::GetInstance()->setup();
    IRenderer *renderer = new Renderer2D{VideoContextSDL::GetInstance()};
    Scroller scroller{};

    const size_t expectedMS{
        (size_t)(1000.0 / VideoContextSDL::GetInstance()->getFps())};

    while (!scroller.escape_key_pressed) {
      const auto start{std::chrono::steady_clock::now()};

      scroller.execute();
      renderer->Render(scroller.GetCameraPos(), scroller.getScale(), photos);
      const auto end{std::chrono::steady_clock::now()};

      const auto elapsedMS{(size_t)(
          std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
              .count())};

      if (expectedMS > elapsedMS) {
        renderer->Delay((size_t)(expectedMS - elapsedMS));
      }
    }
    delete renderer;
    delete VideoContextSDL::GetInstance();
    exit(0);
  });
  t.detach();

  return Fl::run();
}
