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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "json.hpp"
#include "utils.hpp"
#include "core/variant_map.hpp"

static std::string lastParsedObjectName;
static std::string sanitizeString(std::string str)
{
  str = utils::replace( str, "\\", "\\\\");
  str = utils::replace( str, "\"", "\\\"");
  str = utils::replace( str, "\b", "\\b");
  str = utils::replace( str, "\f", "\\f");
  str = utils::replace( str, "\n", "\\n");
  str = utils::replace( str, "\r", "\\r");
  str = utils::replace( str, "\t", "\\t");
  
  return std::string( "\"" ) + str + std::string("\"");
}

static std::string join(const StringArray& rlist, const std::string& sep)
{
  std::string res;
  for( StringArray::const_iterator it = rlist.begin(); it != rlist.end(); ++it )
  {
    if(!res.empty())
    {
      res.append( sep );
    }
    res.append( *it );
  }
  return res;
}

/**
 * parse
 */
Variant Json::parse(const std::string &json)
{
  bool success = true;
  return Json::parse(json, success);
}

/**
 * parse
 */
Variant Json::parse(const std::string& json, bool &success )
{
  success = true;
  //Return an empty Variant if the JSON data is either null or empty
  if( !json.empty() )
  {
    int index = 0;
    std::string data = json;
    //We'll start from index 0

    //Parse the first value
    Variant value = Json::parseValue(data, index, success);

    //Return the parsed value
    return value;
  }
  else
  {
    //Return the empty Variant
    return Variant();
  }
}

std::string Json::serialize(const Variant &data, const std::string& tab)
{
        bool success = true;
        return Json::serialize(data, success, tab);
}

std::string Json::serialize(const Variant &data, bool &success, const std::string& tab)
{
  std::string str;
  success = true;

  if( data.isNull() ) // invalid or null?
  {
    return "null";
  }

  switch( data.type() )
  {
    case Variant::List:
    case Variant::NStringArray: // variant is a list?
    {
      StringArray values;
      const VariantList rlist = data.toList();
      for( VariantList::const_iterator it = rlist.begin(); it != rlist.end(); ++it)
      {
        std::string serializedValue = serialize( *it, "" );
        if( serializedValue.empty() )
        {
            success = false;
            break;
        }

        values.push_back( serializedValue );
      }

      str = "[ " + join( values, ", " ) + " ]";
    }
    break;

    case Variant::Map: // variant is a map?
    {
      VariantMap vmap = data.toMap();
      
      if( vmap.empty() )
      {
        str = "{}";
      }
      else
      {
        str = "{ \n";
        StringArray pairs;
        foreach( it, vmap )
        {        
          std::string serializedValue = serialize( it->second, tab + "  ");
          if( serializedValue.empty())
          {
                  //success = false;
            pairs.push_back( tab + sanitizeString( it->first ) + std::string( " : \"nonSerializableValue\"" ) );
            continue;
          }
          pairs.push_back( tab + sanitizeString( it->first ) + " : " + serializedValue );
        }
        str += join(pairs, ",\n");
        std::string rtab( tab );
        rtab.resize( std::max<int>( 0, tab.size() - 2 ) );
        str += std::string( "\n" ) + rtab + "}";
      }
    }
    break;

    case Variant::String:
    case Variant::NByteArray: // a string or a byte array?
    {
      str = sanitizeString( data.toString() );
    }
    break;

    case Variant::Double:
    case Variant::Float: // double?
    {
      // TODO: cheap hack - almost locale independent double formatting
      str = utils::format( 0xff, "\"%f\"", data.toDouble() );
      str = utils::replace(str, ",", ".");
      if( str.find(".") == std::string::npos && str.find("e") == std::string::npos )
      {
         str += ".0";
      }
    }
    break;

    case Variant::NTilePos:
    {
      const TilePos& pos = data.toTilePos();
      str = utils::format( 0xff, "[ %d, %d ]", pos.i(), pos.j() );
    }
    break;

    case Variant::NSize:
    {
      const Size& size = data.toSize();
      str = utils::format( 0xff, "[ %d, %d ]", size.width(), size.height() );
    }
    break;

    case Variant::NPoint:
    {
      const Point& pos = data.toPoint();
      str = utils::format( 0xff, "[ %d, %d ]", pos.x(), pos.y() );
    }
    break;

    case Variant::NPointF:
    {
      PointF pos = data.toPointF();
      // TODO: cheap hack - almost locale independent double formatting
      std::string posX = utils::replace(utils::format( 0xff, "%f", pos.x()), ",", ".");
      std::string posY = utils::replace(utils::format( 0xff, "%f", pos.y()), ",", ".");
      str = utils::format( 0xff, "[ \"%s\", \"%s\" ]", posX.c_str(), posY.c_str() );
    }
    break;

    case Variant::Bool: // boolean value?
    {
      str = data.toBool() ? "true" : "false";
    }
    break;

    case Variant::ULongLong: // large unsigned number?
    {
      str = utils::format( 0xff, "%u", data.toULongLong() );
    }
    break;

    case Variant::Int: // simple int?
    {
      str = utils::format( 0xff, "%d", data.toInt() );
    }
    break;

    case Variant::UInt:
    {
      str = utils::format( 0xff, "%d", data.toInt() );
    }
    break;

    default:
      if ( data.canConvert( Variant::LongLong ) ) // any signed number?
      {
        str = utils::format( 0xff, "%d", data.toLongLong() );
      }
      else if (data.canConvert( Variant::Long ))
      {
        str = utils::format( 0xff, "%d", data.toLongLong() );
      }
      else if (data.canConvert( Variant::String ) ) // can value be converted to string?
      {
        // this will catch Date, DateTime, Url, ...
        str = sanitizeString( data.toString() );
      }
      else
      {
        success = false;
      }
    break;
  }

  return success ? str : std::string();
}

