#include "custom_bin.h"

PCustomBin::PCustomBin(const Glib::ustring& name)
    : Gst::Pipeline(name)
{
  FileSrc = Gst::FileSrc::create("FileSrc");
  DecodeBin = Gst::DecodeBin::create("DecodeBin");

  AudioConvert = Gst::AudioConvert::create("AudioConvert");
  AudioSink = Gst::ElementFactory::create_element("autoaudiosink", "AudioSink");

  VideoConvert = Gst::VideoConvert::create("VideoConvert");
  VideoSink = Gst::XvImageSink::create("VideoSink");

  Encoder = Gst::ElementFactory::create_element("x264enc", "Encoder");;
  Mux = Gst::ElementFactory::create_element("matroskamux", "Mux");

  FileSink = Gst::FileSink::create("FileSink");

  // Tee = Gst::Tee::create("Tee");
  // GhostPad = Gst::GhostPad::create(DecodeBin->get_static_pad("sink"), "sink");
  // add_pad(GhostPad);

  add(FileSrc);
  add(DecodeBin);

  add(AudioConvert);
  add(AudioSink);

  add(VideoConvert);
  add(VideoSink);

  // add(Encoder);
  // add(Mux);

  // add(FileSink);

  FileSrc->link(DecodeBin);

  AudioConvert->link(AudioSink);
  VideoConvert->link(VideoSink);

  // Encoder->link(Mux);
  // AudioConvert->link(Mux);

  DecodeBin->signal_pad_added().connect(sigc::mem_fun(*this, &PCustomBin::OnDecodePadAdd));
}

Glib::RefPtr<PCustomBin> PCustomBin::create(const Glib::ustring& name)
{
  return Glib::RefPtr<PCustomBin>(new PCustomBin(name));
}

Glib::PropertyProxy<Glib::ustring> PCustomBin::PropertyFileSrc()
{
  return FileSrc->property_location();
}

Glib::PropertyProxy<Glib::ustring> PCustomBin::PropertyFileSink()
{
  return FileSink->property_location();
}

void PCustomBin::OnDecodePadAdd(const Glib::RefPtr<Gst::Pad>& new_pad)
{
  new_pad->link(VideoConvert->get_static_pad("sink"));
  new_pad->link(AudioConvert->get_static_pad("sink"));
  // new_pad->link(Encoder->get_static_pad("sink"));
}
