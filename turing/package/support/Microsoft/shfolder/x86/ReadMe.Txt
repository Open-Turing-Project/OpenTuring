Readme.txt

Contents:

  ShFolder.exe is a self-installing executable that installs ShFolder.dll on your system. This DLL contains the SHGetFolderPath function, designed to be compatible with Windows 95, Windows NT 4.0 and all later systems. Refer to the Platform SDK for details on SHGetFolderPath.

Installation:

  To install ShFolder.dll, simply run ShFolder.exe. You can use the standard IExpress flags to control the operation of ShFolder.exe:

  /Q - Quiet mode. For a fully quiet installation, use /Q:A.
  /T:<full path> - Specifies a temporary working folder
  /C - Extract files only to the temporary working folder.
       Must be used with /T
  /C:<cmd> - Override install command defined by author
