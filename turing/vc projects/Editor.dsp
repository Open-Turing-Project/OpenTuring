# Microsoft Developer Studio Project File - Name="Editor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Editor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Editor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Editor.mak" CFG="Editor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Editor - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Editor - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Editor - Win32 Release"

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

!ELSEIF  "$(CFG)" == "Editor - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Editor___Win32_Debug"
# PROP BASE Intermediate_Dir "Editor___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\obj\Debug\Editor"
# PROP Intermediate_Dir "..\obj\Debug\Editor"
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

# Name "Editor - Win32 Release"
# Name "Editor - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\ready\src\ed.c
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edbbar.c
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edcrash.c
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\eddde.c
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\eddisp.c
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\ederr.c
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edfail.c
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edfalse.c
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edfile.c
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edglob.c
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edgui.c
# End Source File
# Begin Source File

SOURCE=..\src\edint.c
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edjava.c
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edkeywords.in
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edlog.c
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edmenu.c
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edprint.c
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edprog.c
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edprop.c
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edrec.c
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edreg.c
# End Source File
# Begin Source File

SOURCE=..\src\edrun.c
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edsearch.c
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edsplash.c
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edstat.c
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edtext.c
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edtur.c
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edwin.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\ready\src\ed.h
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edbbar.h
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edcrash.h
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\eddde.h
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\eddisp.h
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\ederr.h
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edfail.h
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edfalse.h
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edfile.h
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edglob.h
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edgui.h
# End Source File
# Begin Source File

SOURCE=.\src\edint.h
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edjava.h
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edlog.h
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edmenu.h
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edprint.h
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edprog.h
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edprop.h
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edrec.h
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edreg.h
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edrun.h
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edsearch.h
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edsplash.h
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edstat.h
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edtext.h
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edtur.h
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\edwin.h
# End Source File
# End Group
# End Target
# End Project
