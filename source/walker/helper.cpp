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
    append( walker::unknown,    "unknown" );
    append( walker::immigrant,  "immigrant"    );
    append( walker::citizen,    "citizen" );
    append( walker::emmigrant,  "emmigrant" );
    append( walker::soldier,    "soldier" );
    append( walker::cartPusher, "cartPusher" );
    append( walker::marketLady, "marketLady" );
    append( walker::marketKid,  "marketKid" );
    append( walker::serviceman, "serviceman" );
    append( walker::trainee,    "trainee" );
    append( walker::recruter,   "recruter" );
    append( walker::prefect,    "prefect" );
    append( walker::priest,     "priest" );
    append( walker::taxCollector,"taxCollector" );
    append( walker::merchant,   "merchant");
    append( walker::engineer,   "engineer" );
    append( walker::doctor,     "doctor" );
    append( walker::sheep,      "sheep" );
    append( walker::bathlady,   "bathlady" );
    append( walker::actor,      "actor" );
    append( walker::gladiator,  "gladiator" );
    append( walker::barber,     "barber" );
    append( walker::surgeon,    "surgeon" );
    append( walker::lionTamer,  "lionTamer" );
    append( walker::fishingBoat,"fishingBoat" );
    append( walker::protestor,  "protestor" );
    append( walker::legionary,  "legionary" );
    append( walker::corpse,     "corpse" );
    append( walker::lion,       "lion" );
    append( walker::marketBuyer,"markerBuyer" );
    append( walker::britonSoldier, "britonSoldier" );
    append( walker::fishPlace,  "fishPlace" );
    append( walker::seaMerchant,"seaMerchant" );
    append( walker::all,        "unknown" );
    append( walker::scholar,    "scholar" );
    append( walker::teacher,    "teacher" );
    append( walker::librarian,  "librarian" );
    append( walker::etruscanSoldier,"etruscanSoldier" );
    append( walker::charioteer, "charioteer" );
    append( walker::etruscanArcher, "etruscanArcher" );
    append( walker::spear, "spear" );
    append( walker::romeGuard,  "romeGuard" );
    append( walker::bow_arrow, "bow_arrow" );
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
