#include "loader.h"
#include "exception.h"

#include <vector>
#include <fstream>
#include <istream>

namespace catapult {

static Record parseLine(const std::string& line) {
  // FIXME: it would be nice to continue to consume the stream here.
  auto sStream = std::stringstream { line };
  std::vector<std::string> fields;
  for(std::string field; std::getline(sStream, field, ',');) {
    fields.push_back(field);
  }
  return fields;
}

std::vector<Record> CSVParser::parse(std::istream& dataStream) const {
  std::vector<Record> records;
  for(std::string line; std::getline(dataStream, line);) {
    records.push_back(parseLine(line));
  }
  return records;
}

std::istream &FileLoader::load () const {
  return *new std::ifstream { _path };
}

}  // namespace catapult
