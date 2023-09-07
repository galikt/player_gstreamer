#include "custom_bin.h"
#include <regex>
#include <string>

const std::string REGEX_TYPE{"(^\\w*)\\/"};

PCustomBin::PCustomBin(const Glib::ustring& name)
    : Gst::Pipeline(name)
{
  auto link_element = [](const Glib::RefPtr<Gst::Element>& tee, const Glib::RefPtr<Gst::Element>& element) -> auto {
    auto pad_src = tee->get_request_pad("src_%u");
    auto pad_sink = element->get_static_pad("sink");
    pad_sink->set_active(true);

    return pad_src->link(pad_sink);
  };

  FileSrc = Gst::FileSrc::create();
  add(FileSrc);
  auto decode_bin = Gst::DecodeBin::create();
  decode_bin->signal_pad_added().connect(sigc::mem_fun(*this, &PCustomBin::OnDecodePadAdd));
  add(decode_bin);
  TeeVideo = Gst::Tee::create();
  add(TeeVideo);
  TeeAudio = Gst::Tee::create();
  add(TeeAudio);

  auto qv_screen = Gst::Queue::create();
  add(qv_screen);
  auto qa_screen = Gst::Queue::create();
  add(qa_screen);
  auto qv_file = Gst::Queue::create();
  add(qv_file);
  auto qa_file = Gst::Queue::create();
  add(qa_file);

  VideoConvert = Gst::VideoConvert::create();
  add(VideoConvert);
  auto video_sink = Gst::XvImageSink::create();
  add(video_sink);

  AudioConvert = Gst::AudioConvert::create();
  add(AudioConvert);
  auto audio_sink = Gst::ElementFactory::create_element("autoaudiosink");
  add(audio_sink);

  FileSink = Gst::FileSink::create();
  add(FileSink);
  auto mux = Gst::ElementFactory::create_element("matroskamux");
  add(mux);
  auto encoder_video = Gst::ElementFactory::create_element("theoraenc");
  add(encoder_video);
  auto encoder_audio = Gst::ElementFactory::create_element("avenc_eac3");
  add(encoder_audio);

  // воспроизведение
  FileSrc->link(decode_bin);

  link_element(TeeVideo, qv_screen);
  qv_screen->link(VideoConvert);
  VideoConvert->link(video_sink);

  link_element(TeeAudio, qa_screen);
  qa_screen->link(AudioConvert);
  AudioConvert->link(audio_sink);

  // запись в файл
  link_element(TeeVideo, qv_file);
  qv_file->link(encoder_video);
  encoder_video->link(mux);

  link_element(TeeAudio, qa_file);
  qa_file->link(encoder_audio);
  encoder_audio->link(mux);

  mux->link(FileSink);
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

void PCustomBin::OnDecodePadAdd(const Glib::RefPtr<Gst::Pad>& pad)
{
  std::regex regex(REGEX_TYPE);
  std::smatch match;

  std::string format = pad->get_current_caps()->to_string().c_str();
  regex_search(format, match, regex);

  if (match[1] == "video")
  {
    pad->link(TeeVideo->get_static_pad("sink"));
  }
  else if (match[1] == "audio")
  {
    pad->link(TeeAudio->get_static_pad("sink"));
  }
}
