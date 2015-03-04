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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "good.hpp"
#include "core/priorities.hpp"

namespace good
{

inline Products& operator<<(Products& t, const Product& a)
{
  t.push_back( a );
  return t;
}

struct Stage
{
  Products foods;
  Products materials;
  Products all;
  Products merchandise;
  Product any;

  Stage()
  {
    all << none << wheat
        << fish << meat << fruit
        << vegetable << olive << oil
        << grape << wine << timber
        << furniture << clay << pottery
        << iron << weapon << marble << denaries
        << prettyWine;

    foods << wheat << fish << meat << fruit << vegetable;
    materials << olive << grape << timber << clay << iron << marble;
    merchandise << wine << furniture << pottery << weapon << marble << prettyWine;
    any = Product( all.size() );
  }
};

static Stage stage;

const Product& any() { return stage.any; }
const Products& all() { return stage.all; }
const Products& materials() { return stage.materials; }
const Products& foods() { return stage.foods; }

}
