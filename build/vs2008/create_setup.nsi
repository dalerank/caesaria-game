; Надо для своих диалогов
!include nsDialogs.nsh
!include LogicLib.nsh

!define VERSION "ver.01_05_2013"


XPStyle on

VIAddVersionKey "ProductName" "OpenCaesar3"
VIAddVersionKey "Comments" ""
VIAddVersionKey "CompanyName" ""
VIAddVersionKey "LegalTrademarks" ""
VIAddVersionKey "LegalCopyright" "©"
VIAddVersionKey "FileDescription" "OpenCaesar3"
VIAddVersionKey "FileVersion" ${VERSION}
VIProductVersion "0.2013.1.5"

; The name of the installer
OutFile opencaesar3-${VERSION}-installer.exe
Caption "OpenCaesar3 Setup ${VERSION}"


; The default installation directory
InstallDir "C:\Games\OpenCaesar3"

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\OpenCaesar3" "OpenCaesar3"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------

; Pages

Page components
Page directory
Page instfiles
Page custom EndPage

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

Function EndPage
	ExecShell "open" "$INSTDIR\readme.txt"
	nsDialogs::Create 1018
	Pop $0
	
	  WriteRegStr HKLM "SOFTWARE\OpenCaesar3" "Install_Dir" "$INSTDIR"
  
	; Write the uninstall keys for Windows
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenCaesar3" "DisplayName" "OpenCaesar3"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenCaesar3" "UninstallString" '"$INSTDIR\uninstall.exe"'
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenCaesar3" "NoModify" 1
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenCaesar3" "NoRepair" 1
    WriteUninstaller "uninstall.exe"
  
	${NSD_CreateLabel} 0 30 100% 20 "Done. May close now"
	Pop $0
	
	nsDialogs::Show
FunctionEnd

Var localization
Var resourcesn 
Var sgReader 
 
Section "OpenCaesar3"
	SectionIn RO
	
	CreateDirectory $INSTDIR
	CreateDirectory $INSTDIR\fr
	CreateDirectory $INSTDIR\resources
	
	StrCpy $localization $INSTDIR\fr\LC_MESSAGES
	CreateDirectory $localization
	SetOutPath $localization
	File /r /x .svn "..\..\locales\*.*"
			
	StrCpy $resourcesn $INSTDIR\resources\maps
	CreateDirectory $resourcesn
	SetOutPath $resourcesn
	File /r /x .svn "..\..\resources\maps\*.*"
		
	StrCpy $resourcesn $INSTDIR\resources\pics
	CreateDirectory $resourcesn
	SetOutPath $resourcesn
	File /r /x .svn "..\..\resources\pics\*.*"
	
	StrCpy $sgReader $INSTDIR\sgReader
	CreateDirectory $sgReader
	SetOutPath $sgReader
			
	File /r /x .svn /x .exe "..\..\sgReader\*.*"
		
	SetOutPath $INSTDIR
			
	;File "bin\release_i17\nrpgui_demo.exe"		
	File "..\..\application\*.*"
	File "release\opencaesar3.exe"
		
SectionEnd

Function .onInit
	# the plugins dir is automatically deleted when the installer exits
	#InitPluginsDir
	#File /oname=$PLUGINSDIR\splash.bmp "logo.bmp"
	#optional
	#File /oname=$PLUGINSDIR\splash.wav "C:\myprog\sound.wav"

	#splash::show 2000 $PLUGINSDIR\splash

	#Pop $0 ; $0 has '1' if the user closed the splash screen early,
	; '0' if everything closed normally, and '-1' if some error occurred.
FunctionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenCaesar3"
  DeleteRegKey HKLM SOFTWARE\OpenCaesar3

  ; Remove directories used
  RMDir "$INSTDIR"

SectionEnd