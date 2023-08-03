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

template<class Receiever>
class Interpreter {
  public:
    virtual ~Interpreter() = default;
    virtual CommandList<Receiever> interpret (const Record &record) const = 0;
};

/*! @brief combines a Loader, Parser and Interpreter to produce a list of commands
 * from a data source */
template<class Receiver>
class Importer {
  public:
    Importer (Loader *loader, Parser *parser, Interpreter<Receiver> *interpreter):
      _loader ( loader ),
      _parser ( parser ),
      _interpreter ( interpreter ) {};

    Importer (const Importer&) = delete;
    Importer& operator=(Importer&) = delete;
    virtual ~Importer() {};

    [[nodiscard]] CommandList<Receiver> &import () const;

  private:
    std::unique_ptr<Loader> _loader;
    std::unique_ptr<Parser> _parser;
    std::unique_ptr<Interpreter<Receiver>> _interpreter;
};

template<class Receiver>
[[nodiscard]] CommandList<Receiver> &Importer<Receiver>::import () const {
  auto records = _parser->parse (_loader->load ());
  auto &commandList = *new CommandList<Receiver> ();
  for (const auto &record: records) {
    auto recordCommandList = _interpreter->interpret (record);
    commandList.splice(commandList.end(), recordCommandList);
  }
  return commandList;
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
