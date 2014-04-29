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

#include "helper.hpp"

#include "core/logger.hpp"
#include "core/saveadapter.hpp"

using namespace constants;
using namespace gfx;

class WalkerHelper::Impl : public EnumsHelper<walker::Type>
{
public:
  typedef std::map< walker::Type, std::string > PrettyNames;
  PrettyNames prettyTypenames;
  VariantMap options;

  void append( walker::Type type, const std::string& typeName )
  {
    EnumsHelper<walker::Type>::append( type, typeName );
    prettyTypenames[ type ] = "##wt_" + typeName + "##";
  }

  Impl() : EnumsHelper<walker::Type>( walker::unknown )
  {
      append( walker::all,        "unknown" );

#define __REG_WTYPE(a) append( walker::a, CAESARIA_STR_A(a));
    __REG_WTYPE( unknown )
    __REG_WTYPE( immigrant )
    __REG_WTYPE( citizen )
    __REG_WTYPE( emmigrant )
    __REG_WTYPE( soldier )
    __REG_WTYPE(  cartPusher )
    __REG_WTYPE( marketLady )
    __REG_WTYPE( marketKid )
    __REG_WTYPE( serviceman )
    __REG_WTYPE( trainee )
    __REG_WTYPE( recruter )
    __REG_WTYPE( prefect )
    __REG_WTYPE( priest );
    __REG_WTYPE( taxCollector);
    __REG_WTYPE( merchant )
    __REG_WTYPE( engineer )
    __REG_WTYPE( doctor )
    __REG_WTYPE( sheep )
    __REG_WTYPE( bathlady )
    __REG_WTYPE( actor )
    __REG_WTYPE( gladiator )
    __REG_WTYPE( barber )
    __REG_WTYPE( surgeon )
    __REG_WTYPE( lionTamer )
    __REG_WTYPE( fishingBoat )
    __REG_WTYPE( protestor )
    __REG_WTYPE( legionary )
    __REG_WTYPE( corpse )
    __REG_WTYPE( lion )
    __REG_WTYPE( marketBuyer )
    __REG_WTYPE( britonSoldier )
    __REG_WTYPE( fishPlace )
    __REG_WTYPE( seaMerchant )
    __REG_WTYPE( scholar )
    __REG_WTYPE( teacher )
    __REG_WTYPE( librarian )
    __REG_WTYPE( etruscanSoldier )
    __REG_WTYPE( charioteer )
    __REG_WTYPE( etruscanArcher )
    __REG_WTYPE( spear )
    __REG_WTYPE( romeGuard )
    __REG_WTYPE( bow_arrow )
    __REG_WTYPE( romeHorseman )
    __REG_WTYPE( romeSpearman )
    __REG_WTYPE( catapult )
  }
};

VariantMap WalkerHelper::getOptions(const walker::Type type )
{
  std::string tname = getTypename( type );
  VariantMap::iterator mapIt = instance()._d->options.find( tname );
  if( mapIt == instance()._d->options.end())
  {
    Logger::warning("WalkerInfo: Unknown walker info for type %d", type );
    return VariantMap();
  }

  return mapIt->second.toMap();
}

void WalkerHelper::initialize( const vfs::Path& filename )
{
  _d->options = SaveAdapter::load( filename );
}

WalkerHelper& WalkerHelper::instance()
{
  static WalkerHelper inst;
  return inst;
}

std::string WalkerHelper::getTypename( walker::Type type )
{
  std::string name = instance()._d->findName( type );

  if( name.empty() )
  {
    Logger::warning( "WalkerHelper: can't find walker typeName for %d", type );
    //_CAESARIA_DEBUG_BREAK_IF( "Can't find walker typeName by WalkerType" );
  }

  return name;
}

walker::Type WalkerHelper::getType(const std::string &name)
{
  walker::Type type = instance()._d->findType( name );

  if( type == instance()._d->getInvalid() )
  {
    Logger::warning( "WalkerHelper: can't find walker type for %s", name.c_str() );
    //_CAESARIA_DEBUG_BREAK_IF( "Can't find walker type by typeName" );
  }

  return type;
}

std::string WalkerHelper::getPrettyTypeName(walker::Type type)
{
  Impl::PrettyNames::iterator it = instance()._d->prettyTypenames.find( type );
  return it != instance()._d->prettyTypenames.end() ? it->second : "";
}

Picture WalkerHelper::getBigPicture(walker::Type type)
{
  int index = -1;
  switch( type )
  {
  case walker::immigrant: index=9; break;
  case walker::emmigrant: index=4; break;
  case walker::doctor: index = 2; break;
  case walker::cartPusher: index=51; break;
  case walker::marketLady: index=12; break;
  case walker::marketKid: index=38; break;
  case walker::merchant: index=25; break;
  case walker::prefect: index=19; break;
  case walker::engineer: index=7; break;
  case walker::taxCollector: index=6; break;
  case walker::sheep: index = 54; break;
  case walker::recruter: index=13; break;
  case walker::lionTamer: index=11; break;
  default: index=8; break;
  break;
  }

  return index >= 0 ? Picture::load( "bigpeople", index ) : Picture::getInvalid();
}

WalkerHelper::~WalkerHelper(){}
WalkerHelper::WalkerHelper() : _d( new Impl ){}
