# make a SVN tag:
# svn copy file:///home/greg/dev/SVN/caesar/trunk file:///home/greg/dev/SVN/caesar/tags/0-0-2

DATE=`date +"%Y_%m_%d.%H_%M_%S"`
DIR=BAK/REL_WIN32_$DATE
DST=$DIR/opencaesar3
mkdir -p $DST

cp caesar.exe $DST
cp *.csv $DST

# copy text files in windows format
for FILE in INSTALL_WINDOWS README LICENSE*
do
   cp $FILE $DST/$FILE.txt
   todos $DST/$FILE.txt
done

# copy dependencies
cp win32_deps/*.dll $DST
cp win32_deps/*.ttf $DST

mkdir $DST/sgreader
cp win32_deps/sgreader/*.exe $DST/sgreader
cp win32_deps/sgreader/*.txt $DST/sgreader

# install translations
mkdir -p $DST/fr/LC_MESSAGES
cp fr/LC_MESSAGES/caesar.po $DST/fr/LC_MESSAGES

# make stubs for resources directory
mkdir -p $DST/resources/pics
mkdir -p $DST/resources/maps

cd $DIR
zip -r opencaesar3-win32-$DATE.zip opencaesar3
cd -
mv $DIR/opencaesar3-win32-$DATE.zip BAK

