#include "window.h"
#include <gdk/gdkx.h>

PWindow::PWindow()
{
  OutputFile = get_current_dir_name();
  OutputFile += "/Output.mkv";

  add(MainBox);
  CreateMainMenu(MainBox);

  DrawingArea.set_size_request(640, 360);
  MainBox.pack_start(DrawingArea, Gtk::PACK_EXPAND_WIDGET);
  DrawingArea.signal_realize().connect(sigc::mem_fun(*this, &PWindow::OnDrawingAreaRealize));

  CreateControl(MainBox);

  InitBin();
  ChangeState(PState::NotRedy);

  show_all_children();

  //!
  // ChangeState(PState::Play);
  //!
}

PWindow::~PWindow()
{
  CustomBin->set_state(Gst::STATE_NULL);
  CustomBin->get_bus()->remove_watch(WatchId);
}

void PWindow::CreateMainMenu(Gtk::Box& box)
{
  auto action_group = Gtk::ActionGroup::create();

  action_group->add(Gtk::Action::create("MenuFile", "_File"));
  action_group->add(Gtk::Action::create("Open", Gtk::Stock::OPEN), sigc::mem_fun(*this, &PWindow::OnOpenFile));
  action_group->add(Gtk::Action::create("Quit", Gtk::Stock::QUIT), sigc::mem_fun(*this, &PWindow::OnQuit));

  action_group->add(Gtk::Action::create("MenuSettings", "_Settings"));
  action_group->add(Gtk::Action::create("OutputFile", "_OutputFile"), sigc::mem_fun(*this, &PWindow::OnOutputFile));

  Glib::RefPtr ui_manager = Gtk::UIManager::create();
  ui_manager->insert_action_group(action_group);
  add_accel_group(ui_manager->get_accel_group());

  Glib::ustring ui_info =
      "<ui>"
      "  <menubar name='MenuBar'>"
      "    <menu action='MenuFile'>"
      "      <menuitem action='Open'/>"
      "      <separator/>"
      "      <menuitem action='Quit'/>"
      "    </menu>"
      "    <menu action='MenuSettings'>"
      "      <menuitem action='OutputFile'/>"
      "    </menu>"
      "  </menubar>"
      "</ui>";
  ui_manager->add_ui_from_string(ui_info);

  Gtk::Widget* menu = ui_manager->get_widget("/MenuBar");
  box.pack_start(*menu, Gtk::PACK_SHRINK);
}

void PWindow::CreateControl(Gtk::Box& box)
{
  PlayButton.set_border_width(3);
  PlayButton.signal_clicked().connect(sigc::mem_fun(*this, &PWindow::OnButtonPlay));

  StopButton.set_image_from_icon_name("media-playback-stop", static_cast<Gtk::IconSize>(GTK_ICON_SIZE_SMALL_TOOLBAR));
  StopButton.set_border_width(3);
  StopButton.signal_clicked().connect(sigc::mem_fun(*this, &PWindow::OnButtonStop));

  Scroll.set_range(0, 100);
  Scroll.set_draw_value(false);
  Scroll.signal_change_value().connect(sigc::mem_fun(*this, &PWindow::OnScrollChange));

  box.pack_end(ControlBox, Gtk::PACK_SHRINK);

  ControlBox.pack_start(PlayButton, Gtk::PACK_SHRINK);
  ControlBox.pack_start(StopButton, Gtk::PACK_SHRINK);
  ControlBox.pack_end(Scroll, Gtk::PACK_EXPAND_WIDGET);
}

void PWindow::InitBin()
{
  CustomBin = PCustomBin::create("CustomBin");
  CustomBin->PropertyFileSink() = OutputFile;
  //!
  // CustomBin->PropertyFileSrc() = "/home/user/Downloads/1.mkv";
  //!
  auto bus = CustomBin->get_bus();
  bus->enable_sync_message_emission();
  bus->signal_sync_message().connect(sigc::mem_fun(*this, &PWindow::OnBusMessageSync));

  WatchId = bus->add_watch(sigc::mem_fun(*this, &PWindow::OnBusMessage));
  // Playbin->signal_video_changed().connect(sigc::mem_fun(*this, &PWindow::OnVideoChange));
}

void PWindow::OnOpenFile()
{
  Gtk::FileChooserDialog dialog("Please select a file", Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.set_transient_for(*this);
  dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);
  dialog.add_button("Open", Gtk::RESPONSE_OK);

  int result = dialog.run();
  switch (result)
  {
  case (Gtk::RESPONSE_OK): {
    ChangeState(PState::NotRedy);

    CustomBin->PropertyFileSrc() = dialog.get_filename();
    set_title(Glib::filename_display_basename(dialog.get_filename()));

    ChangeState(PState::Play);
    break;
  }
  }
}

void PWindow::OnOutputFile()
{
  Gtk::FileChooserDialog dialog("Please select a file", Gtk::FILE_CHOOSER_ACTION_SAVE);
  dialog.set_transient_for(*this);
  dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);
  dialog.add_button("Save", Gtk::RESPONSE_OK);
  dialog.set_uri(OutputFile);

  int result = dialog.run();
  switch (result)
  {
  case (Gtk::RESPONSE_OK): {
    ChangeState(PState::Pause);

    CustomBin->PropertyFileSink() = dialog.get_uri();

    ChangeState(PState::Play);
    break;
  }
  }
}

