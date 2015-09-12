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

#include <cstdlib>
#include <iostream>
#include <set>

#include "vfs/file.hpp"
#include "gfx/IMG_savepng.h"
#include "core/logger.hpp"
#include "gfx/loader.hpp"
#include "core/debug_timer.hpp"
#include "gfx/sdl_engine.hpp"
#include "core/stringarray.hpp"
#include "core/utils.hpp"
#include "gfx/picture.hpp"
#include "vfs/directory.hpp"

#ifdef CAESARIA_PLATFORM_WIN
  #undef main
#endif

class ImageName
{
public:
  gfx::Picture image;
  std::string name;

  ImageName(gfx::Picture rimage, const std::string& rname)
  {
    image = rimage;
    name = rname;
  }

  ImageName& operator=(const ImageName& a)
  {
    image = a.image;
    name = a.name;
  }

  ImageName(const ImageName& a)
  {
    image = a.image;
    name = a.name;
  }

  bool operator<(const ImageName& image2) const
  {
    int area1 = this->image.width() * this->image.height();
    int area2 = image2.image.width() * image2.image.height();

    if(area1 != area2)
    {
      return area2 < area1;
    }
    else
    {
      return name < image2.name;
    }
  }
};

class Texture
{
public:
  class Node
  {
  public:
    Rect rect;
    Node* child1;
    Node* child2;
    gfx::Picture image;

    Node(int x, int y, int width, int height)
    {
      rect = Rect( Point(x, y), Size(width, height) );
      child1 = nullptr;
      child2 = nullptr;
    }

    bool isLeaf()
    {
      return child1 == nullptr && child2 == nullptr;
    }

    // Algorithm from http://www.blackpawn.com/texts/lightmaps/
    Node* insert(gfx::Picture rimage, int padding)
    {
      if(!isLeaf())
      {
        Node* newNode = child1->insert(rimage, padding);

        if(newNode != nullptr)
        {
          return newNode;
        }

        return child2->insert(rimage, padding);
      }
      else
      {
        if( this->image.isValid() )
        {
          return nullptr; // occupied
        }

        if(rimage.width() > rect.width() || rimage.height() > rect.height() )
        {
          return nullptr; // does not fit
        }

        if(rimage.width() == rect.width() && rimage.height() == rect.height())
        {
          this->image = rimage; // perfect fit
          return this;
        }

        int dw = rect.width() - rimage.width();
        int dh = rect.height() - rimage.height();

        if(dw > dh)
        {
          child1 = new Node(rect.left(), rect.top(), rimage.width(), rect.height());
          child2 = new Node(padding+rect.left()+rimage.width(), rect.top(), rect.width() - rimage.width() - padding, rect.height());
        }
        else
        {
          child1 = new Node(rect.left(), rect.top(), rect.width(), rimage.height());
          child2 = new Node(rect.left(), padding+rect.top()+rimage.height(), rect.width(), rect.height() - rimage.height() - padding);
        }

        return child1->insert(rimage, padding);
      }
    }
  };

private:
  Node* root;

public:
  gfx::Picture image;
  std::map<std::string, Node*> rectangleMap;

  Texture(int width, int height)
  {
    image = gfx::Picture( Size( width, height ), 0, true );
    image.fill( DefaultColors::clear );

    root = new Node(0,0, width, height);
  }

  bool addImage(gfx::Picture rimage, const std::string& name, int padding)
  {
    Node* node = root->insert(rimage, padding);

    if(node == nullptr)
    {
      return false;
    }

    rectangleMap[name] = node;
    image.draw( rimage, node->rect.lefttop(), rimage.size() );

    return true;
  }

public:
  void write(const std::string& name, bool fileNameOnly, bool unitCoordinates, int width, int height)
  {
    try
    {
      image.save( name + ".png" );

      vfs::NFile atlas = vfs::NFile::open( name + ".atlas", vfs::Entity::fmWrite );

      atlas.write( "{\ntexture: \"" + name + ".png\" \n" );
      atlas.write( "  frames: {\n" );
      for( auto&& e : rectangleMap )
      {
        Rect r = e.second->rect;
        std::string keyVal = e.first;

        if (fileNameOnly)
          keyVal = keyVal.substr(keyVal.find_last_of('/') + 1);

        if (unitCoordinates)
        {
          std::string str = utils::format( 0xff, "    %s %f %f %f %f", keyVal.c_str(),
                                                           r.left()/(float)width,
                                                           r.top()/(float)height,
                                                           r.width()/(float)width,
                                                           r.height()/(float)height );
          atlas.write( str );
        }
        else
        {
          std::string str = utils::format( 0xff, "    %s: [%d, %d, %d, %d]", keyVal.c_str(), r.left(), r.top(), r.width(), r.height() );
          atlas.write( str );
        }

        atlas.write( "\n" );
      }
      atlas.write( "  }\n}" );
      atlas.flush();
    }
    catch(...)
    {

    }
  }
};

class AtlasGenerator
{	
public:
  std::vector<Texture*> textures;

