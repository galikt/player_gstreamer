#ifndef WINDOW_H
#define WINDOW_H

#include <gtkmm.h>
#include <gtkmm/window.h>
#include <gstreamermm.h>
#include <string>
#include <list>
#include <map>
#include "custom_bin.h"

class PWindow : public Gtk::Window
{
public:
  enum PState
  {
    NONE,
    NOT_INIT,
    NOT_READY,
    READY,
    PLAY,
    PAUSE
  };

  struct PStateNode
  {
    PState State;
    std::list<PState> Transition;
  };

protected:
  PState State{PState::NONE};
  std::map<PState, PStateNode> StateGraph;

  Gtk::Menu MainMenu;
  Gtk::VBox MainBox;
  Gtk::HBox ControlBox;
  Gtk::DrawingArea DrawingArea;
  Gtk::Button PlayButton;
  Gtk::Button StopButton;
  Gtk::HScale Scroll;
  std::string CurrentFile;

  Glib::RefPtr<PCustomBin> CustomBin;

  gint64 Duration{0};
  gint64 NewPos{-1};

  guint WatchId;
  guintptr DrawingAreaHandler;
  gulong PadId;
  sigc::connection TimerUpdateControl;

  Glib::ustring InputFile;
  Glib::ustring OutputFile;

public:
  PWindow();
  virtual ~PWindow();

protected:
  void CreateMainMenu(Gtk::Box& box);
  void CreateControl(Gtk::Box& box);
  void InitBin();
  void DeinitBin();
  void SetState(PState new_state);
  void ChangeState(PState new_state);
  std::list<PWindow::PState> SearchPath(PState begin, PState end, std::list<PWindow::PState> path = std::list<PWindow::PState>());

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
  // Gst::PadProbeReturn OnPadBuffer(const Glib::RefPtr<Gst::Pad>& pad, const Gst::PadProbeInfo& data);
  //  void OnVideoChange();
};

#endif