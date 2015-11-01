#!/bin/bash
updaterepo=1
debug_build=0
buildwindows=1
buildlinux=1
buildandroid=1
projectdir=""
buildmac=1
sf_password=""
sf_folder=""
waste_folder=""
have_sfpass=0
have_sffolder=0
have_wastefolder=0
create_update=1
download_lastres=0
send_to_remote=1

txtred='\e[0;31m' # Red
txtgrn='\e[0;32m' # Green
txtblu='\e[0;34m' # Blue
txtrst='\e[0m'    # Text Reset

while [ $# -gt 0 ]; do
  case "$1" in
    --pdir=*)
      project="${1#*=}"
      ;;
    --sfpass=*)
      sf_password="${1#*=}"
      ;;
		--sffolder=*)
      sf_folder="${1#*=}"
      ;;
		--wastedir=*)
      directory="${1#*=}"
      ;;
		--nogit)
      updaterepo=0
      ;;
  	--debugb)
      debug_build=1
      ;;
		--noandroid)
      buildandroid=0
      ;;
		--nowindows)
      buildwindows=0
      ;;
		--nolinux)
      buildlinux=0
      ;;
		--nomac)
      buildmac=0
      ;;
		--wgetlast)
      download_lastres=1
      ;;
		--noremote)
      send_to_remote=0
      ;;
		--noupdate)
      create_update=0
      ;;
    *)
      printf "***************************\n"
      printf "* Error: Invalid argument.*\n"
      printf "***************************\n"
      exit 1
  esac
  shift
done

if [ -x /usr/bin/cmake ]; then
    echo -e "${txtgrn}Check cmake: ${txtblue}found${txtrst}"
else
    echo -e "${txtred}Cmake not found. It need for build game${txtrst}"
		read -r -p "Do you want to install cmake? [Y/n]" response
		response=${response,,} # tolower
		if [[ $response =~ ^(yes|y| ) ]]; then
				echo -e "${txtgrn}Installing ${txtblu}cmake ${txtrst}"
				sudo apt-get install cmake
		fi
fi

if [ -x '/usr/bin/wget' ]; then
    echo -e "${txtgrn}Check wget: ${txtblue}found${txtrst}"
else
    echo -e "${txtred}wget not found. It need for download lastet packages with resources${txtrst}"
		read -r -p "Do you want to install wget? [Y/n]" response
		response=${response,,} # tolower
		if [[ $response =~ ^(yes|y| ) ]]; then
				echo -e "${txtgrn}Installing ${txtblu}cmake ${txtrst}"
				sudo apt-get install wget
		fi
fi

if [ -x '/usr/bin/g++-4.8' ]; then
    echo -e "${txtgrn}Check g++: ${txtblue}found${txtrst}"
else
    echo -e "${txtred}G++ 4.8 compiler not found. It need for build game${txtrst}"
		echo -e "${txtred}Will be installed packages: ${txtblud}g++-4.8 build-essential libasound2-dev sshpass${txtrst}"
		if [ $buildwindows == 1 ]
		then
			echo -e "${txtred}Will be add foreign repositories ppa:ubuntu-toolchain-r/test and ppa:tobydox/mingw-x-precise${txtrst}"	
			echo -e "${txtred}Will be installed packages for windows build: ${txtblud}mingw32-x-binutils mingw32-x-gcc mingw32-x-runtime${txtrst}"
		fi

		read -r -p "Do you want to install developer tools? [Y/n]" response
		response=${response,,} # tolower
		if [[ $response =~ ^(yes|y| ) ]]
		then
				echo -e "${txtgrn}Installing ${txtblu}g++-4.8 ${txtrst}"
				sudo apt-get install g++-4.8 
				
				echo -e "${txtgrn}Installing ${txtblu}build-essential ${txtrst}"
				sudo apt-get install build-essential

				echo -e "${txtgrn}Installing ${txtblu}libasound2-dev ${txtrst}"
				sudo apt-get install libasound2-dev

				echo -e "${txtgrn}Installing ${txtblu}sshpass ${txtrst}"
				sudo apt-get install sshpass

				if [ $buildwindows == 1 ]
				then
					echo -e "${txtgrn}Add repository ppa:ubuntu-toolchain-r/test ${txtrst}"
					sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test

					echo -e "${txtgrn}Add repository ppa:tobydox/mingw-x-precise ${txtrst}"
					sudo add-apt-repository --yes ppa:tobydox/mingw-x-precise

					echo -e "${txtgrn}Updating system ${txtrst}"
					sudo apt-get update
					
					echo -e "${txtgrn}Installing ${txtblu}mingw32-x-binutils ${txtrst}"
					sudo apt-get install -y mingw32-x-binutils 
					
					echo -e "${txtgrn}Installing ${txtblu}mingw32-x-gcc ${txtrst}"
					sudo apt-get install mingw32-x-gcc 
		
					echo -e "${txtgrn}Installing ${txtblu}mingw32-x-runtime ${txtrst}"
					sudo apt-get install mingw32-x-runtime
				fi
		fi
