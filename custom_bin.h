#ifndef CUSTOM_BIN_H
#define CUSTOM_BIN_H

#include <gstreamermm.h>

class PCustomBin : public Gst::Pipeline
{
public:
  PCustomBin(const Glib::ustring& name);
  virtual ~PCustomBin() = default;

  static Glib::RefPtr<PCustomBin> create(const Glib::ustring& name);

  Glib::RefPtr<Gst::FileSrc> FileSrc;

  Glib::RefPtr<Gst::DecodeBin> DecodeBin;

  Glib::RefPtr<Gst::AudioConvert> AudioConvert;
  Glib::RefPtr<Gst::Element> AudioSink;

  Glib::RefPtr<Gst::VideoConvert> VideoConvert;
  Glib::RefPtr<Gst::XvImageSink> VideoSink;

  Glib::RefPtr<Gst::FileSink> FileSink;

  Glib::RefPtr<Gst::Element> Encoder;
  Glib::RefPtr<Gst::Element> Mux;

  // Glib::RefPtr<Gst::GhostPad> GhostPad;
  // Glib::RefPtr<Gst::Tee> Tee;

  Glib::PropertyProxy<Glib::ustring> PropertyFileSrc();
  Glib::PropertyProxy<Glib::ustring> PropertyFileSink();

protected:
  void OnDecodePadAdd(const Glib::RefPtr<Gst::Pad>& new_pad);
};

#endif