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

#ifndef __CAESARIA_DINIVITY_H_INCLUDED__
#define __CAESARIA_DINIVITY_H_INCLUDED__

#include "core/referencecounted.hpp"
#include "enums.hpp"
#include "core/scopedptr.hpp"
#include "game/predefinitions.hpp"
#include "vfs/path.hpp"
#include "service.hpp"
#include "core/variant.hpp"

typedef enum
{
  romeDivCeres = 0, 
  romeDivMars,
  romeDivNeptune,
  romeDivVenus,
  romeDivMercury,

  romeDivCount=0xff
} RomeDivinityType;

class RomeDivinity : public ReferenceCounted
{
public:
  virtual std::string getName() const = 0;
  virtual float getRelation() const = 0;
  virtual Service::Type getServiceType() const = 0;
  virtual std::string getShortDescription() const = 0;
  virtual const Picture& getPicture() const = 0;
  virtual void updateRelation( float income, PlayerCityPtr city ) = 0;
  virtual DateTime getLastFestivalDate() const = 0;
  virtual std::string getMoodDescription() const = 0;
  virtual void load( const VariantMap& vm ) = 0;
  virtual VariantMap save() const = 0;
  virtual void setInternalName(const std::string &newName) = 0;
};

class DivinePantheon
{
public:
  typedef std::vector< RomeDivinityPtr > Divinities;

  static DivinePantheon& getInstance();

  Divinities getAll();

  void load(const VariantMap& stream );
  void save(VariantMap& stream);

  static void doFestival( RomeDivinityType who, int type);

  static RomeDivinityPtr ceres();
  static RomeDivinityPtr mars();
  static RomeDivinityPtr neptune();
  static RomeDivinityPtr venus();
  static RomeDivinityPtr mercury();

  static RomeDivinityPtr get( RomeDivinityType name );
  static RomeDivinityPtr get( std::string name );

private:
  DivinePantheon();

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_DINIVITY_H_INCLUDED__