fi

if [ -x '/usr/bin/zip' ]; then
    echo -e "${txtgrn}Check zip: ${txtblue}found${txtrst}"
else
    echo -e "${txtred}Zip not found. It need for pack archive${txtrst}"
		read -r -p "Do you want to install p7zip-full? [Y/n]" response
		response=${response,,} # tolower
		if [[ $response =~ ^(yes|y| ) ]]; then
				echo -e "${txtgrn}Installing ${txtblu}cmake ${txtrst}"
				sudo apt-get install p7zip-full
		fi
fi	    

if [ $buildlinux == 0 ] && [ $buildandroid == 0 ] && [ $buildwindows == 0 ]
then
	echo -e "${txtred}Nothing to build ${txtrst}"
	echo -e "${txtred}Try to set ${txtblue}--android[windows,linux]=1${txtrst}"
	exit
fi

if [ -z "$project" ]
then
    echo -e "${txtred}Project dir is unset. Use current dir as project folder${txtrst}"
    project="."
fi

if [ ! -f "${project}/CMakeLists.txt" ]
then
	ndir=`pwd`
	echo -e "${txtred}Not found CMakeList.txt. Is ${txtblu}${ndir}/${project}/${txtred} project folder?${txtrst}"
	exit
fi

if [ -z "$sf_password" ]
then
   echo -e "${txtred}"
   echo "SF password not set. Try to use system value" 
   sf_password=`echo $CAESARIA_SF_PASSWORD`
	echo -e "${txtrst}"
fi

if [ -z "$sf_folder" ]
then
	echo -e "${txtred}SF folder not set. Try to use system value${txtrst}" 
  sf_folder=`echo $CAESARIA_SF_FOLDER`
fi

if [ -z "$waste_directory" ]
then
	echo -e "${txtred}Waste folder not set. Try to use system value${txtrst}" 
  waste_folder=`echo $CAESARIA_WASTE_FOLDER`	
fi

if [ -z "$sf_folder" ]
then 
    echo -e "${txtred}"
		echo "SF folder not set. Package will be not upload to sf."
		echo "You need to use --sffolder=name@site.com:/path/to/folder/ or set CAESARIA_SF_FOLDER in env"
		echo "like export CAESARIA_SF_FOLDER=name@site.com:/path/to/folder/"
		echo -e "${txtrst}"
	  have_sffolder=0
else
		have_sffolder=1
fi

if [ -z "$sf_password" ]
then 
    echo -e "${txtred}"
		echo "SF password not set. Package will be not upload to sf."
		echo "You need to use --sfpass=password or set CAESARIA_SF_PASSWORD in env"
		echo "like export CAESARIA_SF_PASSWORD=mysfpass"
		echo -e "${txtrst}"
	  have_sfpass=0
else
		have_sfpass=1
fi

if [ -z "$waste_folder" ]
then 
    echo -e "${txtred}"
		echo "Waste folder not set. Package will be not upload to sf."
		echo "You need to use --wastedir=/path/to/dir or set CAESARIA_WASTE_FOLDER in env"
		echo "like export CAESARIA_WASTE_FOLDER=/path/to/dir"
		echo -e "${txtrst}"
	  have_wastefolder=0
else
		have_wastefolder=1
fi

echo -e "${txtgrn}project directory: ${txtblu}$project"
echo -e "${txtgrn}sf password: ${txtblu}*********"
echo -e "${txtgrn}sf folder: ${txtblu}$sf_folder"
echo -e "${txtgrn}have pass for sf: ${txtblu}$have_sfpass"
if [ $buildwindows == 1 ]
then
	echo -e "${txtgrn}Build game for windows: ${txtblu}yes"
else
	echo -e "${txtgrn}Build game for windows: ${txtred}no"
fi

if [ $buildlinux == 1 ]
then
	echo -e "${txtgrn}Build game for linux: ${txtblu}yes"
else
	echo -e "${txtgrn}Build game for linux: ${txtred}no"
fi

if [ $buildandroid == 1 ]
then
	echo -e "${txtgrn}Build game for android: ${txtblu}yes"
else
	echo -e "${txtgrn}Build game for android: ${txtred}no"
fi

if [ $buildmac == 1 ]
then
	echo -e "${txtgrn}Build game for mac: ${txtblu}yes"
else
	echo -e "${txtgrn}Build game for mac: ${txtred}no"
fi

echo -e "${txtgrn}Goto project directory...${txtrst}"
cd $project
projectdir=`pwd`
echo -e "${txtgrn}Project directory is ${txtblu} $projectdir ${txtrst}"

