#include "parser.hpp"

using namespace DFParser;
int main() {

  Parser parser({"GPS", "XKQ", "BARO"});
  parser.load_file("/home/nickolay/work/log_parser/1.bin");
  auto gps = parser.get_gps();
  auto baro = parser.get_baro();
  int a = 0;
}