/**
 * parseValue
 */
Variant Json::parseValue(const std::string &json, int &index, bool &success)
{
  //Determine what kind of data we should parse by
  //checking out the upcoming token
  bool done = false;
  while(!done)
  {
    int token = Json::lookAhead(json, index);
    switch( token )
    {
      case JsonTokenString:      return Json::parseString(json, index, success);
      case JsonTokenNumber:      return Json::parseNumber(json, index);
      
      case JsonTokenCommentOpen:
        Json::parseComment(json, index, success); 
      break;
      
      case JsonTokenCurlyOpen: 
      case JsonTokenObjectName: 
          return Json::parseObject(json, index, success);
      
      case JsonTokenSquaredOpen: return Json::parseArray(json, index, success);
      case JsonTokenTrue:  Json::nextToken(json, index); return Variant(true);
      case JsonTokenFalse: Json::nextToken(json, index); return Variant(false);
      case JsonTokenNull:  Json::nextToken(json, index); return Variant();
      case JsonTokenNone:
        done = true;
      break;
    }
  }

  //If there were no tokens, flag the failure and return an empty Variant
  success = false;
  return Variant( std::string( json.c_str() + index ));
}

/**
 * parseObject
 */
Variant Json::parseObject(const std::string &json, int &index, bool &success)
{
  VariantMap rmap;
  //Get rid of the whitespace and increment index
  Json::nextToken(json, index);

  //Loop through all of the key/value pairs of the object
  bool done = false;
  while(!done)
  {
    //Get the upcoming token
    int token = Json::lookAhead(json, index);

    switch( token )
    {
    case JsonTokenNone:
      {
        success = false;
        bool sc;
        std::string errText = Json::parseObjectName( json, index, sc ).toString();
        return Variant( errText );
      }
    break;

    case JsonTokenComma:
      {
        Json::nextToken(json, index);
      }
    break;

    case JsonTokenCurlyClose:
      {
        Json::nextToken(json, index);
        return Variant( rmap );
      }
    break;

    case JsonTokenCommentOpen:
      {
        Json::parseComment(json, index, success);
      }
    break;

    case JsonTokenObjectName:
      {
        std::string name = Json::parseObjectName( json, index, success ).toString();
        lastParsedObjectName = name;

        if(!success)
        {
          return Variant(name);
        }

        name = utils::replace( name, " ", "" );

        index++;
        Variant value = Json::parseValue(json, index, success);

        if(!success)
        {
          return Variant( value.toString() );
        }

        //Assign the value to the key in the map
        rmap[name] = value;
      }
    break;

    default:
      {
        //Parse the key/value pair's name
        std::string name = Json::parseString(json, index, success).toString();

        if(!success)
        {
          return Variant();
        }

        //Get the next token
        token = Json::nextToken(json, index);

        //If the next token is not a colon, flag the failure
        //return an empty Variant
        if(token != JsonTokenColon)
        {
          success = false;
          std::string errText = utils::format( 0xff, "Wrong token colon near \"%s\"", name.c_str() );
          return Variant( errText );
        }

        //Parse the key/value pair's value
        Variant value = Json::parseValue(json, index, success);

        if(!success)
        {
          return Variant();
        }

        //Assign the value to the key in the map
        rmap[name] = value;
      }
    break;
    }
  } //end while(!done)

  //Return the map successfully
  return Variant(rmap);
}

