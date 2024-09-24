#pragma once

#include <exception>
#include <string>
#include <utility>

namespace jGraph::Exception
{

class JGraphIOException : public std::exception
{
  public:
    explicit JGraphIOException(std::string message)
        : errorMessage(std::move(message))
    {
    }

    [[nodiscard]] const char *what() const noexcept override
    {
        return errorMessage.c_str();
    }

  private:
    std::string errorMessage;
};

namespace internals
{

class DimacsParsingException : public JGraphIOException
{
  public:
    DimacsParsingException(const std::string &details)
        : JGraphIOException("Error while parsing DIMACS file : " + details)
    {
    }
};

} // namespace internals

} // namespace jGraph::Exception
