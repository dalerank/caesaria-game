#include "oc3_saveadapter.hpp"
#include "oc3_scopedptr.hpp"
#include "oc3_json.h"

#include <fstream>

VariantMap SaveAdapter::load( const std::string& fileName )
{
  std::fstream f(fileName.c_str(), std::ios::in | std::ios::binary);

  f.seekg( 0, std::ios::end);
  std::ios::pos_type lastPos = f.tellp();

  f.seekg( 0, std::ios::beg );
  ScopedPtr< char > data( new char[lastPos] );

  f.read( data.data(), lastPos );

  f.close();

  bool jsonParsingOk;
  VariantMap ret = Json::parse( data.data(), jsonParsingOk ).toMap();
  if( jsonParsingOk )
    return ret;

  return VariantMap();
}

SaveAdapter::SaveAdapter()
{

}

bool SaveAdapter::save( const VariantMap& oprions )
{
  return false;
}