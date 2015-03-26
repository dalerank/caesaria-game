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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include <string>
#include <map>

namespace gui
{

#define BUF_SIZE 1024

// <цифра> ::= '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'
// <число> ::= <цифра> { <цифра> } [ '.' <цифра> { <цифра> } ]
//
// <выражение> ::= <слагаемое> [ ( '+' | '-' ) <слагаемое> ]
// <слагаемое> ::= <множитель> [ ( '*' | '/' ) <множитель> ]
// <множитель> ::= ( <число> | '(' <выражение> ')' ) [ '^' <множитель> ]

class Widget;

class WidgetCalc
{
public:
  WidgetCalc(Widget &parent );
  double eval( const std::string& str );

private:
  double _number( const std::string& str, unsigned *idx);
  double _expr(const std::string& str, unsigned *idx);
  double _term(const std::string &str, unsigned *idx);
  double _factor(const std::string &str, unsigned *idx);

  std::map< std::string, int > _alias;
  Widget& _widget;
};

}//end namespace gui
