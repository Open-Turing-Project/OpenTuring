# Microsoft Developer Studio Project File - Name="MIO" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=MIO - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MIO.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MIO.mak" CFG="MIO - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MIO - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "MIO - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MIO - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /WX /GX /O2 /I "p:\ready\src" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "MIO - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MIO___Win32_Debug"
# PROP BASE Intermediate_Dir "MIO___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\obj\Debug\MIO"
# PROP Intermediate_Dir "..\obj\Debug\MIO"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /WX /Gm /GX /Zi /Od /I "p:\ready\src" /I "..\..\ready\src" /I "..\..\ready\src\tools" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_CRTDBG_MAP_ALLOC" /D "_WINDOWS" /D "_WINOOT_" /D _WIN32_IE=0x0300 /D "_MEMORY_CHECK" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x1009 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "MIO - Win32 Release"
# Name "MIO - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\mdio.c
# End Source File
# Begin Source File

SOURCE=..\src\mdioconfig.c
# End Source File
# Begin Source File

SOURCE=..\src\mdiodir.c
# End Source File
# Begin Source File

SOURCE=..\src\mdiodlg.c
# End Source File
# Begin Source File

SOURCE=..\src\mdiodraw.c
# End Source File
# Begin Source File

SOURCE=..\src\mdiofile.c
# End Source File
# Begin Source File

SOURCE=..\src\mdiofont.c
# End Source File
# Begin Source File

SOURCE=..\src\mdiogui.c
# End Source File
# Begin Source File

SOURCE=..\src\mdiojoy.c
# End Source File
# Begin Source File

SOURCE=..\src\mdiomusic.c
# End Source File
# Begin Source File

SOURCE=..\src\mdionet.c
# End Source File
# Begin Source File

SOURCE=..\src\mdiopc.c
# End Source File
# Begin Source File

SOURCE=..\src\mdiopic.c
# End Source File
# Begin Source File

SOURCE=..\src\mdiopicgif.c
# End Source File
# Begin Source File

SOURCE=..\src\mdiopicjpeg.c
# End Source File
# Begin Source File

SOURCE=..\src\mdiorgb.c
# End Source File
# Begin Source File

SOURCE=..\src\mdiosprite.c
# End Source File
# Begin Source File

SOURCE=..\src\mdiosys.c
# End Source File
# Begin Source File

SOURCE=..\src\mdiotime.c
# End Source File
# Begin Source File

SOURCE=..\src\mdioutil.c
# End Source File
# Begin Source File

SOURCE=..\src\mdiowin.c
# End Source File
# Begin Source File

SOURCE=..\src\mdiowindlg.c
# End Source File
# Begin Source File

SOURCE=..\src\mdiowingraph.c
# End Source File
# Begin Source File

SOURCE=..\src\mdiowintext.c
# End Source File
# Begin Source File

SOURCE=..\src\mdiowintop.c
# End Source File
# Begin Source File

SOURCE=..\src\mio.c
# End Source File
# Begin Source File

SOURCE=..\src\mioconfig.c
# End Source File
# Begin Source File

SOURCE=..\src\miocrypto.c
# End Source File
# Begin Source File

SOURCE=..\src\miodir.c
# End Source File
# Begin Source File

SOURCE=..\src\miodlg.c
# End Source File
# Begin Source File

SOURCE=..\src\miodraw.c
# End Source File
# Begin Source File

SOURCE=..\src\mioerrmsg.in
# End Source File
# Begin Source File

SOURCE=..\src\mioerror.c
# End Source File
# Begin Source File

SOURCE=..\src\mioerrstr.in
# End Source File
# Begin Source File

SOURCE=..\src\miofile.c
# End Source File
# Begin Source File

SOURCE=..\src\miofont.c
# End Source File
# Begin Source File

SOURCE=..\src\miogui.c
# End Source File
# Begin Source File

SOURCE=..\src\miojoy.c
# End Source File
# Begin Source File

SOURCE=..\src\miolexer.c
# End Source File
# Begin Source File

SOURCE=..\src\miolexread.in
# End Source File
# Begin Source File

SOURCE=..\src\miolexscan.in
# End Source File
# Begin Source File

SOURCE=..\src\miomouse.c
# End Source File
# Begin Source File

SOURCE=..\src\miomusic.c
# End Source File
# Begin Source File

SOURCE=..\src\mionet.c
# End Source File
# Begin Source File

SOURCE=..\src\miopc.c
# End Source File
# Begin Source File

SOURCE=..\src\miopic.c
# End Source File
# Begin Source File

SOURCE=..\src\miorgb.c
# End Source File
# Begin Source File

SOURCE=..\src\miosprite.c
# End Source File
# Begin Source File

SOURCE=..\src\miosys.c
# End Source File
# Begin Source File

SOURCE=..\src\miotext.c
# End Source File
# Begin Source File

SOURCE=..\src\miotime.c
# End Source File
# Begin Source File

SOURCE=..\src\mioview.c
# End Source File
# Begin Source File

SOURCE=..\src\miowin.c
# End Source File
# Begin Source File

SOURCE=..\src\miowindlg.c
# End Source File
# Begin Source File

SOURCE=..\src\miowindow.c
# End Source File
# Begin Source File

SOURCE=..\src\miowingraph.c
# End Source File
# Begin Source File

SOURCE=..\src\miowintext.c
# End Source File
# Begin Source File

SOURCE=..\src\miowrapper.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\ready\src\ederr.h
# End Source File
# Begin Source File