  void run(std::string name, int width, int height, int padding, bool fileNameOnly, bool unitCoordinates, const StringArray& dirs)
	{
    StringArray imageFiles;

    for( auto str : dirs )
		{
      vfs::Path path(str);
      if(!path.exist() || !path.isFolder())
			{
        Logger::warning("Error: Could not find directory '" + path.toString() + "'");
				return;
			}

      getImageFiles(path, imageFiles);
		}

    Logger::warning( "Found %d images", imageFiles.size() );

    std::set<ImageName> imageNameSet;

    for( const std::string& filename : imageFiles)
		{
			try
			{
        vfs::NFile file = vfs::NFile::open( filename );
        gfx::Picture image = PictureLoader::instance().load( file, true );

        if(image.width() > width || image.height() > height)
				{
          Logger::warning( "Error: '%s' (%dx%d) ) is larger than the atlas (%dx%d)",
                           filename.c_str(), image.width(), image.height(), width, height );
					return;
				}			        
				
        ImageName in(image, file.path().baseName(false).toString() );
        imageNameSet.insert( in );
			}
      catch(...)
			{
        Logger::warning( "Could not open file: '" + filename + "'" );
			}
		}
		    
    Texture* tx = new Texture(width, height);
    textures.push_back( tx );
		int count = 0;
		
    for( auto&& imageName : imageNameSet )
		{
      bool added = false;
			
      Logger::warning( "Adding " + imageName.name + " to atlas (" + utils::i2str(++count) + ")");
			
      for( auto&& texture : textures)
			{
        if(texture->addImage(imageName.image, imageName.name, padding))
				{
					added = true;
					break;
				}
			}
			
			if(!added)
			{
        Texture* texture = new Texture(width, height);
        texture->addImage(imageName.image, imageName.name, padding);
        textures.push_back(texture);
			}
		}
		
		count = 0;
		
    for(Texture* texture : textures)
		{
      Logger::warning( "Writing atlas: " + name + utils::i2str(++count));
      texture->write(name + utils::i2str(count), fileNameOnly, unitCoordinates, width, height);
		}
	}
	
  void getImageFiles(vfs::Path path, StringArray& imageFiles)
	{
    if(path.isFolder())
		{
      vfs::Directory directory( path );
      StringArray files = directory.entries().items().files( ".png" );
      StringArray directories = directory.entries().items().folders();
		
      imageFiles << files;
			
      for( auto str : directories)
			{
        getImageFiles( vfs::Path( str ), imageFiles);
			}
		}
  }
};

int main(int argc, char* argv[])
{
  Logger::registerWriter( Logger::consolelog, "" );
  gfx::Engine* engine = new gfx::SdlEngine();

  Logger::warning( "GraficEngine: set size" );  
  engine->setScreenSize( Size( 800, 800 ) );
  engine->setFlag( gfx::Engine::debugInfo, true );
  engine->setFlag( gfx::Engine::batching, false );
  engine->init();

  if(argc < 5)
  {
    Logger::warning("CaesarIA texture atlas generator by Dalerank(java code Lukasz Bruun)");
    Logger::warning("\tUsage: AtlasGenerator <name> <width> <height> <padding> <ignorePaths> <unitCoordinates> <directory> [<directory> ...]");
    Logger::warning("\t\t<padding>: Padding between images in the final texture atlas.");
    Logger::warning("\t\t<ignorePaths>: Only writes out the file name without the path of it to the atlas txt file.");
    Logger::warning("\t\t<unitCoordinates>: Coordinates will be written to atlas json file in 0..1 range instead of 0..width, 0..height range");
    Logger::warning("\tExample: tileset atlas 2048 2048 5 1 1 images");
    return 0;
  }

  AtlasGenerator atlasGenerator;
  StringArray dirs;

  for(int i = 7; i < argc; ++i)
    dirs << argv[i];

  atlasGenerator.run(argv[1],
                     utils::toInt(argv[2]),
                     utils::toInt(argv[3]),
                     utils::toInt(argv[4]),
                     utils::toInt(argv[5]) != 0,
                     utils::toInt(argv[6]) != 0,
                     dirs);

  bool running = true;
  SDL_Event event;

  bool gray = true;
  bool ygray = true;
  gfx::Picture bg( engine->screenSize(), 0, true );
  int offset = 10;
  for( int x=0; x < bg.width(); x+= offset )
  {
    ygray = !ygray;
    gray = ygray;
    for( int y=0; y < bg.height(); y+= offset )
    {
      bg.fill( gray ? DefaultColors::darkSlateGray : DefaultColors::lightSlateGray, Rect( x, y, x+offset, y+offset ) );
      gray = !gray;
    }    
  }
  bg.update();

  int index = 0;
  while(running)
  {
    static unsigned int lastTimeUpdate = DebugTimer::ticks();    
    while(SDL_PollEvent(&event) != 0)
    {
      if(event.type == SDL_QUIT) running = false;
      if(event.type == SDL_KEYUP) index++;
      if(event.type == SDL_KEYDOWN) index--;
    }

    index = math::clamp<int>( index, 0, atlasGenerator.textures.size() );
    engine->startRenderFrame();

    engine->draw( bg, Point() );
    gfx::Picture pic = atlasGenerator.textures[ index ]->image;
    engine->draw( pic, Rect( Point(), pic.size()), Rect( Point(), Size(800) ) );
    engine->endRenderFrame();

    int delayTicks = DebugTimer::ticks() - lastTimeUpdate;
    if( delayTicks < 33 )
    {
      engine->delay( std::max<int>( 33 - delayTicks, 0 ) );
    }
    lastTimeUpdate = DebugTimer::ticks();
  }
}
