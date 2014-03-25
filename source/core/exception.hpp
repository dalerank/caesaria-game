// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com


#ifndef _CAESARIA_EXCEPTION_HPp_INCLUDE_
#define _CAESARIA_EXCEPTION_HPp_INCLUDE_

#include <sstream>

#define THROW(x)   { std::stringstream _exception_text; _exception_text << x; throw Exception(_exception_text.str()); }

class Exception
{
public:
   Exception(const std::string &aDescription) : _desc( aDescription )
   {
   }

   ~Exception() {}

   const std::string& getDescription() const { return _desc; }

private:
   std::string _desc;
};

#endif //_CAESARIA_EXCEPTION_HPp_INCLUDE_
