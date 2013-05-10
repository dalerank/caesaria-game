// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#include "oc3_json.hpp"
#include "oc3_stringhelper.hpp"
#include <iostream>

static std::string sanitizeString(std::string str)
{
  str = StringHelper::replace( str, "\\", "\\\\");
  str = StringHelper::replace( str, "\"", "\\\"");
  str = StringHelper::replace( str, "\b", "\\b");
  str = StringHelper::replace( str, "\f", "\\f");
  str = StringHelper::replace( str, "\n", "\\n");
  str = StringHelper::replace( str, "\r", "\\r");
  str = StringHelper::replace( str, "\t", "\\t");
  
  return std::string( "\"" ) + str + std::string("\"");
}

static std::string join(const StringArray& rlist, const std::string& sep)
{
  std::string res;
  for( StringArray::const_iterator it = rlist.begin();
       it != rlist.end(); it++ )
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
Variant Json::parse(const std::string& json, bool &success)
{
        success = true;

        //Return an empty Variant if the JSON data is either null or empty
        if( !json.empty() )
        {
                std::string data = json;
                //We'll start from index 0
                int index = 0;

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

  if( !data.isValid() ) // invalid or null?
  {
    str = "null";
  }

  else if( (data.type() == Variant::List) || (data.type() == Variant::NStringArray) ) // variant is a list?
  {
    StringArray values;
    const VariantList rlist = data.toList();
    for( VariantList::const_iterator it = rlist.begin(); it != rlist.end(); it++)
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
// 	else if(data.type() == Variant::Hash) // variant is a hash?
// 	{
// 		const VariantHash vhash = data.toHash();
// 		QHashIterator<std::string, Variant> it( vhash );
// 		str = "{ ";
// 		QList<QByteArray> pairs;
// 
// 		while(it.hasNext())
// 		{
// 			it.next();
//               QByteArray serializedValue = serialize(it.value(), "");
// 
// 			if(serializedValue.isNull())
// 			{
// 				success = false;
// 				break;
// 			}
// 
//               pairs << tab.toAscii() + sanitizeString(it.key()).toUtf8() + " : " + serializedValue;
// 		}
// 
// 		str += join(pairs, ", ");
// 		str += " }";
// 	}
    else if(data.type() == Variant::Map) // variant is a map?
    {
      const VariantMap vmap = data.toMap();
      std::map<std::string, Variant>::const_iterator it = vmap.begin();
      str = "{ \n";
      StringArray pairs;
      while( it != vmap.end() )
      {
              it++;

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
    else if((data.type() == Variant::String) || (data.type() == Variant::NByteArray)) // a string or a byte array?
    {
            str = sanitizeString( data.toString() );
    }
    else if(data.type() == Variant::Double) // double?
    {
      str = StringHelper::format( 0xff, "%f", data.toDouble() );
      if( str.find(".") == std::string::npos && str.find("e") == std::string::npos )
      {
         str += ".0";
      }
    }
    else if (data.type() == Variant::Bool) // boolean value?
    {
            str = data.toBool() ? "true" : "false";
    }
    else if (data.type() == Variant::ULongLong) // large unsigned number?
    {
      str = StringHelper::format( 0xff, "%u", data.toULongLong() );
    }
    else if ( data.canConvert( Variant::LongLong ) ) // any signed number?
    {
            str = StringHelper::format( 0xff, "%d", data.toLongLong() );
    }
    else if (data.canConvert( Variant::NLong ))
    {
            str = StringHelper::format( 0xff, "%d", data.toLongLong() );
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

    if (success)
    {
            return str;
    }
    else
    {
      return std::string();
    }
}

/**
 * parseValue
 */
Variant Json::parseValue(const std::string &json, int &index, bool &success)
{
        //Determine what kind of data we should parse by
        //checking out the upcoming token
        switch(Json::lookAhead(json, index))
        {
                case JsonTokenString:
                        return Json::parseString(json, index, success);
                case JsonTokenNumber:
                        return Json::parseNumber(json, index);
                case JsonTokenCurlyOpen:
                        return Json::parseObject(json, index, success);
                case JsonTokenSquaredOpen:
                        return Json::parseArray(json, index, success);
                case JsonTokenTrue:
                        Json::nextToken(json, index);
                        return Variant(true);
                case JsonTokenFalse:
                        Json::nextToken(json, index);
                        return Variant(false);
                case JsonTokenNull:
                        Json::nextToken(json, index);
                        return Variant();
                case JsonTokenNone:
                        break;
        }

        //If there were no tokens, flag the failure and return an empty Variant
        success = false;
        return Variant();
}

/**
 * parseObject
 */
Variant Json::parseObject(const std::string &json, int &index, bool &success)
{
        VariantMap rmap;
        int token;

        //Get rid of the whitespace and increment index
        Json::nextToken(json, index);

        //Loop through all of the key/value pairs of the object
        bool done = false;
        while(!done)
        {
                //Get the upcoming token
                token = Json::lookAhead(json, index);

                if(token == JsonTokenNone)
                {
                         success = false;
                         return VariantMap();
                }
                else if(token == JsonTokenComma)
                {
                        Json::nextToken(json, index);
                }
                else if(token == JsonTokenCurlyClose)
                {
                        Json::nextToken(json, index);
                        return rmap;
                }
                else
                {
                        //Parse the key/value pair's name
                        std::string name = Json::parseString(json, index, success).toString();

                        if(!success)
                        {
                                return VariantMap();
                        }

                        //Get the next token
                        token = Json::nextToken(json, index);

                        //If the next token is not a colon, flag the failure
                        //return an empty Variant
                        if(token != JsonTokenColon)
                        {
                                success = false;
                                return Variant(VariantMap());
                        }

                        //Parse the key/value pair's value
                        Variant value = Json::parseValue(json, index, success);

                        if(!success)
                        {
                                return VariantMap();
                        }

                        //Assign the value to the key in the map
                        rmap[name] = value;
                }
        }

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
 * parseString
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
                if(index == json.size())
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
                        if(index == json.size())
                        {
                                break;
                        }

                        c = json[index++];

                        if(c == '\"')
                        {
                                s.append("\"");
                        }
                        else if(c == '\\')
                        {
                          s.append("\\");
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
//                                         int symbol = StringHelper::fromHex( unicodeStr.c_str() );
// 
//                                         s.append( symbol );
// 
//                                         index += 4;
//                                 }
//                                 else
//                                 {
//                                         break;
//                                 }
                          _OC3_DEBUG_BREAK_IF( true && "yet not work")
                        }
                }
                else
                {
                        s.append( &c );
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
          return Variant( StringHelper::toFloat( numberStr.c_str() ) );
        }
        else if( numberStr[0] == '-' ) 
        {
          return Variant( StringHelper::toInt( numberStr.c_str() ) );
        } 
        else 
        {
          return Variant( StringHelper::toUint( numberStr.c_str() ) );
        }
}

/**
 * lastIndexOfNumber
 */
int Json::lastIndexOfNumber(const std::string &json, int index)
{
   int lastIndex;

   for(lastIndex = index; lastIndex < json.size(); lastIndex++)
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
        for(; index < json.size(); index++)
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
        Json::eatWhitespace(json, index);

        if(index == json.size())
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

        //True
        if(remainingLength >= 4)
        {
                if (json[index] == 't' && json[index + 1] == 'r' &&
                        json[index + 2] == 'u' && json[index + 3] == 'e')
                {
                        index += 4;
                        return JsonTokenTrue;
                }
        }

        //False
        if (remainingLength >= 5)
        {
                if (json[index] == 'f' && json[index + 1] == 'a' &&
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

        return JsonTokenNone;
}
