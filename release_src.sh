# make a SVN tag:
# svn copy file:///home/greg/dev/SVN/caesar/trunk file:///home/greg/dev/SVN/caesar/tags/0-0-2

DATE=`date +"%Y_%m_%d.%H_%M_%S"`
DIR=BAK/REL_SRC_$DATE
DST=$DIR/opencaesar3
mkdir -p $DST
cp *.?pp $DST
cp make* $DST
cp *.sh $DST
cp *.csv $DST
cp INSTALL* $DST
cp README* $DST
cp LICENSE* $DST
mkdir -p $DST/fr/LC_MESSAGES
cp fr/LC_MESSAGES/caesar.po $DST/fr/LC_MESSAGES

tar cvz --directory $DIR -f opencaesar3-src-$DATE.tgz opencaesar3
mv opencaesar3-src-$DATE.tgz BAK


