/*
** EPITECH PROJECT, 2026
** ERRORS
** File description:
** arcade
*/

#ifndef ERRORS
  #define ERRORS
  #include <exception>
  #include <string>

namespace Arcade {
class ARCError : public std::exception {
public:
  ARCError(std::string const &message) : _message(message) {}
  const char *what() const noexcept override { return _message.c_str(); }

private:
  std::string _message;
};
}

#endif /* ERRORS */
