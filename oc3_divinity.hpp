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

#ifndef __OPENCAESAR3_DINIVITY_H_INCLUDED__
#define __OPENCAESAR3_DINIVITY_H_INCLUDED__

#include "oc3_referencecounted.hpp"
#include "oc3_enums.hpp"
#include "oc3_scopedptr.hpp"
#include "oc3_predefinitions.hpp"
#include "oc3_filepath.hpp"
#include "oc3_service.hpp"

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
  virtual void updateRelation( float income ) = 0;
  virtual DateTime getLastFestivalDate() const = 0;
};

class DivinePantheon
{
public:
  typedef std::vector< RomeDivinityPtr > Divinities;

  static DivinePantheon& getInstance();
  static RomeDivinityPtr get( RomeDivinityType name );

  static Divinities getAll();

  void initialize( const io::FilePath& filename );

  static void doFestival4( RomeDivinityPtr who );

  static RomeDivinityPtr ceres();
  static RomeDivinityPtr mars();
  static RomeDivinityPtr neptune();
  static RomeDivinityPtr venus();
  static RomeDivinityPtr mercury();

private:
  DivinePantheon();

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif
