#ifndef CUSTOM_BIN_H
#define CUSTOM_BIN_H

#include <gstreamermm.h>

/*
gst-launch-1.0 filesrc location=1.mkv ! decodebin name=dec ! tee name=tv ! queue ! videoconvert ! autovideosink
dec. ! tee name=ta ! queue ! audioconvert ! autoaudiosink matroskamux name=mux ! filesink location=0.mkv
tv. ! queue ! theoraenc ! mux.video_%u
ta. ! queue ! avenc_eac3 ! mux.audio_%u
*/
class PCustomBin : public Gst::Pipeline
{
public:
  PCustomBin(const Glib::ustring& name);
  virtual ~PCustomBin() = default;

  static Glib::RefPtr<PCustomBin> create(const Glib::ustring& name);

  Glib::RefPtr<Gst::FileSrc> FileSrc;

  // Glib::RefPtr<Gst::DecodeBin> DecodeBin;

  Glib::RefPtr<Gst::AudioConvert> AudioConvert;
  // Glib::RefPtr<Gst::Element> AudioSink;

  Glib::RefPtr<Gst::VideoConvert> VideoConvert;
  // Glib::RefPtr<Gst::XvImageSink> VideoSink;

  Glib::RefPtr<Gst::FileSink> FileSink;

  // Glib::RefPtr<Gst::Element> Encoder;
  // Glib::RefPtr<Gst::Element> Mux;

  Glib::RefPtr<Gst::Tee> TeeVideo;
  Glib::RefPtr<Gst::Tee> TeeAudio;

  Glib::PropertyProxy<Glib::ustring> PropertyFileSrc();
  Glib::PropertyProxy<Glib::ustring> PropertyFileSink();

protected:
  void OnDecodePadAdd(const Glib::RefPtr<Gst::Pad>& pad);
};

#endif