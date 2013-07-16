#!/bin/bash

usage()
{
echo "opencaesar3 install script
Usage: opencaesar3 --pic-dir path1
Options:
   --pic-dir path : set the path to the extracted pictures
                    this path is the output directory of the sgreader tool (eg: ~/caesar3_extract)
                    it contains the C3, C3_North and C3_south directories
   --data-dir path: set the path to the extracted caesar3 archive
                    this path is the output directory of the unshield tool (eg: ~/caesar3_orig)
                    it contains the Exe directory
   -h, --help     : display this help
"
}


PIC_DIR="."
DATA_DIR="."

while true; do
   case "$1" in
   --pic-dir ) PIC_DIR="$2"; shift 2;;
   --data-dir ) DATA_DIR="$2"; shift 2;;
   -h | --help ) usage; exit 0;;
   -- ) shift; break;;
   * ) break;;
   esac
done

# make the output directories
mkdir -v -p resources
mkdir -v -p resources/pics
mkdir -v -p resources/maps
# rename all png files to lower case
rename -v 'y/A-Z/a-z/' $PIC_DIR/C3*
rename -v 'y/A-Z/a-z/' $PIC_DIR/c3/*.png
rename -v 'y/A-Z/a-z/' $PIC_DIR/c3_north/*.png
rename -v 'y/A-Z/a-z/' $PIC_DIR/c3_south/*.png
# archive all png files to a handful of zip files
rm resources/pics/pics.zip
rm resources/pics/pics_north.zip
rm resources/pics/pics_south.zip
zip -j resources/pics/pics.zip $PIC_DIR/c3/*.png
zip -j resources/pics/pics_north.zip $PIC_DIR/c3_north/*.png
zip -j resources/pics/pics_south.zip $PIC_DIR/c3_south/*.png
zip -j resources/pics/pics_wait.zip $PIC_DIR/c3/c3title*.png
# copy all maps
cp $DATA_DIR/Exe/*.map resources/maps
rename -v 'y/A-Z/a-z/' resources/maps/*.map

echo "All installed in directory: resources"
echo "DONE."


