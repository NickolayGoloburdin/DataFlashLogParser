#include "parser.h"

using namespace DFParser;

std::list<GPS> DFParser::compare_gps_baro(std::list<GPS> gps,
                                          std::list<Baro> baro) {
  std::list<GPS> compared;
  std::list<Baro>::iterator it = baro.begin();
  for (auto i : gps) {
    while (i.timestamp > it->timestamp) {
      it++;
    }

    compared.push_back(GPS{i.lat, i.lon, it->alt, i.timestamp});
  }

  return compared;
}

Parser::Parser(std::set<std::string> types) {
  headers[ftm_type] = {"FTM", ftm_len, "<BB4s16s"};
  for (auto i : types) {
    auto a = i;
    a.resize(4);
    types_.insert(a);
  }
}

Parser::~Parser() {}

void Parser::load_file(const std::string &path) {
  std::ifstream in(path, std::ios::binary | std::ios::in);
  if (in) {
    in.seekg(0, in.end);
    buffer_lenght = in.tellg();
    in.seekg(0, in.beg);
    buffer = new char[buffer_lenght];
    in.read(buffer, buffer_lenght);
    in.close();
  }
  form_struct_and_data();
}
void Parser::form_struct_and_data() {
  int ofs = 0;
  char *buff_ptr;
  while (ofs < buffer_lenght - 3) {
    buff_ptr = buffer + ofs;
    char header[3];
    memcpy(header, buff_ptr, 3);
    buff_ptr += 3;
    char mtype = header[2];
    if (mtype == ftm_type && header[0] == head1 && header[1] == head2) {
      char body[22];
      memcpy(body, buff_ptr, 22);
      buff_ptr += 22;
      auto [msgtype, msglen, msgname, mav_pystruct] =
          pystruct::unpack(PY_STRING("<BB4s16s"), body);
      if (static_cast<char>(msgtype) != mtype) {
        int len = static_cast<int>(msglen);
        std::string name{msgname};
        std::string struct_ = "<";

        if (msglen == 0) {
          ofs += ftm_len + 3;
          continue;
        }

        headers[msgtype] = {name, msglen, struct_};
      }
      ofs += ftm_len + 3;

    } else {

      if (headers.find(mtype) == headers.end() || header[0] != head1 ||
          header[1] != head2) {
        ofs++;
        continue;
      }

      if (types_.count(headers[mtype].name_)) {
        headers[mtype].data.push_back(buff_ptr);
      }
      ofs += headers[mtype].len;
    }
  }
}
std::list<GPS> Parser::get_gps() {
  std::list<GPS> gps;
  for (auto i : headers[116].data) {
    char body[48];
    memcpy(body, i, 48);
    auto [TimeUS, I, Status, GMS, GWk, NSats, HDop, Lat, Lng, Alt, Spd, GCrs,
          VZ, Yaw, U] = pystruct::unpack(PY_STRING("<QBBIHBhiiiffffB"), body);
    gps.push_back(GPS{float(Lat) / 10000000, float(Lng) / 10000000, float(Alt),
                      long(TimeUS)});
  }
  return gps;
}
std::list<Baro> Parser::get_baro() {
  std::list<Baro> baro;
  for (auto i : headers[96].data) {
    char body[36];
    memcpy(body, i, 36);
    auto [TimeUS, I, Alt, Press, Temp, CRt, SMS, Offset, GndTemp, Health] =
        pystruct::unpack(PY_STRING("<QBffcfIffB"), body);
    baro.push_back(Baro{float(Alt), long(TimeUS)});
  }
  return baro;
}
