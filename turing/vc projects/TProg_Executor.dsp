# Microsoft Developer Studio Project File - Name="TProg_Executor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=TProg_Executor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TProg_Executor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TProg_Executor.mak" CFG="TProg_Executor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TProg_Executor - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "TProg_Executor - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TProg_Executor - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "TProg_Executor - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TProg_Executor___Win32_Debug"
# PROP BASE Intermediate_Dir "TProg_Executor___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\obj\Debug\TProg_Executor"
# PROP Intermediate_Dir "..\obj\Debug\TProg_Executor"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_CRTDBG_MAP_ALLOC" /D "_WINDOWS" /D "_WINOOT_" /D _WIN32_IE=0x0300 /D "_MEMORY_CHECK" /YX /FD /GZ /c
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

# Name "TProg_Executor - Win32 Release"
# Name "TProg_Executor - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\tprolog\event.c
# End Source File
# Begin Source File

SOURCE=..\src\tprolog\execute.c
# End Source File
# Begin Source File

SOURCE=..\src\tprolog\graphics.c
# End Source File
# Begin Source File

SOURCE=..\src\tprolog\heap.c
# End Source File
# Begin Source File

SOURCE=..\src\tprolog\kernel.c
# End Source File
# Begin Source File

SOURCE=..\src\tprolog\limits.c
# End Source File
# Begin Source File

SOURCE=..\src\tprolog\math.c
# End Source File
# Begin Source File

SOURCE=..\src\tprolog\mt.c
# End Source File
# Begin Source File

SOURCE=..\src\tprolog\run.c
# End Source File
# Begin Source File

SOURCE=..\src\tprolog\star.c
# End Source File
# Begin Source File

SOURCE=..\src\tprolog\string.c
# End Source File
# Begin Source File

SOURCE=..\src\tprolog\system.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\src\tprolog\cinterface
# End Source File
# End Group
# End Target
# End Project