if [ $updaterepo == 1 ]
then
	echo -e "${txtgrn}Try to get last revision from repo...${txtrst}"
	git pull origin master
else
	echo -e "${txtred}Git update disabled from params. Using current revision${txtrst}"
fi
REVISION_NUMBER=`git rev-list HEAD --count`
echo -e "${txtgrn}Revision number is ${txtblu} $REVISION_NUMBER ${txtrst}"

if [ $download_lastres == 1 ]
then
	wget --quiet --continue https://www.dropbox.com/s/fw4mrhzo733xtdm/caesaria_latest_resources.zip
	mkdir $projectdir/../caesaria-test
	unzip -n caesaria_latest_resources.zip -d $projectdir/../caesaria-test
fi

if [ $buildlinux == 1 ]
then
	echo -e "${txtgrn}Reset previous cmake build folder... ${txtblu} $currentdir/build ${txtrst}"
	rm -rf build
	mkdir build 
	cd build

	if [ $debug_build == 1 ]
	then
		echo -e "${txtgrn}Build game in ${txtred}debug${txtgrn} mode ${txtrst}"
		cmake -DCMAKE_BUILD_TYPE=Debug ..
	else
		echo -e "${txtgrn}Build game in ${txtblu}release${txtgrn} mode ${txtrst}"
		cmake -DCMAKE_BUILD_TYPE=Release ..
	fi
	
	echo -e "${txtgrn}Start building... ${txtrst}"
	make -j5

	echo -e "${txtgrn}Goto to artifacts directory... ${txtrst}"
	cd ../caesaria-test
	
	echo -e "${txtgrn}Remove unneed files ${txtblu}stdout.txt libFLAC-8.dll libmikmod-2.dll smpeg.dll${txtrst}"
	rm stdout.txt libFLAC-8.dll libmikmod-2.dll smpeg.dll

	if [ $send_to_remote == 0 ]
	then
		echo -e "${txtgrn}Create package for linux${txtrst}"
		PACKAGE_NAME_LINUX=caesaria_nightly_linux_$REVISION_NUMBER.zip
		zip -r $PACKAGE_NAME_LINUX resources README.md caesaria.linux 

		if [ $have_sfpass == 1 ] && [ $have_sffolder == 1 ]
		then
			echo -e "Host frs.sourceforge.net\n\tStrictHostKeyChecking no\n" >> ~/.ssh/config	
			echo -e "${txtgrn}Upload nigtly build linux to sf.net${textrst}"
			sshpass -p $sf_password scp -v $PACKAGE_NAME_LINUX ${sf_folder}/bin/
		fi

		if [ $have_wastefolder == 1 ]
		then
			echo -e "${txtgrn}Upload nigtly build linux to wasterdir:${txtblu}$waste_folder"
			mv $PACKAGE_NAME_LINUX $waste_folder
		else		
		  echo -e "${txtred}Skip moving package to waste directory"
			rm $PACKAGE_NAME_LINUX
		fi
	fi

fi #build for linux
exit

if [ $buildwindows == 1 ]
then
	maybuildwinx=1 
  
	if [ $maybuildwinx == 1 ]
	then
		cd $projectdir
		echo -e "${txtgrn}Reset previous cmake build folder... ${txtblu} $currentdir/build ${txtrst}"
		rm -rf build
		mkdir build 
		cd build

		if [ $debug_build == 1 ]
		then
			echo -e "${txtgrn}Build game in ${txtred}debug${txtgrn} mode ${txtrst}"
			cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=../xcompile/win32-cross.cmake ..
		else
			echo -e "${txtgrn}Build game in ${txtblu}release${txtgrn} mode ${txtrst}"
			cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../xcompile/win32-cross.cmake ..
		fi

		echo -e "${txtgrn}Start building... ${txtrst}"
		make -j5

		echo -e "${txtgrn}Goto to artifacts directory... ${txtrst}"
		cd ../caesaria-test

		echo -e "${txtgrn}Copy pthread dll to artifacts directory${txtrst}"	
		cp /usr/i686-w64-mingw32/lib/libwinptread-1.dll ./
	
		if [ $send_to_remote == 0 ]
		then
			echo -e "${txtgrn}Create package for windows${txtrst}"
			PACKAGE_NAME_WINDOWS=caesaria_nightly_windows_$REVISION_NUMBER.zip
			zip -r $PACKAGE_NAME_WINDOWS resources README.md caesaria.exe *.dll

			if [ $have_sfpass == 1 ] && [ $have_sffolder == 1 ]
			then
				echo -e "Host frs.sourceforge.net\n\tStrictHostKeyChecking no\n" >> ~/.ssh/config
				echo -e "${txtgrn}Upload nigtly build windows to sf.net${textrst}"
				sshpass -p $sf_password scp -vvv $PACKAGE_NAME_WINDOWS ${sf_folder}/bin/
			fi

			if [ $have_wastefolder == 1 ]
			then
				echo -e "${txtgrn}Upload nigtly build windows to wasterdir:${txtblu}$waste_folder"
				mv $PACKAGE_NAME_WINDOWS $waste_folder
			else		
				echo -e "${txtred}Skip moving package to waste directory"
				rm $PACKAGE_NAME_WINDOWS
			fi
		fi
	else
		echo -e "${txtred}Cant create build for windows${txtrst}"
	fi
