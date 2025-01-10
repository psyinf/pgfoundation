#pragma once
#include <fstream>

#include <pgf/caching/URI.hpp>

namespace pg::foundation {
class DataProvider
{
public:
    DataProvider(const URI& uri)
      : _uri(uri)
    {
    }

    virtual void open() = 0;
    virtual void close() = 0;

    virtual std::istream& asStream() = 0;
    virtual void          asBuffer(std::vector<char>& buffer, size_t max_ch) = 0;
    virtual char          readNext() = 0;

    virtual ~DataProvider() = default;

    const virtual URI& getUri() const { return _uri; }

private:
    URI _uri;
};

class FileDataProvider : public DataProvider
{
    std::ifstream _file;

public:
    using DataProvider::DataProvider;

    FileDataProvider(const URI& uri, bool autoOpen = false)
      : DataProvider(uri)
    {
        if (autoOpen) { open(); }
    }

    virtual std::istream& asStream() override
    {
        if (!_file.is_open()) { throw std::runtime_error("File not open"); }
        return _file;
    }

    virtual void asBuffer(std::vector<char>& buffer, size_t max_ch) override
    {
        throw std::runtime_error("Not implemented");
        // TODO: get file size and resize buffer
        // buffer.resize(file_size);
        //_file.read(buffer.data(), max_ch); }
    }

    virtual char readNext() override
    {
        if (!_file.is_open()) { throw std::runtime_error("File not open"); }
        return static_cast<char>(_file.get());
    }

    virtual void open() override { _file = std::ifstream{getUri().uri, std::ios_base::binary}; }

    virtual void close() override { _file.close(); }
};

} // namespace pg::foundation