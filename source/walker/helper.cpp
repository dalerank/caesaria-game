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

#include "animals.hpp"
#include "human.hpp"
#include "world/nation.hpp"
#include "core/logger.hpp"
#include "core/variant_map.hpp"
#include "core/saveadapter.hpp"

using namespace constants;
using namespace gfx;

class TypeEnums : public EnumsHelper<walker::Type>
{
public:
  TypeEnums() : EnumsHelper<walker::Type>( walker::unknown )
  {

  }
};

class HationEnums : public EnumsHelper<world::Nation>
{
public:
  HationEnums() : EnumsHelper<world::Nation>( world::unknownNation )
  {

  }
};

class WalkerHelper::Impl
{
public:    
  typedef std::map< walker::Type, std::string > PrettyNames;
  typedef std::map< world::Nation, std::string > PrettyNations;

  TypeEnums htype;
  PrettyNames typenames;

  HationEnums hnation;
  PrettyNations nationnames;

  VariantMap options;

  void appendType( walker::Type type, const std::string& name )
  {
    htype.append( type, name );
    typenames[ type ] = "##wt_" + name + "##";
  }

  void appendNation( world::Nation nation, const std::string& name )
  {
    hnation.append( nation, name );
    nationnames[ nation ] = "##wn_" + name + "##";
  }