fi

if [ $create_update == 1 ]
then
	echo -e "${txtgrn}Goto artifacts folder${txtrst}"
	cd ${projectdir}
	cd ../caesaria-test

	sf_update_folder=${sf_folder}/update/
	echo -e "${txtgrn}Create update package${txtrst}"
	echo -e "${txtgrn}Create ${txtblu}release${txtgrn} fileset${txtrst}"
	./updater.linux --release --version 0.5.$REVISION_NUMBER

	echo -e "${txtgrn}Create ${txtblu}update${txtgrn} fileset${txtrst}"
	./updater.linux --update --version 0.5.$REVISION_NUMBER

	echo -e "${txtgrn}Upload ${txtblu}sounds${txtgrn} to remote update server ${txtrst}"
	sshpass -p $SF_PASSWORD scp -v resources/audio/* ${sf_folder}/update/resources/audio/

	echo -e "${txtgrn}Upload ${txtblu}gfx${txtgrn} to remote update server ${txtrst}"
	sshpass -p $SF_PASSWORD scp -v resources/gfx/* ${sf_folder}/update/resources/gfx/

	echo -e "${txtgrn}Upload ${txtblu}gui models${txtgrn} to remote update server ${txtrst}"
	sshpass -p $SF_PASSWORD scp -v resources/gui/* ${sf_folder}/update/resources/gui/

	echo -e "${txtgrn}Upload ${txtblu}localization${txtgrn} to remote update server ${txtrst}"
	sshpass -p $SF_PASSWORD scp -v resources/locale/* ${sf_folder}/update/resources/locale/

	echo -e "${txtgrn}Upload ${txtblu}maps${txtgrn} to remote update server ${txtrst}"	
	sshpass -p $SF_PASSWORD scp -v resources/maps/* ${sf_folder}/update/resources/maps/

	echo -e "${txtgrn}Upload ${txtblu}missions${txtgrn} to remote update server ${txtrst}"	
	sshpass -p $SF_PASSWORD scp -v resources/missions/* ${sf_folder}/update/resources/missions/

	echo -e "${txtgrn}Upload ${txtblu}videos${txtgrn} to remote update server ${txtrst}"	
	sshpass -p $SF_PASSWORD scp -v resources/smk/* ${sf_folder}/update/resources/smk/

	echo -e "${txtgrn}Upload ${txtblu}tutorial${txtgrn} to remote update server ${txtrst}"	
	sshpass -p $SF_PASSWORD scp -v resources/tutorial/* ${sf_folder}/update/resources/tutorial/

	echo -e "${txtgrn}Upload ${txtblu}game remake models${txtgrn} to remote update server ${txtrst}"	
	sshpass -p $SF_PASSWORD scp -v resources/*.model ${sf_folder}/update/resources/

	echo -e "${txtgrn}Upload ${txtblu}game c3 models${txtgrn} to remote update server ${txtrst}"	
	sshpass -p $SF_PASSWORD scp -v resources/*.c3 ${sf_folder}/update/resources/

	echo -e "${txtgrn}Upload ${txtblu}fonts${txtgrn} to remote update server ${txtrst}"	
	sshpass -p $SF_PASSWORD scp -v resources/*.ttf ${sf_folder}/update/resources/

	echo -e "${txtgrn}Upload ${txtblu}readme${txtgrn} to remote update server ${txtrst}"	
	sshpass -p $SF_PASSWORD scp -v README.md ${sf_folder}/update/

	echo -e "${txtgrn}Upload ${txtblu}binaries${txtgrn} to remote update server ${txtrst}"	
	sshpass -p $SF_PASSWORD scp -v caesaria.* ${sf_folder}/update/

	echo -e "${txtgrn}Upload ${txtblu}updaters${txtgrn} to remote update server ${txtrst}"	
	sshpass -p $SF_PASSWORD scp -v updater.* ${sf_folder}/update/

	echo -e "${txtgrn}Upload ${txtblu}stable info${txtgrn} to remote update server ${txtrst}"	
	sshpass -p $SF_PASSWORD scp -v stable_info.txt ${sf_folder}/update/

	echo -e "${txtgrn}Upload ${txtblu}update info${txtgrn} to remote update server ${txtrst}"	
	sshpass -p $SF_PASSWORD scp -v version_info.txt ${sf_folder}/update/
fi
