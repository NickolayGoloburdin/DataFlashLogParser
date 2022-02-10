#include "parser.h"

using namespace DFParser;
int main() {

  DFParser::Parser parser({"GPS", "XKQ", "BARO"});
  parser.load_file("/home/nickolay/work/log_parser/1.bin");
  std::list<DFParser::GPS> gps = parser.get_gps();
  std::list<DFParser::Baro> baro = parser.get_baro();
  auto a = DFParser::compare_gps_baro(gps, baro);
  int c = 0;
}