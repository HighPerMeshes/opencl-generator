#ifndef PRINT_WITH_DELIMITER_HPP
#define PRINT_WITH_DELIMITER_HPP

#include <util/for_each_interleave.hpp>
#include <util/print.hpp>

#include <string_view>

namespace ct
{

template <typename Stream, typename Iterator>
void print_with_delimiter(Stream &stream, Iterator &&begin, Iterator &&end, std::string_view delimiter = {", "})
{
  auto printLambda = [&stream](auto &&entry) { print(stream, std::forward<decltype(entry)>(entry)); };
  auto delim = [printLambda, delimiter] { printLambda(delimiter); };
  for_each_interleave(std::forward<Iterator>(begin), std::forward<Iterator>(end), printLambda, delim);
}

template <typename Stream, typename Collection>
void print_with_delimiter(Stream &stream, const Collection &collection, std::string_view delimiter = {", "})
{
  print_with_delimiter(stream, std::begin(collection), std::end(collection), delimiter);
}

} // namespace ct

#endif /* PRINT_WITH_DELIMITER_HPP */
