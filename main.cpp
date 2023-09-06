#include <gtkmm/application.h>
#include <gstreamermm.h>
#include <memory>
#include "window.h"

int main(int argc, char* argv[])
{
  Gst::init(argc, argv);

  auto app = Gtk::Application::create(argc, argv, "org.player");
  auto window = std::make_unique<PWindow>();

  return app->run(*window);
}