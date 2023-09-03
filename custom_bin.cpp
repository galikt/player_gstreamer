#include "custom_bin.h"

PCustomBin::PCustomBin(const Glib::ustring& name)
    : Gst::Pipeline(name)
{
  Tee = Gst::Tee::create("Tee");
  Video = Gst::XvImageSink::create("ImageSink");
  FileSink = Gst::FileSink::create("FileSink");


  add(Tee);
  add(Video);
  // add(FileSink);

  GhostPad = Gst::GhostPad::create(Tee->get_static_pad("sink"), "sink");
  add_pad(GhostPad);

  auto tee_video_pad = Tee->get_request_pad("src_%u");
  auto video_pad = Video->get_static_pad("sink");
  tee_video_pad->link(video_pad);

  auto tee_file_pad = Tee->get_request_pad("src_%u");
  auto file_pad = FileSink->get_static_pad("sink");
  tee_file_pad->link(file_pad);
}

Glib::RefPtr<PCustomBin> PCustomBin::create(const Glib::ustring& name)
{
  return Glib::RefPtr<PCustomBin>(new PCustomBin(name));
}