#include <cxxabi.h>
#include "exception.h"
namespace AccelCompEng
{



Exception::Exception(const char* what, const char* detail, int line) noexcept:
   _what(what),
   _detail(detail),
   _line(line)
{}



Exception::~Exception() noexcept
{
   if (_demangle)
   {
      free(_demangle);
   }
}



int Exception::line() const noexcept
{
   return _line;
}



const char* Exception::what() const noexcept
{
   if (!_demangle)
   {
      int stat;
      _demangle = abi::__cxa_demangle(_what,0,0,&stat);
   }
   return _demangle;
}



const char* Exception::detail() const noexcept
{
   return _detail;
}



}