  Impl()
  {
#define __REG_WNATION(a) appendNation( world::a, CAESARIA_STR_A(a));
    __REG_WNATION( unknownNation )
    __REG_WNATION( rome )
    __REG_WNATION( etruscan )
    __REG_WNATION( barbarian )
    __REG_WNATION( numidian )
    __REG_WNATION( pict )
    __REG_WNATION( samnite )
    __REG_WNATION( selecid )
    __REG_WNATION( carthaginian )
    __REG_WNATION( celt )
    __REG_WNATION( eygptian )
    __REG_WNATION( goth )
    __REG_WNATION( graeci )
    __REG_WNATION( judaean )
    __REG_WNATION( native )
    __REG_WNATION( visigoth )
    __REG_WNATION( gaul )
    __REG_WNATION( iberian )
    __REG_WNATION( helveti )
#undef __REG_WNATION

      appendType( walker::all,        "unknown" );

#define __REG_WTYPE(a) appendType( walker::a, CAESARIA_STR_A(a));
    __REG_WTYPE( unknown    )
    __REG_WTYPE( immigrant  )
    __REG_WTYPE( citizen    )
    __REG_WTYPE( emigrant   )
    __REG_WTYPE( soldier    )
    __REG_WTYPE( cartPusher )
    __REG_WTYPE( marketLady )
    __REG_WTYPE( marketBuyer)
    __REG_WTYPE( marketKid  )
    __REG_WTYPE( serviceman )
    __REG_WTYPE( trainee    )
    __REG_WTYPE( recruter   )
    __REG_WTYPE( prefect    )
    __REG_WTYPE( priest     )
    __REG_WTYPE( taxCollector)
    __REG_WTYPE( merchant   )
    __REG_WTYPE( engineer   )
    __REG_WTYPE( doctor     )
    __REG_WTYPE( sheep      )
    __REG_WTYPE( bathlady   )
    __REG_WTYPE( actor      )
    __REG_WTYPE( gladiator  )
    __REG_WTYPE( barber     )
    __REG_WTYPE( surgeon    )
    __REG_WTYPE( lionTamer  )
    __REG_WTYPE( fishingBoat)
    __REG_WTYPE( protestor  )
    __REG_WTYPE( legionary  )
    __REG_WTYPE( corpse     )
    __REG_WTYPE( lion       )
    __REG_WTYPE( britonSoldier )
    __REG_WTYPE( fishPlace  )
    __REG_WTYPE( seaMerchant)
    __REG_WTYPE( scholar    )
    __REG_WTYPE( teacher    )
    __REG_WTYPE( librarian  )
    __REG_WTYPE( etruscanSoldier )
    __REG_WTYPE( charioteer )
    __REG_WTYPE( etruscanArcher )
    __REG_WTYPE( spear      )
    __REG_WTYPE( waterGarbage )
    __REG_WTYPE( romeGuard  )
    __REG_WTYPE( bow_arrow  )
    __REG_WTYPE( romeHorseman )
    __REG_WTYPE( romeSpearman )
    __REG_WTYPE( balista    )
    __REG_WTYPE( mugger     )
    __REG_WTYPE( rioter     )
    __REG_WTYPE( wolf       )
    __REG_WTYPE( dustCloud  )
    __REG_WTYPE( romeChastenerSoldier )
    __REG_WTYPE( indigeneRioter )
    __REG_WTYPE( missioner  )
    __REG_WTYPE( indigene   )
    __REG_WTYPE( romeChastenerElephant )
    __REG_WTYPE( zebra      )
    __REG_WTYPE( supplier   )
    __REG_WTYPE( patrician  )
    __REG_WTYPE( circusCharioter )
    __REG_WTYPE( docker )
    __REG_WTYPE( gladiatorRiot )
    __REG_WTYPE( merchantCamel )
#undef __REG_WTYPE
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

bool WalkerHelper::isHuman(WalkerPtr wlk)
{
  return is_kind_of<Human>( wlk );
}

bool WalkerHelper::isAnimal(WalkerPtr wlk)
{
  return is_kind_of<Animal>( wlk ) || is_kind_of<Fish>( wlk);
}

void WalkerHelper::load( const vfs::Path& filename )
{
  _d->options = config::load( filename );
}

WalkerHelper& WalkerHelper::instance()
{
  static WalkerHelper inst;
  return inst;
}

std::string WalkerHelper::getTypename( walker::Type type )
{
  std::string name = instance()._d->htype.findName( type );

  if( name.empty() )
  {
    Logger::warning( "WalkerHelper: can't find walker typeName for %d", type );
    //_CAESARIA_DEBUG_BREAK_IF( "Can't find walker typeName by WalkerType" );
  }

  return name;
}

walker::Type WalkerHelper::getType(const std::string &name)
{
  walker::Type type = instance()._d->htype.findType( name );

  if( type == instance()._d->htype.getInvalid() )
  {
    Logger::warning( "WalkerHelper: can't find walker type for %s", name.c_str() );
    //_CAESARIA_DEBUG_BREAK_IF( "Can't find walker type by typeName" );
  }

  return type;
}

std::string WalkerHelper::getPrettyTypename(walker::Type type)
{
  Impl::PrettyNames::iterator it = instance()._d->typenames.find( type );
  return it != instance()._d->typenames.end() ? it->second : "";
}

std::string WalkerHelper::getNationName(world::Nation type)
{
  Impl::PrettyNations::iterator it = instance()._d->nationnames.find( type );
  return it != instance()._d->nationnames.end() ? it->second : "";
}

world::Nation WalkerHelper::getNation(const std::string &name)
{
  world::Nation nation = instance()._d->hnation.findType( name );

  if( nation == instance()._d->hnation.getInvalid() )
  {
    Logger::warning( "WalkerHelper: can't find nation type for %s", name.c_str() );
    //_CAESARIA_DEBUG_BREAK_IF( "Can't find walker type by typeName" );
  }

  return nation;
}

Picture WalkerHelper::getBigPicture(walker::Type type)
{
  int index = -1;
  switch( type )
  {
  case walker::immigrant: index=4; break;
  case walker::emigrant: index=9; break;
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


struct RelationInfo
{
  std::set<int> friends;
  std::set<int> enemies;

  void addFriend( int friendt )
  {
    friends.insert( friendt );
    enemies.erase( friendt );
  }

  void remFriend( int friendt )  { friends.erase( friendt );  }
  void remEnemy( int enemyt )  { enemies.erase( enemyt );  }

  void addEnemy( int enemyt )
  {
    friends.erase( enemyt );
    enemies.insert( enemyt );
  }
};

class WalkerRelations::Impl
{
public:
  typedef std::map< walker::Type, RelationInfo > WalkerRelations;
  typedef std::map<world::Nation, RelationInfo > NationRelations;

  WalkerRelations walkers;
  NationRelations nations;
};

WalkerRelations& WalkerRelations::instance()
{
  static WalkerRelations inst;
  return inst;
}

void WalkerRelations::addFriend(walker::Type who, walker::Type friendType)
{
  WalkerRelations::Impl::WalkerRelations& r = instance()._d->walkers;
  r[ who ].addFriend( friendType );
  r[ friendType ].addFriend( who );
}

void WalkerRelations::remFriend(walker::Type who, walker::Type friendType)
{
  WalkerRelations::Impl::WalkerRelations& r = instance()._d->walkers;
  r[ who ].remFriend( friendType );
  r[ friendType ].remFriend( who );
}

void WalkerRelations::addFriend(world::Nation who, world::Nation friendType)
{
  instance()._d->nations[ who ].addFriend( friendType );
  instance()._d->nations[ friendType ].addFriend( who );
}

void WalkerRelations::addEnemy(walker::Type who, walker::Type enemyType)
{
  instance()._d->walkers[ who ].addEnemy( enemyType );
  instance()._d->walkers[ enemyType ].addEnemy( who );
}

void WalkerRelations::addEnemy(world::Nation who, world::Nation enemyType)
{
  instance()._d->nations[ who ].addEnemy( enemyType );
  instance()._d->nations[ enemyType ].addEnemy( who );
}

template<class T, class Type>
bool __isNeutral( T& relations, Type a, Type b)
{
  typename T::iterator it = relations.find( a );

  if( it != relations.end() )
  {
    return it->second.enemies.count( b ) == 0;
  }

  return true;
}

bool WalkerRelations::isNeutral(walker::Type a, walker::Type b)
{
  return __isNeutral( instance()._d->walkers, a, b );
}

bool WalkerRelations::isNeutral(world::Nation a, world::Nation b)
{
  return __isNeutral( instance()._d->nations, a, b );
}

void WalkerRelations::load(vfs::Path path)
{
  VariantMap stream = config::load( path );
  load( stream );
}

template<class T>
void __fillRelations( const std::string& name, const VariantMap& items, const std::string& section,
                      T (*check)(const std::string&),
                      const std::string& warnText, void (*add)(T, T), T unknownType )
{
  T wtype = check( name );

  StringArray types = items.get( section ).toStringArray();
  foreach( itType, types )
  {
    T ftype = check( *itType );

    if( ftype == unknownType )
    {
      Logger::warning( warnText.c_str(), itType->c_str(), name.c_str() );
    }
    else
    {
      add( wtype, ftype );
    }
  }
}

void WalkerRelations::load(const VariantMap& stream)
{
  //_d->relations.clear();
  VariantMap wrelations = stream.get( "walkers" ).toMap();
  foreach( it, wrelations )
  {
    VariantMap item = it->second.toMap();
    __fillRelations<walker::Type>( it->first, item, "friend",
                                   &WalkerHelper::getType,
                                   "WalkerRelations: unknown friend %s for type %s",
                                   &WalkerRelations::addFriend, walker::unknown );

    __fillRelations<walker::Type>( it->first, item, "enemy",
                                   &WalkerHelper::getType,
                                   "WalkerRelations: unknown enemy %s for type %s",
                                   &WalkerRelations::addEnemy, walker::unknown );
  }

  VariantMap nrelations = stream.get( "nations" ).toMap();
  foreach( it,nrelations)
  {
    VariantMap item = it->second.toMap();
    __fillRelations<world::Nation>( it->first, item, "friend",
                                   &WalkerHelper::getNation,
                                   "NationRelations: unknown friend %s for type %s",
                                   &WalkerRelations::addFriend, world::unknownNation );

    __fillRelations<world::Nation>( it->first, item, "enemy",
                                   &WalkerHelper::getNation,
                                   "NationRelations: unknown enemy %s for type %s",
                                   &WalkerRelations::addEnemy, world::unknownNation );
  }
}

void WalkerRelations::clear()
{
  _d->walkers.clear();

}

VariantMap WalkerRelations::save() const
{
  VariantMap ret;

  return ret;
}

WalkerRelations::WalkerRelations() : _d( new Impl )
{

}
