#ifndef WINDOW_H
#define WINDOW_H

#include <gtkmm.h>
#include <gtkmm/window.h>
#include <gstreamermm.h>
#include <string>
#include "custom_bin.h"

class PWindow : public Gtk::Window
{
protected:
  enum PState
  {
    None,
    NotRedy,
    Redy,
    Play,
    Pause
  } State{PState::None};

  Gtk::Menu MainMenu;
  Gtk::VBox MainBox;
  Gtk::HBox ControlBox;
  Gtk::DrawingArea DrawingArea;
  Gtk::Button PlayButton;
  Gtk::Button StopButton;
  Gtk::HScale Scroll;
  std::string CurrentFile;

  Glib::RefPtr<Gst::PlayBin> Playbin;
  // Glib::RefPtr<PCustomBin> CustomBin;

  gint64 Duration{ 0 };
  gint64 NewPos { -1 };

  guint WatchId;
  guintptr DrawingAreaHandler;
  gulong PadId;
  sigc::connection TimerUpdateControl;

  Glib::ustring OutputFile;

  Glib::RefPtr<Gst::Tee> TeeVideo;
  Glib::RefPtr<Gst::Tee> TeeAudio;
  Glib::RefPtr<Gst::XvImageSink> Video;
  Glib::RefPtr<Gst::Element> Audio;
  Glib::RefPtr<Gst::FileSink> FileSink;

  Glib::RefPtr<Gst::Queue> Queue1;
  Glib::RefPtr<Gst::Queue> Queue2;

public:
  PWindow();
  virtual ~PWindow();

protected:
  void CreateMainMenu(Gtk::Box& box);
  void CreateControl(Gtk::Box& box);
  void InitBin();
  void ChangeState(PState state);

  void OnOpenFile();
  void OnQuit();
  void OnOutputFile();
  void OnButtonPlay();
  void OnButtonStop();
  bool OnScrollChange(Gtk::ScrollType type, double value);
  void OnDrawingAreaRealize();
  bool OnTimeout();
  void OnBusMessageSync(const Glib::RefPtr<Gst::Message>& message);
  bool OnBusMessage(const Glib::RefPtr<Gst::Bus>&, const Glib::RefPtr<Gst::Message>& message);
  //Gst::PadProbeReturn OnPadBuffer(const Glib::RefPtr<Gst::Pad>& pad, const Gst::PadProbeInfo& data);
  // void OnVideoChange();
};

#endif