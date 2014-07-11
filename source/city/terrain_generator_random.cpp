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

#include "terrain_generator_random.hpp"

Random::Random() {
  srand(static_cast <unsigned> (time(0)));
}

Random::Random(int seed) {
  srand(seed);
}

Random::Random(double seed) {
  srand(seed);
}

Random::Random(float seed) {
  srand(seed);
}

int Random::Integer(int minimum, int maximum) {
  return minimum + (std::rand() % (maximum - minimum + 1));
}

double Random::Double(double minimum, double maximum) {
  return minimum + fmod(std::rand(), (maximum - minimum + 1.0));
}

float Random::Float(float minimum, float maximum) {
  return minimum + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maximum - minimum)));
}