/**
 * parseArray
 */
Variant Json::parseArray(const std::string &json, int &index, bool &success)
{
        VariantList list;

        Json::nextToken(json, index);

        bool done = false;
        while(!done)
        {
                int token = Json::lookAhead(json, index);

                if(token == JsonTokenNone)
                {
                        success = false;
                        return VariantList();
                }
                else if(token == JsonTokenComma)
                {
                        Json::nextToken(json, index);
                }
                else if(token == JsonTokenSquaredClose)
                {
                        Json::nextToken(json, index);
                        break;
                }
                else
                {
                        Variant value = Json::parseValue(json, index, success);

                        if(!success)
                        {
                                return VariantList();
                        }

                        list.push_back(value);
                }
        }

        return Variant(list);
}

/**
 * parse comment
 */
void Json::parseComment(const std::string &json, int &index, bool &success)
{
  success = false;
  index+=2;

  bool complete = false;
  while(!complete)
  {
    if(index == (int)json.size())
    {
      break;
    }

    char c = json[index++];

    if(c == '/' && json[ index - 2 ] == '*' )
    {
      complete = true;
      success = true;;
      break;
    }
  }
}

/**
* parse object name without colons
*/
Variant Json::parseObjectName(const std::string &json, int &index, bool &success, char limiter )
{
  std::string s;

  eatWhitespace(json, index);

  bool complete = false;

  int lastIndex = math::clamp<int>( index + 64, 0, json.size() );
  for( int i=index; i < lastIndex; i++ )
  {    
    if( json[i+1] != limiter )
    {
      s += json[ i ];
    }
    else
    {
      s += json[ i ];
      index = (i + 1);
      complete = true;
      break;
    }
  }

  for( unsigned int i=0; i < s.size(); i++ )
  {
    if( std::string("{}[],").find( s[i] ) != std::string::npos )
    {
      s = json.substr( std::max( 0, index-20 ), lastIndex );
      complete = false;
      break;
    }
  }

  if( !complete )
  {
    success = false;
    std::string advText = json.substr( std::max( 0, index - 60), 120 );
    std::string errText = utils::format( 0xff, "Wrong symbol in object name \"%s\"  at \n %s", s.c_str(), advText.c_str() );
    return Variant( errText );
  }

  return Variant(s);
}

/**
 * parse string with colons
 */
Variant Json::parseString(const std::string &json, int &index, bool &success)
{
        std::string s;
        char c;

        Json::eatWhitespace(json, index);

        c = json[index++];

        bool complete = false;
        while(!complete)
        {
          if(index == (int)json.size())
          {
            break;
          }

          c = json[index++];

          if(c == '\"')
          {
            complete = true;
            break;
          }
          else if(c == '\\')
          {
            if(index == (int)json.size())
            {
              break;
            }

            c = json[index++];

            if( c == '\"' || c == '\\' )
            {
              s += c;
            }
            else if(c == '/')
            {
              s.append("/");
            }
            else if(c == 'b')
            {
              s.append("\b");
            }
            else if(c == 'f')
            {
              s.append("\f");
            }
            else if(c == 'n')
            {
              s.append("\n");
            }
            else if(c == 'r')
            {
              s.append("\r");
            }
            else if(c == 't')
            {
              s.append("\t");
            }
            else if(c == 'u')
            {
//                                 int remainingLength = json.size() - index;
// 
//                                 if(remainingLength >= 4)
//                                 {
//                                         std::string unicodeStr = json.substr(index, 4);
// 
//                                         int symbol = utils::fromHex( unicodeStr.c_str() );
// 
//                                         s.append( symbol );
// 
//                                         index += 4;
//                                 }
//                                 else
//                                 {
//                                         break;
//                                 }
              _CAESARIA_DEBUG_BREAK_IF( true && "yet not work")
            }
          }
          else
          {
            s += c;
          }
        }

        if(!complete)
        {
                success = false;
                return Variant();
        }

        return Variant(s);
}

