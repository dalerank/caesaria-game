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

#include "rect_calc.hpp"
#include "widget.hpp"
#include "core/logger.hpp"

namespace gui
{

WidgetCalc::WidgetCalc(Widget& widget, const VariantMap& vars) : _widget( widget )
{
  _alias[ "w" ] = widget.width();
  _alias[ "h" ] = widget.height();
  _alias[ "pw" ] = widget.parent()->width();
  _alias[ "ph" ] = widget.parent()->height();

  foreach( it, vars )
    _alias[ it->first ] = it->second.toInt();
}

double WidgetCalc::eval(const std::string& str)
{
  unsigned i = 0;
  return _expr(str, &i);
}

double WidgetCalc::_number(const std::string &str, unsigned *idx)
{
  double result = 0.0;
  double div = 10.0;
  int sign = 1;

  if(str[*idx] == '-')
  {
    sign = -1;
    ++*idx;
  }

  if( isalpha( str[*idx] ) )
  {
    std::string name;
    do
    {
      name += str[*idx];
      ++*idx;
    }
    while( isalnum( str[*idx] ) );

    std::map< std::string, int >::iterator it = _alias.find( name );
    if( it == _alias.end() )
    {
      Logger::warning( "!!! WARNING: WidgetCalc cant find alias for " + name );
    }
    else
      result = _alias[ name ];
  }
  else
  {
    while (str[*idx] >= '0' && str[*idx] <= '9')
    {
      result = result * 10.0 + (str[*idx] - '0');
      ++*idx;
    }

    if (str[*idx] == '.')
    {
      ++*idx;
      while (str[*idx] >= '0' && str[*idx] <= '9')
      {
        result = result + (str[*idx] - '0') / div;
        div *= 10.0;
        ++*idx;
      }
    }
  }

  return sign * result;
}

double WidgetCalc::_expr(const std::string& str, unsigned *idx)
{
  double result = _term(str, idx);

  while (str[*idx] == '+' || str[*idx] == '-')
  {
    switch (str[*idx])
    {
    case '+':
      ++*idx;
      result += _term(str, idx);
    break;

    case '-':
      ++*idx;
      result -= _term(str, idx);
    break;
    }
  }

  return result;
}

double WidgetCalc::_term(const std::string& str, unsigned *idx)
{
  double result = _factor(str, idx);
  double div;

  while (str[*idx] == '*' || str[*idx] == '/')
  {
    switch (str[*idx])
    {
    case '*':
      ++*idx;

      result *= _factor(str, idx);

      break;
    case '/':
      ++*idx;

      div = _factor(str, idx);

      if (div != 0.0)
        {
          result /= div;
        }
      else
      {
        Logger::warning( "!!! WARNING: Division by zero!");
        return 0;
      }

      break;
    }
  }

  return result;
}

double WidgetCalc::_factor(const std::string& str, unsigned *idx)
{
  double result;
  int sign = 1;

  if (str[*idx] == '-')
  {
    sign = -1;

    ++*idx;
  }

  if (str[*idx] == '(')
  {
    ++*idx;

    result = _expr(str, idx);

    if (str[*idx] != ')')
    {
      Logger::warning( "!!! WARNING: Brackets unbalanced!");
      return 0;
    }

    ++*idx;
  }
  else
    result = _number(str, idx);

  if (str[*idx] == '^')
  {
    ++*idx;

    result = pow(result, _factor(str, idx));
  }

  return sign * result;
}

}//end namespace gui
