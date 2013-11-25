scons -c
scons BUILD="release" MACOSX_TARGET_ARCH="i386"
cp tdm_package.macosx tdm_package.i386.macosx
scons BUILD="release" MACOSX_TARGET_ARCH="ppc"
cp tdm_package.macosx tdm_package.ppc.macosx
lipo -arch i386 tdm_package.i386.macosx -arch ppc tdm_package.ppc.macosx -create -output tdm_package.macosx
