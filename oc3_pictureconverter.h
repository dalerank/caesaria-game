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

#ifndef __OPENCAESAR3_PICTURECONVERTER_H_INCLUDE_
#define __OPENCAESAR3_PICTURECONVERTER_H_INCLUDE_

#include <memory>

class PictureConverter;
typedef std::auto_ptr< PictureConverter > PictureConverterPtr;

class Picture;

class PictureConverter
{
    friend PictureConverterPtr;
public:
    static PictureConverterPtr create();

    void convToGrayscale( Picture& dst, const Picture& src );
    void rgbBalance( Picture& dst, const Picture& src, int lROffset, int lGOffset, int lBOffset );

private:
    PictureConverter();
    ~PictureConverter();
};

#endif