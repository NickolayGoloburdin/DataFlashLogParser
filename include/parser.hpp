#include "cppystruct.h"
#include "cppystruct/unpack.h"
#include <fstream>
#include <list>
#include <map>
#include <set>
#include <string>
namespace DFParser {

struct GPS {
  float lat;
  float lon;
  float alt;
  long timestamp;
};

struct Baro {
  float alt;
  long timestamp;
};

class Parser {
public:
  Parser(std::set<std::string> types);

  struct Message {
    std::string name_;
    int len = 0;
    std::string struct_decode;
    std::list<char *> data;
  };

  ~Parser();
  void load_file(const std::string &path);
  void form_struct_and_data();
  std::list<GPS> get_gps();
  std::list<Baro> get_baro();

private:
  std::set<std::string> types_;
  char ftm_type = 0x80;
  char head2 = -107;
  char *buffer;
  int buffer_lenght;
  char head1 = -93;
  char gps_type = 0x74;
  int ftm_len = 86;
  char a[10] = "<BB4s16s";
  std::map<char, Message> headers;
};
} // namespace DFParser