/**
 * parseNumber
 */
Variant Json::parseNumber(const std::string &json, int &index)
{
        Json::eatWhitespace(json, index);

        int lastIndex = Json::lastIndexOfNumber(json, index);
        int charLength = (lastIndex - index) + 1;
        std::string numberStr;

        numberStr = json.substr( index, charLength );

        index = lastIndex + 1;

        if( numberStr.find('.') != std::string::npos )
        {
          return Variant( utils::toFloat( numberStr.c_str() ) );
        }
        else if( numberStr[0] == '-' ) 
        {
          return Variant( utils::toInt( numberStr.c_str() ) );
        } 
        else 
        {
          return Variant( utils::toUint( numberStr.c_str() ) );
        }
}

/**
 * lastIndexOfNumber
 */
int Json::lastIndexOfNumber(const std::string &json, int index)
{
   int lastIndex;

   for(lastIndex = index; lastIndex < (int)json.size(); lastIndex++)
   {
     if( std::string("0123456789+-.eE").find( json[lastIndex] ) == std::string::npos )
      {
              break;
      }
   }

   return lastIndex -1;
}

/**
 * eatWhitespace
 */
void Json::eatWhitespace(const std::string &json, int &index)
{
        for(; index < (int)json.size(); index++)
        {
          if(std::string(" \t\n\r").find(json[index]) == std::string::npos)
                {
                        break;
                }
        }
}

/**
 * lookAhead
 */
int Json::lookAhead(const std::string &json, int index)
{
  int saveIndex = index;
  return Json::nextToken(json, saveIndex);
}

/**
 * nextToken
 */
int Json::nextToken(const std::string &json, int &index)
{
  //int saveIndex = index;
  Json::eatWhitespace(json, index);

  if(index == (int)json.size())
  {
     return JsonTokenNone;
  }

  char c = json[index];
  index++;
  switch( c )
  {
          case '{': return JsonTokenCurlyOpen;
          case '}': return JsonTokenCurlyClose;
          case '[': return JsonTokenSquaredOpen;
          case ']': return JsonTokenSquaredClose;
          case ',': return JsonTokenComma;
          case '"': return JsonTokenString;
          case '0': case '1': case '2': case '3': case '4':
          case '5': case '6': case '7': case '8': case '9':
          case '-': return JsonTokenNumber;
          case ':': return JsonTokenColon;
  }

  index--;

  int remainingLength = json.size() - index;

  if( remainingLength > 2 )
  {
     if( json[index] == '/' && json[index + 1] == '*' )
     {
       index += 2;
       return JsonTokenCommentOpen;
     }

     if(  json[index] == '*' && json[index + 1] == '/' )
     {
       index += 2;
       return JsonTokenCommentClose;
     }
  }

  //True
  if(remainingLength >= 4)
  {
     if( json[index] == 't' && json[index + 1] == 'r' &&
         json[index + 2] == 'u' && json[index + 3] == 'e')
     {
             index += 4;
             return JsonTokenTrue;
     }
  }

  //False
  if (remainingLength >= 5)
  {
     if( json[index] == 'f' && json[index + 1] == 'a' &&
         json[index + 2] == 'l' && json[index + 3] == 's' &&
         json[index + 4] == 'e')
     {
         index += 5;
         return JsonTokenFalse;
     }
  }

  //Null
  if (remainingLength >= 4)
  {
     if (json[index] == 'n' && json[index + 1] == 'u' &&
             json[index + 2] == 'l' && json[index + 3] == 'l')
     {
             index += 4;
             return JsonTokenNull;
     }
  }

  if( remainingLength > 1 )
  {
    int lastIndex = math::clamp<int>( index + 64, 0, json.size() );
    for( int i=index+1; i < lastIndex ; i++ )
    {
      if( json[i] == ':' )
      {
        return JsonTokenObjectName;
      }
    }
  }

  return JsonTokenNone;
}

std::string Json::lastParsedObject() { return lastParsedObjectName; }
