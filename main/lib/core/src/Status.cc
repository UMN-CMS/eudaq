#include "eudaq/Serializer.hh"
#include "eudaq/Status.hh"
#include "eudaq/Exception.hh"
#include "eudaq/Utils.hh"

namespace eudaq {

  Status::Status(Deserializer &ds) {
    ds.read(m_level);
    ds.read(m_state);
    ds.read(m_msg);
    ds.read(m_tags);
  }

  void Status::Serialize(Serializer &ser) const {
    ser.write(m_level);
    ser.write(m_state);
    ser.write(m_msg);
    ser.write(m_tags);
  }
  
  void Status::ResetStatus(State st, Level lvl, const std::string &msg){
    m_state = st;
    m_level = lvl;
    m_msg = msg;
  }

  void Status::ResetTags(){
    m_tags.clear();
  }
  
  std::string Status::Level2String(int level) {
    static const char *const strings[] = {"DEBUG", "OK",   "THROW", "EXTRA",
                                          "INFO",  "WARN", "ERROR", "USER",
					  "NONE"};
    if (level < LVL_DEBUG || level > LVL_NONE)
      return "";
    return strings[level];
  }

  int Status::String2Level(const std::string &str) {
    int lvl = 0;
    std::string tmpstr1, tmpstr2 = ucase(str);
    while ((tmpstr1 = Level2String(lvl)) != "") {
      if (tmpstr1 == tmpstr2)
        return lvl;
      lvl++;
    }
    EUDAQ_THROW("Unrecognised level: " + str);
  }

  Status &Status::SetTag(const std::string &name, const std::string &val){
    m_tags[name] = val;
    return *this;
  }

  std::string Status::GetTag(const std::string &name,
                             const std::string &def) const{
    std::map<std::string, std::string>::const_iterator i = m_tags.find(name);
    if (i == m_tags.end())
      return def;
    return i->second;
  }

  void Status::Print(std::ostream &os, size_t offset) const {
    os << std::string(offset, ' ') << "<Status>\n";
    os << std::string(offset + 2, ' ') << "<Level>" << Level2String(m_level) <<"</Level>\n";
    os << std::string(offset + 2, ' ') << "<State>" << m_state <<"</State>\n";
    os << std::string(offset + 2, ' ') << "<Message>" <<m_msg <<"</Message>\n";
    if(!m_tags.empty()){
      os << std::string(offset + 2, ' ') << "<Tags>\n";
      for (auto &tag: m_tags){
	os << std::string(offset+4, ' ') << "<Tag>"<< tag.first << "=" << tag.second << "</Tag>\n";
      }
      os << std::string(offset + 2, ' ') << "</Tags>\n";
    }
    os << std::string(offset, ' ') << "</Status>\n";
  }
}