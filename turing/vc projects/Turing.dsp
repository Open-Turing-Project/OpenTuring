# Microsoft Developer Studio Project File - Name="Turing" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Turing - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Turing.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Turing.mak" CFG="Turing - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Turing - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Turing - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Turing - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "p:\ready\src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  wininet.lib /nologo /subsystem:windows /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=echo PROBLEM! - You have created the release version of Turing - You want the debug version
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Turing - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Debug"
# PROP Intermediate_Dir "..\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\ready\src" /I "..\..\ready\src\tools" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x1009 /d "_DEBUG" /d "TURING"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libcmtd.lib version.lib winmm.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wininet.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Cmds=del ..\test\turing.exe
PostBuild_Cmds=copy ..\Debug\turing.exe ..\test\turing.exe	echo Copied turing.exe	copy ..\Debug\turing.pdb ..\test\support\bin\turing.pdb	echo Copied turing.pdb
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Turing - Win32 Release"
# Name "Turing - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\..\ready\src\rsrc\breakpoint.cur
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\rsrc\breakpoint.ico
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\rsrc\dbgdir.ico
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\rsrc\dbgfile.ico
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\rsrc\dbgfont.ico
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\rsrc\dbglexer.ico
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\rsrc\dbgnet.ico
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\rsrc\dbgpic.ico
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\rsrc\dbgsprite.ico
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\rsrc\dbgunknown.ico
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\rsrc\dbgwin.ico
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\ed.rc

!IF  "$(CFG)" == "Turing - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Turing - Win32 Debug"

# ADD BASE RSC /l 0x1009 /i "\ready\src" /i "\Documents and Settings\Tom\My Documents\dev\ready\src"
# ADD RSC /l 0x1009 /i "\ready\src" /i "\Documents and Settings\Tom\My Documents\dev\ready\src"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\mio.rc

!IF  "$(CFG)" == "Turing - Win32 Release"

!ELSEIF  "$(CFG)" == "Turing - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\ready\src\rsrc\readyclass.ico
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\rsrc\readyjava.ico
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\rsrc\turing.ico
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\rsrc\turingdem.ico
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\rsrc\turingfile.ico
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\rsrc\turinginclude.ico
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\rsrc\turingrunsm.ico
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\rsrc\turingunit.ico
# End Source File
# Begin Source File

SOURCE=..\..\ready\src\rsrc\two_up.ico
# End Source File
# End Group
# Begin Source File

SOURCE=..\Debug\HTMLHELP.LIB
# End Source File
# End Target
# End Project
