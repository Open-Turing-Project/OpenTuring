# Microsoft Developer Studio Project File - Name="TLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=TLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TLib.mak" CFG="TLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "TLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TLib - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "p:\turing\tlib\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "TLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TLib___Win32_Debug"
# PROP BASE Intermediate_Dir "TLib___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\obj\Debug\TLib"
# PROP Intermediate_Dir "..\obj\Debug\TLib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "p:\turing\tlib\include" /I "..\..\ready\src" /I "..\..\ready\src\tools" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_CRTDBG_MAP_ALLOC" /D "_WINDOWS" /D "_WINOOT_" /D _WIN32_IE=0x0300 /D "_MEMORY_CHECK" /FR /YX /FD /GZ /c
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

# Name "TLib - Win32 Release"
# Name "TLib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\tlib\main.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TL.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA4AD.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA4CM.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA4DN.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA4DV.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA4ML.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA4MN.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA4MX.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA8AD.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA8CD.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA8CL.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA8CM.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA8CR.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA8DN.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA8DV.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA8FL.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA8GX.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA8LN.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA8MD.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA8ML.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA8MN.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA8MX.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA8QR.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA8RD.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA8RE.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA8SD.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA8SG.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA8SR.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA8SX.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA8T.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA8TD.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA8TR.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA8X.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLA8XP.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLAIMD.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLAIMN.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLAIMX.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLANMD.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLAPII.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLAPRI.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLAPRR.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLARNI.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLARNR.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLARNZ.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLARSC.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLARSR.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLARSZ.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLARZ.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLAV48.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLAV84.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLAV8D.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLAVD8.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLAVES.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLAVFS.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLAVI4.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLAVI8.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLAVN4.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLAVN8.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLAVRS.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLAVS8.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLAVSI.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLAVSN.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLB.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLBALL.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLBDAL.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLBFRC.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLBFRU.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLBNWC.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLBNWU.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLBUNI.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLBUNR.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLBUUN.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLC.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLCALL.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLCCLR.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLCCON.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLCDIF.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLCEQ.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLCGEQ.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLCIN.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLCINT.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLCLEQ.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLCNEQ.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLCUN.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLCXOR.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLdata.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLE.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLEABT.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLECR.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLECRL.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLECRS.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLEDH.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLEH.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLEHE.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLEHX.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLELF.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLEQUIT.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLETR.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLI.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLICL.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIEFR.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIEOF.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIFA.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIFS.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIGC.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIGCB.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIGF.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIGI.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIGIB.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIGK.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIGN.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIGR.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIGS.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIGSS.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIGSW.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIGT.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIOA.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIOF.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIOP.c
# End Source File
# Begin Source File

SOURCE=..\tlib\tlios.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIPC.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIPE.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIPF.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIPI.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIPK.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIPN.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIPR.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIPS.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIRE.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIRER.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLISF.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLISK.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLISKE.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLISS.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLISSI.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLISSO.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLISSS.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLITL.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIWR.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIWRR.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIZ.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLIZC.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLK.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLM.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMCDINI.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMCDSIG.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMCDWT.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMCEMP.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMCFIN.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMCLEN.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMCPINI.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMCPSIG.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMCPWT.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMCRINI.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMCRSIG.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMCRWT.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMCTINI.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMCTSIG.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMCTWT.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMDCTWT.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMDENT.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMDEXT.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMDINI.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMGNEP.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMIPENT.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMIPEXT.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMIPINI.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMRENT.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMREXT.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMRFIN.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMRINI.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLMUDUMP.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLS.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSASN.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSBS.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSBSS.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSBSX.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSBX.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSBXS.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSBXX.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSCAT.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSCSS.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSCSX.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSCTA.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSCXS.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSCXX.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSDEL.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSIND.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSLEN.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSMCC.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSMCS.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSMSC.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSMSS.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSREC.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSRES.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSRPT.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSVCS.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSVES.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSVFS.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSVIS.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSVNS.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSVRS.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSVS8.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSVSI.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLSVSN.c
# End Source File
# Begin Source File

SOURCE=..\tlib\TLX.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\tlib\include\cinterface
# End Source File
# Begin Source File

SOURCE=..\tlib\include\kernelTypes
# End Source File
# Begin Source File

SOURCE=..\tlib\include\memory
# End Source File
# End Group
# End Target
# End Project
