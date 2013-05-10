#ifndef __OPENCAESAR3_SAVEADAPTER_H_INCLUDED__
#define __OPENCAESAR3_SAVEADAPTER_H_INCLUDED__

#include "oc3_variant.hpp"

class SaveAdapter
{
public:
  static VariantMap load( const std::string& fileName );

  static bool save( const VariantMap& oprions );
private:
  SaveAdapter();
};

#endif //__OPENCAESAR3_SAVEADAPTER_H_INCLUDED__