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
#include "SDL/SDL_video.h"

class ImageName
{
public:
  SDL_Surface* image;
  std::string name;

  ImageName(SDL_Surface* rimage, const std::string& rname)
  {
    image = rimage;
    name = rname;
  }

  ImageName(const ImageName& a )
  {
    image = a.image;
    name = a.name;
  }

  bool operator<(const ImageName& image2) const
  {
    int area1 = this->image->w * this->image->h;
    int area2 = image2.image->w * image2.image->h;

    if(area1 != area2)
    {
      return area2 < area1;
    }
    else
    {
      return strcmp(this->name.c_str(), image2.name.c_str()) < 0;
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
    std::vector<Node*> child;
    SDL_Surface* image;

    Node(int x, int y, int width, int height)
    {
      rect = Rect(x, y, width, height);
      child.resize(2);
      child[0] = nullptr;
      child[1] = nullptr;
      image = nullptr;
    }

    bool IsLeaf()
    {
      return child[0] == nullptr && child[1] == nullptr;
    }

    // Algorithm from http://www.blackpawn.com/texts/lightmaps/
    Node* Insert(SDL_Surface* image, int padding)
    {
      if(!IsLeaf())
      {
        Node* newNode = child[0]->Insert(image, padding);

        if(newNode != nullptr)
        {
          return newNode;
        }

        return child[1]->Insert(image, padding);
      }
      else
      {
        if(this->image != nullptr)
        {
          return nullptr; // occupied
        }

        if(image->w > rect.width() || image->h > rect.height() )
        {
          return nullptr; // does not fit
        }

        if(image->w == rect.width() && image->h == rect.height())
        {
          this->image = image; // perfect fit
          return this;
        }

        int dw = rect.width() - image->w;
        int dh = rect.height() - image->h;

        if(dw > dh)
        {
          child[0] = new Node(rect.left(), rect.top(), image->w, rect.height());
          child[1] = new Node(padding+rect.left()+image->w, rect.top(), rect.width() - image->w - padding, rect.height());
        }
        else
        {
          child[0] = new Node(rect.left(), rect.top(), rect.width(), image->h);
          child[1] = new Node(rect.left(), padding+rect.top()+image->h, rect.width(), rect.height() - image->h - padding);
        }
        /*if(dw > dh)
        {
          child[0] = new Node(rect.x, rect.y, image.getWidth(), rect.height);
          child[1] = new Node(padding+rect.x+image.getWidth(), rect.y, rect.width - image.getWidth(), rect.height);
        }
        else
        {
          child[0] = new Node(rect.x, rect.y, rect.width, image.getHeight());
          child[1] = new Node(rect.x, padding+rect.y+image.getHeight(), rect.width, rect.height - image.getHeight());
        }*/

        return child[0]->Insert(image, padding);
      }
    }
  };

private:
  SDL_Surface* image;
  //Graphics2D graphics;
  Node* root;
  std::map<std::string, Rect> rectangleMap;

public:
  Texture(int width, int height)
  {
    image = SDL_CreateRGBSurface( 0, width, height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 );
    //graphics = image.createGraphics();

    root = new Node(0,0, width, height);
  }

  bool AddImage(SDL_Surface* rimage, const std::string& name, int padding)
  {
    Node* node = root->Insert(rimage, padding);

    if(node == nullptr)
    {
      return false;
    }

    rectangleMap[name] = node->rect;
    SDL_Rect rect = { (short)node->rect.left(), (short)node->rect.top(), (ushort)rimage->w, (ushort)rimage->h };
    SDL_BlitSurface( rimage, nullptr, image, &rect );

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
        Rect r = e.second;
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

    for( auto&& filename : imageFiles)
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
				
        ImageName in(image.surface(), file.path().baseName(false).toString() );
        imageNameSet.insert( in );
			}
      catch(...)
			{
        Logger::warning( "Could not open file: '" + filename + "'" );
			}
		}
		
    std::vector<Texture*> textures;
    textures.push_back( new Texture(width, height) );
		int count = 0;
		
		for(ImageName imageName : imageNameSet)
		{
      bool added = false;
			
      Logger::warning( "Adding " + imageName.name + " to atlas (" + utils::i2str(++count) + ")");
			
      for( auto texture : textures)
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
  if(argc < 4)
  {
    Logger::warning("Texture Atlas Generator by Lukasz Bruun - lukasz.dk");
    Logger::warning("\tUsage: AtlasGenerator <name> <width> <height> <padding> <ignorePaths> <unitCoordinates> <directory> [<directory> ...]");
    Logger::warning("\t\t<padding>: Padding between images in the final texture atlas.");
    Logger::warning("\t\t<ignorePaths>: Only writes out the file name without the path of it to the atlas txt file.");
    Logger::warning("\t\t<unitCoordinates>: Coordinates will be written to atlas txt file in 0..1 range instead of 0..width, 0..height range");
    Logger::warning("\tExample: AtlasGenerator atlas 2048 2048 5 1 1 images");
    return 0;
  }

  AtlasGenerator atlasGenerator;
  StringArray dirs;

  for(int i = 6; i < argc; ++i)
    dirs << argv[i];

  atlasGenerator.Run(argv[0],
                     utils::toInt(argv[1]),
                     utils::toInt(argv[2]),
                     utils::toInt(argv[3]),
                     utils::toInt(argv[4]) != 0,
                     utils::toInt(argv[5]) != 0,
                     dirs);
}