void PWindow::OnQuit()
{
  hide();
}

void PWindow::OnButtonPlay()
{
  ChangeState(State == PState::Play ? PState::Pause : PState::Play);
}

void PWindow::OnButtonStop()
{
  ChangeState(PState::Redy);
}

bool PWindow::OnScrollChange(Gtk::ScrollType type, double value)
{
  double coff = (double)Duration / 100.0;
  NewPos = gint64(value * coff);
  return true;
}

void PWindow::OnDrawingAreaRealize()
{
  DrawingAreaHandler = GDK_WINDOW_XID(DrawingArea.get_window()->gobj());
}

bool PWindow::OnTimeout()
{
  if (NewPos > 0.0)
  {
    CustomBin->seek(Gst::FORMAT_TIME, Gst::SEEK_FLAG_FLUSH, NewPos);
    NewPos = -1;
  }
  else
  {
    Gst::Format format_time = Gst::FORMAT_TIME;
    gint64 pos = 0;

    if (CustomBin->query_position(format_time, pos) && CustomBin->query_duration(format_time, Duration))
    {
      double coff = 100.0 / (double)Duration;
      Scroll.set_value((double)pos * coff);
    }
  }

  return true;
}

void PWindow::ChangeState(PState state)
{
  if (State == state)
  {
    return;
  }

  if ((State == PState::NotRedy) && (state == PState::Play))
  {
    ChangeState(PState::Redy);
  }

  switch (state)
  {
  case PState::NotRedy: {
    PlayButton.set_image_from_icon_name("media-playback-start", static_cast<Gtk::IconSize>(GTK_ICON_SIZE_SMALL_TOOLBAR));
    PlayButton.set_sensitive(false);
    StopButton.set_sensitive(false);
    Scroll.set_value(0.0);
    Scroll.set_sensitive(false);
    CustomBin->set_state(Gst::STATE_NULL);
    break;
  }
  case PState::Redy: {
    PlayButton.set_image_from_icon_name("media-playback-start", static_cast<Gtk::IconSize>(GTK_ICON_SIZE_SMALL_TOOLBAR));
    PlayButton.set_sensitive(true);
    StopButton.set_sensitive(false);
    Scroll.set_value(0.0);
    Scroll.set_sensitive(false);
    CustomBin->set_state(Gst::STATE_READY);
    break;
  }
  case PState::Play: {
    PlayButton.set_image_from_icon_name("media-playback-pause", static_cast<Gtk::IconSize>(GTK_ICON_SIZE_SMALL_TOOLBAR));
    StopButton.set_sensitive(true);
    Scroll.set_sensitive(true);

    CustomBin->set_state(Gst::STATE_PLAYING);

    TimerUpdateControl = Glib::signal_timeout().connect(sigc::mem_fun(*this, &PWindow::OnTimeout), 500);
    break;
  }
  case PState::Pause: {
    PlayButton.set_image_from_icon_name("media-playback-start", static_cast<Gtk::IconSize>(GTK_ICON_SIZE_SMALL_TOOLBAR));
    CustomBin->set_state(Gst::STATE_PAUSED);

    TimerUpdateControl.disconnect();
    break;
  }
  }

  State = state;
}

void PWindow::OnBusMessageSync(const Glib::RefPtr<Gst::Message>& message)
{
  if (!gst_is_video_overlay_prepare_window_handle_message(message->gobj()))
  {
    return;
  }

  if (DrawingAreaHandler != 0)
  {
    auto overlay = Glib::RefPtr<Gst::VideoOverlay>::cast_dynamic(message->get_source());

    if (overlay)
    {
      overlay->set_window_handle(DrawingAreaHandler);
    }
  }
}

bool PWindow::OnBusMessage(const Glib::RefPtr<Gst::Bus>&, const Glib::RefPtr<Gst::Message>& message)
{
  switch (message->get_message_type())
  {
  case Gst::MESSAGE_EOS: {
    ChangeState(PState::Redy);
    break;
  }
  case Gst::MESSAGE_ERROR: {
    ChangeState(PState::Redy);
    break;
  }
  }

  return true;
}

// void PWindow::OnVideoChange()
// {
//   auto pad = Playbin->get_video_pad(0);

//   if (pad)
//   {
//     PadId = pad->add_probe(Gst::PAD_PROBE_TYPE_BUFFER, sigc::mem_fun(*this, &PWindow::OnPadBuffer));
//   }
// }

// Gst::PadProbeReturn PWindow::OnPadBuffer(const Glib::RefPtr<Gst::Pad>& pad, const Gst::PadProbeInfo& data)
// {
//   int width;
//   int height;

//   auto caps = pad->get_current_caps();
//   caps = caps->create_writable();

//   const Gst::Structure structure = caps->get_structure(0);
//   if (structure)
//   {
//     structure.get_field("width", width);
//     structure.get_field("height", height);
//   }

//   DrawingArea.set_size_request(width, height);

//   pad->remove_probe(PadId);
//   PadId = 0;

//   return Gst::PAD_PROBE_OK;
// }
