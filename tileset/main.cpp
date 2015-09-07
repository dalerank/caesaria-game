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
#include "core/logger.hpp"
#include "gfx/loader_png.hpp"
#include "core/stringarray.hpp"
#include "core/utils.hpp"
#include "gfx/picture.hpp"
#include "vfs/directory.hpp"
#include <SDL_video.h>
#include <SDL.h>

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
private:
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

    bool IsLeaf()
    {
      return child1 == nullptr && child2 == nullptr;
    }

    // Algorithm from http://www.blackpawn.com/texts/lightmaps/
    Node* Insert(gfx::Picture rimage, int padding)
    {
      if(!IsLeaf())
      {
        Node* newNode = child1->Insert(rimage, padding);

        if(newNode != nullptr)
        {
          return newNode;
        }

        return child2->Insert(rimage, padding);
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

        return child1->Insert(rimage, padding);
      }
    }
  };

private:
  gfx::Picture image;
  Node* root;
  std::map<std::string, Node*> rectangleMap;

public:
  Texture(int width, int height)
  {
    image = gfx::Picture( Size( width, height ), 0, true );
    image.fill( DefaultColors::clear );
    //graphics = image.createGraphics();

    root = new Node(0,0, width, height);
  }

  bool AddImage(gfx::Picture rimage, const std::string& name, int padding)
  {
    Node* node = root->Insert(rimage, padding);

    if(node == nullptr)
    {
      return false;
    }

    rectangleMap[name] = node;
    SDL_Rect rect = { (short)node->rect.left(), (short)node->rect.top(), (ushort)rimage.width(), (ushort)rimage.height() };
    SDL_BlitSurface( rimage.surface(), nullptr, image.surface(), &rect );

    return true;
  }

public:
  void Write(const std::string& name, bool fileNameOnly, bool unitCoordinates, int width, int height)
  {
    try
    {
      //ImageIO.write(image, "png", new File(name + ".png"));

      vfs::NFile atlas = vfs::NFile::open( name + ".atlas", vfs::Entity::fmWrite );

      std::string header = "{\ntexture: \"" + name + ".png\" \n";
      std::string frames = "frames: {\n";

      atlas.write( header.c_str(), header.size() );
      atlas.write( frames.c_str(), frames.size() );
      for( auto&& e : rectangleMap )
      {
        Rect r = e.second->rect;
        std::string keyVal = e.first;

        if (fileNameOnly)
          keyVal = keyVal.substr(keyVal.find_last_of('/') + 1);
        if (unitCoordinates)
        {
          std::string str = utils::format( 0xff, "%s %f %f %f %f", keyVal.c_str(),
                                                           r.left()/(float)width,
                                                           r.top()/(float)height,
                                                           r.width()/(float)width,
                                                           r.height()/(float)height );
          atlas.write( str.c_str(), str.size() );
        }
        else
        {
          std::string str = utils::format( 0xff, "%s: [%d, %d, %d, %d]", keyVal.c_str(), r.left(), r.top(), r.width(), r.height() );
          atlas.write( str.c_str(), str.size() );
        }

        atlas.write( "\n", 1 );
      }
      atlas.write( "}\n}", 3 );
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
  void Run(std::string name, int width, int height, int padding, bool fileNameOnly, bool unitCoordinates, const StringArray& dirs)
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

      GetImageFiles(path, imageFiles);
		}

    Logger::warning( "Found %d images", imageFiles.size() );

    std::set<ImageName> imageNameSet;
    PictureLoaderPng pngLoader;

    for( const std::string& filename : imageFiles)
		{
			try
			{
        vfs::NFile file = vfs::NFile::open( filename );
        gfx::Picture image = pngLoader.load( file );
				
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
		
    std::vector<Texture*> textures;
    Texture* tx = new Texture(width, height);
    textures.push_back( tx );
		int count = 0;
		
    for(auto&& imageName : imageNameSet)
		{
      bool added = false;
			
      Logger::warning( "Adding " + imageName.name + " to atlas (" + utils::i2str(++count) + ")");
			
      for( auto&& texture : textures)
			{
        if(texture->AddImage(imageName.image, imageName.name, padding))
				{
					added = true;
					break;
				}
			}
			
			if(!added)
			{
        Texture* texture = new Texture(width, height);
        texture->AddImage(imageName.image, imageName.name, padding);
        textures.push_back(texture);
			}
		}
		
		count = 0;
		
    for(Texture* texture : textures)
		{
      Logger::warning( "Writing atlas: " + name + utils::i2str(++count));
      texture->Write(name + utils::i2str(count), fileNameOnly, unitCoordinates, width, height);
		}
	}
	
  void GetImageFiles(vfs::Path path, StringArray& imageFiles)
	{
    if(path.isFolder())
		{
      vfs::Directory directory( path );
      StringArray files = directory.entries().items().files( ".png" );
      StringArray directories = directory.entries().items().folders();
		
      imageFiles << files;
			
      for( auto str : directories)
			{
        GetImageFiles( vfs::Path( str ), imageFiles);
			}
		}
  }
};

int main(int argc, char* argv[])
{
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window* window = SDL_CreateWindow("Tileset",
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            800, 600,
                            SDL_WINDOW_OPENGL );
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED );


  Logger::registerWriter( Logger::consolelog, "" );

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

  atlasGenerator.Run(argv[1],
                     utils::toInt(argv[2]),
                     utils::toInt(argv[3]),
                     utils::toInt(argv[4]),
                     utils::toInt(argv[5]) != 0,
                     utils::toInt(argv[6]) != 0,
                     dirs);
}
