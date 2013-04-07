# Microsoft Developer Studio Project File - Name="Moondust" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Moondust - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Moondust.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Moondust.mak" CFG="Moondust - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Moondust - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Moondust - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Moondust - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\SDL\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib SDL.lib sdlmain.lib /nologo /subsystem:console /machine:I386 /libpath:"..\SDL\lib\\"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "Moondust - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\SDL\include" /I ".\fastsid\\" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /FD /GZ /QIfist /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib SDL.lib sdlmain.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"..\SDL\lib\\"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "Moondust - Win32 Release"
# Name "Moondust - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\audio.c
# End Source File
# Begin Source File

SOURCE=.\binary.asm

!IF  "$(CFG)" == "Moondust - Win32 Release"

# Begin Custom Build
IntDir=.\Release
InputPath=.\binary.asm
InputName=binary

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasmw -t -f  win32 -o$(IntDir)\$(InputName).obj -Xvc $(InputName).asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Moondust - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
IntDir=.\Debug
InputPath=.\binary.asm
InputName=binary

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasmw -t -f  win32 -o$(IntDir)\$(InputName).obj -Xvc $(InputName).asm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\effects.c
# End Source File
# Begin Source File

SOURCE=.\fastsid.c
# End Source File
# Begin Source File

SOURCE=.\Moondust.c

!IF  "$(CFG)" == "Moondust - Win32 Release"

!ELSEIF  "$(CFG)" == "Moondust - Win32 Debug"

# ADD CPP /w /W0

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\print.c

!IF  "$(CFG)" == "Moondust - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Moondust - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rand.c
# End Source File
# Begin Source File

SOURCE=.\seed_smear.c
# End Source File
# Begin Source File

SOURCE=.\Ships.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\fastsid\fastsid.h
# End Source File
# Begin Source File

SOURCE=.\gfx.h
# End Source File
# Begin Source File

SOURCE=.\moondust.h
# End Source File
# Begin Source File

SOURCE=.\motion_data.h
# End Source File
# Begin Source File

SOURCE=.\sid.h
# End Source File
# Begin Source File

SOURCE=.\sound.h
# End Source File
# Begin Source File

SOURCE=.\types.h
# End Source File
# Begin Source File

SOURCE=.\unused.h
# End Source File
# Begin Source File

SOURCE=.\vice.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\resource.rc
# End Source File
# End Group
# End Target
# End Project