SOURCE=..\src\edint.h
# End Source File
# Begin Source File

SOURCE=..\ready\src\edtext.h
# End Source File
# Begin Source File

SOURCE=..\src\fileman.h
# End Source File
# Begin Source File

SOURCE=..\src\libungif\gif_lib.h
# End Source File
# Begin Source File

SOURCE="..\src\jpeg-6b\jconfig.h"
# End Source File
# Begin Source File

SOURCE="..\src\jpeg-6b\jerror.h"
# End Source File
# Begin Source File

SOURCE="..\src\jpeg-6b\jmorecfg.h"
# End Source File
# Begin Source File

SOURCE="..\src\jpeg-6b\jpeglib.h"
# End Source File
# Begin Source File

SOURCE=..\src\language.h
# End Source File
# Begin Source File

SOURCE=..\src\levent.h
# End Source File
# Begin Source File

SOURCE=..\src\mdio.h
# End Source File
# Begin Source File

SOURCE=..\src\mdioconfig.h
# End Source File
# Begin Source File

SOURCE=..\src\mdiodir.h
# End Source File
# Begin Source File

SOURCE=..\src\mdiodlg.h
# End Source File
# Begin Source File

SOURCE=..\src\mdiodraw.h
# End Source File
# Begin Source File

SOURCE=..\src\mdiofile.h
# End Source File
# Begin Source File

SOURCE=..\src\mdiofont.h
# End Source File
# Begin Source File

SOURCE=..\src\mdiogui.h
# End Source File
# Begin Source File

SOURCE=..\src\mdiojoy.h
# End Source File
# Begin Source File

SOURCE=..\src\mdiomusic.h
# End Source File
# Begin Source File

SOURCE=..\src\mdionet.h
# End Source File
# Begin Source File

SOURCE=..\src\mdiopc.h
# End Source File
# Begin Source File

SOURCE=..\src\mdiopic.h
# End Source File
# Begin Source File

SOURCE=..\src\mdiopicgif.h
# End Source File
# Begin Source File

SOURCE=..\src\mdiopicjpeg.h
# End Source File
# Begin Source File

SOURCE=..\src\mdiorgb.h
# End Source File
# Begin Source File

SOURCE=..\src\mdiosprite.h
# End Source File
# Begin Source File

SOURCE=..\src\mdiosys.h
# End Source File
# Begin Source File

SOURCE=..\src\mdiotime.h
# End Source File
# Begin Source File

SOURCE=..\src\mdioutil.h
# End Source File
# Begin Source File

SOURCE=..\src\mdiowin.h
# End Source File
# Begin Source File

SOURCE=..\src\mdiowindlg.h
# End Source File
# Begin Source File

SOURCE=..\src\mdiowingraph.h
# End Source File
# Begin Source File

SOURCE=..\src\mdiowintext.h
# End Source File
# Begin Source File

SOURCE=..\src\mdiowintop.h
# End Source File
# Begin Source File

SOURCE=..\src\mio.h
# End Source File
# Begin Source File

SOURCE=..\src\miocommands.h
# End Source File
# Begin Source File

SOURCE=..\src\mioconfig.h
# End Source File
# Begin Source File

SOURCE=..\src\miocrypto.h
# End Source File
# Begin Source File

SOURCE=..\src\miodialogs.h
# End Source File
# Begin Source File

SOURCE=..\src\miodir.h
# End Source File
# Begin Source File

SOURCE=..\src\miodlg.h
# End Source File
# Begin Source File

SOURCE=..\src\miodraw.h
# End Source File
# Begin Source File

SOURCE=..\src\mioerr.h
# End Source File
# Begin Source File

SOURCE=..\src\mioerror.h
# End Source File
# Begin Source File

SOURCE=..\src\miofile.h
# End Source File
# Begin Source File

SOURCE=..\src\miofont.h
# End Source File
# Begin Source File

SOURCE=..\src\miogui.h
# End Source File
# Begin Source File

SOURCE=..\src\miojoy.h
# End Source File
# Begin Source File

SOURCE=..\src\miolexer.h
# End Source File
# Begin Source File

SOURCE=..\src\miomouse.h
# End Source File
# Begin Source File

SOURCE=..\src\miomusic.h
# End Source File
# Begin Source File

SOURCE=..\src\mionet.h
# End Source File
# Begin Source File

SOURCE=..\src\miopc.h
# End Source File
# Begin Source File

SOURCE=..\src\miopic.h
# End Source File
# Begin Source File

SOURCE=..\src\miorgb.h
# End Source File
# Begin Source File

SOURCE=..\src\miosprite.h
# End Source File
# Begin Source File

SOURCE=..\src\miosys.h
# End Source File
# Begin Source File

SOURCE=..\src\miotext.h
# End Source File
# Begin Source File

SOURCE=..\src\miotime.h
# End Source File
# Begin Source File

SOURCE=..\src\miotypes.h
# End Source File
# Begin Source File

SOURCE=..\src\mioview.h
# End Source File
# Begin Source File

SOURCE=..\src\miowin.h
# End Source File
# Begin Source File

SOURCE=..\src\miowindlg.h
# End Source File
# Begin Source File

SOURCE=..\src\miowindow.h
# End Source File
# Begin Source File

SOURCE=..\src\miowingraph.h
# End Source File
# Begin Source File

SOURCE=..\src\miowintext.h
# End Source File
# Begin Source File

SOURCE=..\src\turing.h
# End Source File
# End Group
# End Target
# End Project
