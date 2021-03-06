// Copyright (C) 2015 Kenenth Benzie
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef REDMINE_COMMAND_LINE_H
#define REDMINE_COMMAND_LINE_H

#include <string>

namespace redmine {
namespace cl {
/// @brief Command line argument wrapper.
///
/// The redmine::cl::args object wraps the command line arguments passed to a
/// C++ ::main function. No copies of the values in @a argv are made and access
/// to all argument values remain unmodified.
class args {
 public:
  /// @brief Construct from command line arguments.
  ///
  /// @param argc Argument count.
  /// @param argv Argument values.
  args(int argc, char **argv);

  /// @brief Copy constructor.
  ///
  /// @param other Object to copy from.
  args(const args &other);

  /// @brief Copy assignment operator.
  ///
  /// @param other Object to copy assign from.
  ///
  /// @return Reference to this object.
  args &operator=(const args &other);

  /// @brief Consume first argument.
  ///
  /// @return Reference to this object.
  args &operator++();

  /// @brief Consume first argument.
  ///
  /// @return Reference to this object.
  args &operator++(int);

  /// @brief Consume first count arguments.
  ///
  /// @param count Number of arguments to consume.
  ///
  /// @return Reference to this object.
  args &operator+=(const int count);

  /// @brief Access argument.
  ///
  /// @param index Argument index.
  ///
  /// @return Argument value.
  const char *operator[](const int index);

  /// @brief Return the first argument.
  ///
  /// @return Argument value.
  const char *front();

  /// @brief Return the last argument.
  ///
  /// @return Argument value.
  const char *back();

  /// @brief Return iterator to beginning.
  ///
  /// @return The first element iterator.
  const char *const *begin();

  /// @brief Return iterator to end.
  ///
  /// @return One past the last element iterator.
  const char *const *end();

  /// @brief Return argument count.
  ///
  /// @return Number of arguments.
  int count();

 private:
  /// @brief Argument count.
  int argc;
  /// @brief Argument value array.
  char **argv;
};

/// @brief Prompt the user for input, no answer checking.
///
/// @param question Question to ask.
///
/// @return Answer string.
std::string get_answer_string(const std::string &question);

/// @brief Prompt the user for input, must be a valid unsigned int.
///
/// @param question Question to ask.
///
/// @return Number answer.
uint32_t get_answer_number(const std::string &question);

/// @brief Prompt the user for input, must be "true" or "false.
///
/// @param question Question to ask.
///
/// @return Boolean answer.
bool get_answer_bool(const std::string &question);
}  // cl
}  // redmine

#endif  // REDMINE_COMMAND_LINE_H
