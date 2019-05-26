# Open Turing 1.0
#### Download: https://github.com/downloads/Open-Turing-Project/OpenTuring/package.zip
##### Current lead maintainer: Tristan Hume



Open Turing is an open-source implementation of Turing for Windows.
I (Tristan Hume) acquired the source code from Tom West, the last maintainer of the project.
This version is backwards-compatible with normal turing.

As well as being open-source it is also faster and has more features.
Unfortunately, at the moment many of these features are undocumented.
Look at the "How to Learn About New Features" section for more info.

### Partial change log:
* Up to a 40% speed increase. (depends on program)
* Built in hash maps
* Basic OpenGL 3D. Beta, no input.
* Command line invocation with turing.exe -run file.t
* New splash and logo

### To Get/Download It:
* Download the zip: https://github.com/downloads/Open-Turing-Project/OpenTuring/package.zip
* Or look in the downloads tab
* If you want the development release, click the "master" button and switch to the dev branch, click the zip button, download it and look in the /turing/test folder.

### How to Learn About New Features

Look in the support/predefs folder of the distribution. If you see an interesting looking module name, open the file in turing.

Read the functions in the module. I.E the Hashmap.tu module has the functions for using hashmaps. The GL.tu file has OpenGL functions.

For usage examples (helpful!) look in the Examples/Open Turing folder. These may not exist for every feature.

### Support policy (Or lack thereof)
* If the editor crashes it will show an email. Send panic.log to the email and describe how the crash happened.
* You can also go to the "Issues" tab and post an issue.
* Otherwise, ask for help on http://compsci.ca/

## For Programmers/Developers/Writers/People who want to help out.

### How it works

The Turing environment is written in C with some parts as C compiled from Turing Plus.
Compiling is done through a MS Visual C++ 2010 project (works with express.)

It compiles turing code to bytecode which is executed with a VM. 
If you don't know what that means, go add library functions but don't mess with the language itself.

The main file for the project is /turing/turing.sln this contains the various sub-projects.
The main executable is the "Turing" subproject. The standalone executable project is called "Prolog."
To get a fully working release with your changes you must compile both of these. For testing (if you don't need standalone executables) you only have to compile the "Turing" project.

#### Various other sub-projects:
* MIO - The main library. Includes things like Sprites, Drawing, Input, etc...
* Editor - The IDE. Located in the /ready folder but accessed from the main project.
* TLib - Standard library for compiled turing plus
* Coder/Compiler - Translates from turing code to bytecode.
* Executor - The bytecode VM.
* Interperer - The link between the compiler and executor.

#### Things you should care about:
* To add library functions:
	1. Add a file to the MIO project. Remember to use MIO prefixes, look at one of the existing files (I.E miofont.c)
	2. MIO prefix = platform independent (or mostly independent.) MDIO = Windows only.
	3. Write wrappers for your functions in "miowrapper.c". Look at the other wrappers for examples.
	4. Add your functions to lookup.c (I think it's in the Coder project.) Remember, the functions must be in alphabetical order.
	5. Write a turing module file that uses "external" declarations to declare the functions using the identifiers you used in lookup.c
	6. Put the file in the support/predefs folder and put the file name in support/predefs/predefs.lst
* The main projects you should care about are Editor, MIO, Executor, Compiler and Coder.
* The VS project builds to /turing/test
* I (Tristan) figured all this out with almost no instruction. So you should be OK.

### How to Submit Contributions:
1. Get github! (Seriously, it's awesome.)
2. Look at some turorials on using github
2. Fork the **development** branch
3. Commit changes to your fork.
4. Send a pull request with your changes.
5. If your changes are any good they will be included in the main distribution.

### How Can I Help?

Look in the issues tab. Either fix things or add new features listed there.

### Legal

Open Turing uses the IGL JPEG Library and the libungif library.
