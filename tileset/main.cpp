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
#include "core/saveadapter.hpp"
#include "gfx/loader.hpp"
#include "core/color_list.hpp"
#include "core/variant_map.hpp"
#include "core/debug_timer.hpp"
#include "gfx/sdl_engine.hpp"
#include "core/stringarray.hpp"
#include "core/utils.hpp"
#include "gfx/picture.hpp"
#include "vfs/directory.hpp"
#include "zlib.h"
#include "zip.h"

#ifdef GAME_PLATFORM_WIN
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
    image.fill( ColorList::clear );

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
      for( auto& e : rectangleMap )
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
  StringArray names;

  void run(const std::string& name, int width, int height, int padding, bool fileNameOnly, bool unitCoordinates,
           const StringArray& dirs, const StringArray& files = StringArray())
	{
    StringArray imageFiles;
    imageFiles << files;

    for( auto str : dirs )
		{
      vfs::Path path(str);
      if(!path.exist() || !path.isFolder())
			{
        Logger::warning("Error: Could not find directory '{0}'", path.toString());
				return;
			}

      getImageFiles(path, imageFiles);
		}

    Logger::warning( "Found {} images", imageFiles.size() );

    std::set<ImageName> imageNameSet;

    for( const std::string& filename : imageFiles)
		{
			try
			{
        vfs::NFile file = vfs::NFile::open( filename );
        gfx::Picture image = PictureLoader::instance().load( file, true );

        if(image.width() > width || image.height() > height)
				{
          Logger::warning( "Error: '{0}' ({1}x{2}) ) is larger than the atlas ({3}x{4}})",
                           filename, image.width(), image.height(), width, height );
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
		
    for( auto& imageName : imageNameSet )
		{
      bool added = false;
			
      Logger::warning( "Adding " + imageName.name + " to atlas (" + utils::i2str(++count) + ")");
			
      for( auto& texture : textures)
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
      Logger::warning( "Writing atlas: {} {}", name, utils::i2str(++count));
      std::string txName = name + utils::i2str(count);
      texture->write(txName, fileNameOnly, unitCoordinates, width, height);
      names.push_back( txName + ".png" );
      names.push_back( txName + ".atlas" );
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

struct ArchPath
{
  std::string repo;
  std::string output;
};

struct ArchiveConfig
{
  struct Item
  {
    typedef enum { atlas, file } Type;
    const ArchPath& archpath;
    Type type;
    std::string folder;
    StringArray files;

    Item( const ArchPath& arch ) :
      archpath( arch )
    {

    }

    void load( const VariantMap& vm )
    {
      std::string typestr = vm.get( "type" ).toString();
      type = (typestr == "atlas" ? atlas : file);
      VARIANT_LOAD_STR( folder, vm );

      StringArray rfiles = vm.get( "files" ).toStringArray();
      vfs::Directory repoFolder( archpath.repo );
      vfs::Directory addFolder( folder );

      vfs::Directory mainFolder( repoFolder/addFolder );

      if( !mainFolder.exist() )
        return;

      const vfs::Entries entries = mainFolder.entries();
      for( auto& str : rfiles )
      {
        if( str.back() == '*' )
        {
          std::string templateStr = str;
          templateStr.resize( str.size() - 1 );

          for( auto& item : entries.items() )
          {
            if( item.name.toString().compare(0, templateStr.size(), templateStr) == 0 )
            {
              files.push_back( item.fullpath.toString() );
            }
          }
        }
        else
        {
          vfs::Path fpath( mainFolder/str );
          files.push_back( fpath.absolutePath().toString() );
        }
      }
    }
  };

  struct Items : public std::vector<Item>
  {
    const ArchPath& archpath;

    Items( const ArchPath& arch )
     : archpath( arch )
    {

    }

    void add()
    {
      push_back( Item( archpath ) );
    }

    void load( const VariantMap& vm )
    {
      for( auto& i : vm )
      {
        push_back( Item( archpath ) );
        back().load( i.second.toMap() );
      }
    }
  };

  ArchiveConfig( const ArchPath& arch ) :
    archpath( arch ), items( arch )
  {

  }

  //const std::string& repo;
  std::string name;
  std::string archive;
  const ArchPath& archpath;
  VariantMap info;
  int margin;
  bool ignorePath;
  bool floatCoordinates;
  Size size;
  Items items;

  void add()
  {
    items.add();
  }

  void load( const VariantMap& vm )
  {
    VARIANT_LOAD_STR( name, vm )
    VARIANT_LOAD_STR( archive, vm )
    VARIANT_LOAD_ANY( size, vm )
    VARIANT_LOAD_VMAP( info, vm )
    VARIANT_LOAD_ANY( floatCoordinates, vm )
    VARIANT_LOAD_ANY( margin, vm )
    VARIANT_LOAD_ANY( ignorePath, vm )
    VARIANT_LOAD_CLASS( items, vm )
  }
};

struct Config
{
  struct Archives : public std::vector<ArchiveConfig>
  {
    const ArchPath& archpath;

    Archives( const ArchPath& arch ) :
       archpath( arch )
    {

    }

    void add()
    {
      push_back( ArchiveConfig( archpath ) );
    }

    void load( const VariantMap& vm )
    {
      for( auto& item : vm )
      {
        push_back( ArchiveConfig( archpath ) );
        back().load( item.second.toMap() );
      }
    }
  };

  ArchPath archpath;
  Archives archives;

  Config() :
    archives( archpath )
  {

  }

  void load( const VariantMap& vm )
  {
    archpath.repo = vm.get( "repository" ).toString();
    archpath.output = vm.get( "output" ).toString();
    VARIANT_LOAD_CLASS( archives, vm )
  }

  void once(const std::string& name, int width, int height, int padding, bool fileNameOnly, bool unitCoordinates, const StringArray& dirs)
  {
    archpath.output = "";
    archpath.output = ".";
    archives.add();

    ArchiveConfig& arch = archives.back();
    arch.name = name;
    arch.size = Size( width, height );
    arch.margin = padding;
    arch.ignorePath = fileNameOnly;
    arch.floatCoordinates = unitCoordinates;
  }
};

class Atlases : public std::vector<AtlasGenerator*>
{
public:
  gfx::Picture findByIndex( int index, std::string& name )
  {
    gfx::Picture ret = gfx::Picture::getInvalid();
    int currentStart = 0;
    for( auto& a : *this )
    {
      if( index >= currentStart &&
          index < currentStart + a->textures.size() )
      {
        int tI = index - currentStart;
        ret = a->textures[ tI ]->image;
        name = a->names[ tI ];
        break;
      }
      else
      {
        currentStart += a->textures.size();
      }
    }

    return ret;
  }

  int max() const
  {
    int result = 0;
    for( auto& a : *this )
      result += a->textures.size();

    return result;
  }
};

void createSet( const ArchiveConfig& archive, const StringArray& names )
{
  vfs::Directory outputDir( archive.archpath.output );
  vfs::Path relativePath( archive.archive );
  relativePath = relativePath.changeExtension( "zip" );
  vfs::Path arcName = outputDir/relativePath;
  vfs::Directory realDir = arcName.directory();

  if( !realDir.exist() )
    vfs::Directory::createByPath( realDir );

  int createMode = APPEND_STATUS_CREATE;
  if( arcName.exist() )
    createMode = APPEND_STATUS_ADDINZIP;

  zipFile zf = zipOpen( arcName.toCString(), createMode );
  if ( zf == nullptr )
  {
     /* Handle error */
     Logger::warning( "Unable to open {} for writing\n", arcName.toCString() );
     return;
  }

  const unsigned int bufferSize = 1024;
  char buf[bufferSize] = { 0 };
  size_t bytes_read = 0;
  int err;
  zip_fileinfo zi;

  int opt_compress_level=Z_DEFAULT_COMPRESSION;
  for( auto& filename : names )
  {
    vfs::NFile nfile = vfs::NFile::open( filename );

    zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
    zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
    zi.dosDate = 0;
    zi.internal_fa = 0;
    zi.external_fa = 0;
    zipFiletime(filename.c_str(),&zi.tmz_date,&zi.dosDate);

    std::string basename = vfs::Path( filename ).baseName().toString();
    err = zipOpenNewFileInZip(zf,basename.c_str(),&zi,
                              NULL,0,NULL,0,NULL,
                              (opt_compress_level != 0) ? Z_DEFLATED : 0,
                              opt_compress_level);

    bytes_read = nfile.read( buf, bufferSize );
    while (bytes_read > 0)
    {
       err = zipWriteInFileInZip (zf,buf,bytes_read);
       if (err != ZIP_OK)
       {
          Logger::warning( "Error during compression with file " + filename );
          err = zipCloseFileInZip(zf);
          break;
       }
       bytes_read = nfile.read(buf, bufferSize);
    }

    err = zipCloseFileInZip(zf);
  }

  std::string data = config::save( archive.info );
  err = zipOpenNewFileInZip( zf, "info", &zi,
                             NULL,0,NULL,0,NULL,
                             (opt_compress_level != 0) ? Z_DEFLATED : 0,
                             opt_compress_level);
  zipWriteInFileInZip( zf, data.c_str(), data.size() );
  zipCloseFileInZip(zf);

  zipClose(zf,NULL);
}

void unpackAtlases( const std::string& fullpath )
{
  vfs::Directory currentDir = vfs::Path( fullpath ).directory();
  vfs::Entries::Items entries = currentDir.entries().items();
  for( auto& item : entries )
  {
    if( item.name.isMyExtension( ".atlas" )  )
    {
      VariantMap config = config::load( item.name );
      vfs::Path textureName = config.get( "texture" ).toString();

      vfs::Path dirName = textureName.removeExtension();
      bool dirCreated = currentDir.create( dirName.toString() );
      if( !dirCreated )
      {
        Logger::warning( "WARNING !!! Cant create directory " + textureName.toString() );
        continue;
      }

      vfs::Directory dir2save = currentDir/dirName;

      vfs::NFile file = vfs::NFile::open( textureName );
      gfx::Picture atlasTx = PictureLoader::instance().load( file, true );

      VariantMap frames = config.get( "frames" ).toMap();
      for( auto& frame : frames )
      {
        std::string name = frame.first + ".png" ;
        VariantList rectVl = frame.second.toList();
        Point start( rectVl.get( 0 ).toInt(), rectVl.get( 1 ).toInt() );
        Size size( rectVl.get( 2 ).toInt(), rectVl.get( 3 ).toInt() );

        gfx::Picture image = gfx::Picture( size, 0, true );
        image.fill( ColorList::clear );

        image.draw( atlasTx, Rect( start, size ) );
        image.save( (dir2save/name).toString() );
      }
    }
  }
}

int main(int argc, char* argv[])
{
  Logger::registerWriter( Logger::consolelog, "" );
  gfx::Engine* engine = new gfx::SdlEngine();

  Logger::warning( "GraficEngine: set size 800x800" );
  engine->setScreenSize( Size( 800, 800 ) );
  engine->setFlag( gfx::Engine::showMetrics, true );
  engine->setFlag( gfx::Engine::batching, false );
  engine->init();
  engine->setTitle( "CaesarIA: tileset packer" );

  Config config;
  Atlases gens;

  if(argc == 2 && strcmp( argv[1], "unpack" ) == 0 )
  {
    unpackAtlases( argv[0] );
    return 0;
  }

  vfs::Path path( "tileset.model" );
  if( path.exist() )
  {
    VariantMap vm = config::load( path );
    config.load( vm );
  }
  else
  {
    if(argc < 5)
    {
      Logger::warning("CaesarIA texture atlas generator by Dalerank(java code Lukasz Bruun)");
      Logger::warning("\tUsage: AtlasGenerator <name> <width> <height> <padding> <ignorePaths> <unitCoordinates> <directory> [<directory> ...]");      
      Logger::warning("\t\t<padding>: Padding between images in the final texture atlas.");
      Logger::warning("\t\t<ignorePaths>: Only writes out the file name without the path of it to the atlas txt file.");
      Logger::warning("\t\t<unitCoordinates>: Coordinates will be written to atlas json file in 0..1 range instead of 0..width, 0..height range");
      Logger::warning("\tExample: tileset atlas 2048 2048 5 1 1 images");
      Logger::warning("\t");
      Logger::warning("\tUsage: AtlasGenerator unpack");
      Logger::warning("\twill unpack current atlasses to different textures");
      return 0;
    }

    StringArray dirs;
    for(int i = 7; i < argc; ++i)
      dirs << argv[i];

    config.once( argv[1],
                 utils::toInt(argv[2]),
                 utils::toInt(argv[3]),
                 utils::toInt(argv[4]),
                 utils::toInt(argv[5]) != 0,
                 utils::toInt(argv[6]) != 0,
                 dirs );
  }

  for( auto& archiveIt : config.archives )
  {
    StringArray atlasFiles;
    StringArray allFiles;
    for( const ArchiveConfig::Item& item : archiveIt.items)
    {
      if( item.type == ArchiveConfig::Item::atlas )
        atlasFiles << item.files;
      else
        allFiles << item.files;
    }

    auto gen = new AtlasGenerator();
    gen->run( archiveIt.name,
              archiveIt.size.width(),
              archiveIt.size.height(),
              archiveIt.margin,
              archiveIt.ignorePath,
              archiveIt.floatCoordinates,
              StringArray(),
              atlasFiles );
    gens.push_back( gen );

    StringArray info = archiveIt.info[ "atlas" ].toStringArray();
    for( auto& name : gen->names )
    {
      if( name.find( ".atlas" ) != std::string::npos )
        info.push_back( name );
    }
    archiveIt.info[ "atlas" ] = info;

    allFiles << gen->names;
    createSet( archiveIt, allFiles );
  }

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
      bg.fill( gray ? ColorList::darkSlateGray : ColorList::lightSlateGray, Rect( x, y, x+offset, y+offset ) );
      gray = !gray;
    }    
  }
  bg.update();

  int index = 0;
  std::string picName;
  gfx::Picture pic = gens.findByIndex(index, picName);
  engine->setTitle( picName );

  while(running)
  {
    static unsigned int lastTimeUpdate = DebugTimer::ticks();    
    while(SDL_PollEvent(&event) != 0)
    {
      if(event.type == SDL_QUIT) running = false;
      if(event.type == SDL_KEYUP)
      {
        if( event.key.keysym.sym == SDLK_UP ) index++;
        if( event.key.keysym.sym == SDLK_DOWN) index--;

        pic = gens.findByIndex(index, picName);
        engine->setTitle( picName );
      }
    }

    index = math::clamp<int>( index, 0, gens.max()-1 );
    engine->frame().start();

    engine->draw( bg, Point() );    
    engine->draw( pic, Rect( Point(), pic.size()), Rect( Point(), Size(800,800) ) );

    engine->frame().finish();

    int delayTicks = DebugTimer::ticks() - lastTimeUpdate;
    if( delayTicks < 33 )
    {
      engine->delay( std::max<int>( 33 - delayTicks, 0 ) );
    }
    lastTimeUpdate = DebugTimer::ticks();
  }
}
