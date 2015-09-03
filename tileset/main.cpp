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

#include "core/logger.hpp"
#include "core/stringarray.hpp"
#include "core/utils.hpp"
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

  bool operator<(const ImageName& image2)
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

  Texture(int width, int height)
  {
    image = SDL_CreateRGBSurface( 0, width, height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 );
    graphics = image.createGraphics();

    root = new Node(0,0, width, height);
    rectangleMap = new TreeMap<String, Rectangle>();
  }

  bool AddImage(SDL_Surface* image, const std::string& name, int padding)
  {
    Node node = root.Insert(image, padding);

    if(node == null)
    {
      return false;
    }

    rectangleMap.put(name, node.rect);
    graphics.drawImage(image, null, node.rect.x, node.rect.y);

    return true;
  }

public:
  void Write(const std::string& name, bool fileNameOnly, bool unitCoordinates, int width, int height)
  {
    try
    {
      //ImageIO.write(image, "png", new File(name + ".png"));

      std::string atlasName = name + ".atlas";
      StringArray atlas;

      atlas << "{\ntexture: \"" + name + ".png\" \n";
      atlas << "frames: {\n";
      for( auto&& e : rectangleMap )
      {
        Rect r = e.second;
        const std::string& keyVal = e.first;

        if (fileNameOnly)
          keyVal = keyVal.substr(keyVal.lastIndexOf('/') + 1);
        if (unitCoordinates)
        {
          atlas.write(keyVal + " " + r.x/(float)width + " " + r.y/(float)height + " " + r.width/(float)width + " " + r.height/(float)height);
        }
        else
          atlas.write(keyVal + ": [" + r.x + ", " + r.y + ", " + r.width + ", " + r.height + " ]");
        atlas.newLine();
      }
      atlas.write( "}\n}" );
      atlas.close();
    }
    catch(IOException e)
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

    std::set<ImageName> imageNameSet();
		
    for( auto&& filename : imageFiles)
		{
			try
			{
        gfx::Picture image = gfx::Picture::load( filename );
				
        if(image.width() > width || image.height() > height)
				{
          Logger::warning( "Error: '%s' (%dx%d) ) is larger than the atlas (%dx%d)",
                           filename.getPath(), image.getWidth(), image.getHeight(), width, height );
					return;
				}
				
        String path = filename.getPath().substring(0, filename.getPath().lastIndexOf(".")).replace("\\", "/");
				
				imageNameSet.add(new ImageName(image, path));
				
			}
			catch(IOException e)
			{
        System.out.println("Could not open file: '" + filename.getAbsoluteFile() + "'");
			}
		}
		
		List<Texture> textures = new ArrayList<Texture>();
		
		textures.add(new Texture(width, height));
		
		int count = 0;
		
		for(ImageName imageName : imageNameSet)
		{
			boolean added = false;
			
			System.out.println("Adding " + imageName.name + " to atlas (" + (++count) + ")");
			
			for(Texture texture : textures)
			{
				if(texture.AddImage(imageName.image, imageName.name, padding))
				{
					added = true;
					break;
				}
			}
			
			if(!added)
			{
				Texture texture = new Texture(width, height);
				texture.AddImage(imageName.image, imageName.name, padding);
				textures.add(texture);
			}
		}
		
		count = 0;
		
		for(Texture texture : textures)
		{
			System.out.println("Writing atlas: " + name + (++count));
			texture.Write(name + count, fileNameOnly, unitCoordinates, width, height);
		}
	}
	
  void GetImageFiles(vfs::Path path, StringArray& imageFiles)
	{
    if(path.isFolder())
		{
      vfs::Directory directory( path );
      StringArray files = directory.listFiles( ".png" );
      StringArray directories = directory.listFolders();
		
      imageFiles.addAll( files );
			
      for( auto str : directories)
			{
        GetImageFiles( vfs::Path( d ), imageFiles);
			}
		}
  }
}

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
    return;
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
