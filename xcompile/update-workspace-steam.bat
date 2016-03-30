cd ../
mkdir MSVC
cd MSVC
cmake -DCMAKE_BUILD_TYPE=Release ../ -DUSE_STEAM=1
start CaesarIA.sln /D .

