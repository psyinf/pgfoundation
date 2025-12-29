#pragma once
#include <fstream>

#include <pgf/caching/URI.hpp>
#include <span>

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

    virtual std::istream&     asStream() = 0;
    virtual std::vector<char> asBuffer() = 0;
    virtual std::span<char>   asSpan() = 0;
    virtual void              toBuffer(std::vector<char>& buffer, size_t max_ch) = 0;
    virtual char              readNext() = 0;

    virtual ~DataProvider() = default;

    virtual size_t size() = 0;

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

    virtual void toBuffer(std::vector<char>& buffer, size_t max_ch) override
    {
        if (!_file.is_open()) { throw std::runtime_error("File not open"); }
        // get max of max_ch and file size
        max_ch = std::min(max_ch, static_cast<size_t>(_file.seekg(0, std::ios_base::end).tellg()));
        _file.seekg(0, std::ios_base::beg);

        buffer.resize(max_ch);
        _file.read(buffer.data(), max_ch);
    }

    virtual std::vector<char> asBuffer() override
    {
        // rely on copy elision
        if (!_file.is_open()) { throw std::runtime_error("File not open"); }
        _file.seekg(0, std::ios_base::end);
        auto size = _file.tellg();
        _file.seekg(0, std::ios_base::beg);

        std::vector<char> buffer(size);
        _file.read(buffer.data(), size);
        return buffer;
    }

    virtual std::span<char> asSpan() override
    {
        if (!_file.is_open()) { throw std::runtime_error("File not open"); }
        auto file_size = size();
        thread_local std::vector<char> buffer;
        buffer.resize(file_size);

        _file.read(buffer.data(), file_size);
        return std::span<char>(buffer);
    }

    virtual char readNext() override
    {
        if (!_file.is_open()) { throw std::runtime_error("File not open"); }
        return static_cast<char>(_file.get());
    }

    virtual void open() override { _file = std::ifstream{getUri().uri, std::ios_base::binary}; }

    virtual void close() override { _file.close(); }

    virtual size_t size() override
    {
        if (!_file.is_open()) { throw std::runtime_error("File not open"); }
        _file.seekg(0, std::ios_base::end);
        auto size = _file.tellg();
        _file.seekg(0, std::ios_base::beg);
        return size;
    }

    virtual ~FileDataProvider() override { close(); }
};

} // namespace pg::foundation