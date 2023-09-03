#ifndef CUSTOM_BIN_H
#define CUSTOM_BIN_H

#include <gstreamermm.h>

class PCustomBin : public Gst::Pipeline
{
public:
  PCustomBin(const Glib::ustring& name);
  virtual ~PCustomBin() = default;

  static Glib::RefPtr<PCustomBin> create(const Glib::ustring& name);

  Glib::RefPtr<Gst::GhostPad> GhostPad;
  Glib::RefPtr<Gst::Tee> Tee;
  Glib::RefPtr<Gst::XvImageSink> Video;
  Glib::RefPtr<Gst::FileSink> FileSink;

};

#endif