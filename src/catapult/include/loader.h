#ifndef __LOADER_H__
#define __LOADER_H__

#include <filesystem>
#include <istream>
#include <list>
#include <vector>

#include "command.h"

namespace catapult {

using Record = std::vector<std::string>;

//! @brief Base Class for loaders. Loads take data from somewhere and returns an istream of it;
class Loader {
  public:
    virtual ~Loader() = default;
    [[nodiscard]] virtual std::istream &load () const = 0;
};

class Parser {
  public:
    virtual ~Parser() = default;
    virtual std::vector<Record> parse (std::istream &data) const = 0;
};

template<class ReturnType>
class Interpreter {
  public:
    virtual ~Interpreter() = default;
    virtual ReturnType interpret (const Record &record) const = 0;
};

//! @brief Takes data from a file and returns an istream of it
class FileLoader: public Loader {
  public:
    FileLoader (const std::filesystem::path &path): _path { path } {};
    [[nodiscard]] std::istream &load () const override;

  private:
    std::filesystem::path _path;
};

//! @brief a very base CSVParser - no escaping this one!
class CSVParser: public Parser {
  public:
    /*! @brief parses csv data;
     *  @param data the csv data as an std::string
     *  @return a vector of vector of fields as std::string;
     */
    std::vector<Record> parse (std::istream &data) const override;
};

}  // namespace catapult

#endif  // __LOADER_H__
