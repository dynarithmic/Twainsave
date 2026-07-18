# TwainSave
TwainSave is a free, open source version of Dynarithmic Software's [TwainSave](http://www.dynarithmic.com/onlinehelp5/twainsave_opensource/Introduction.html) command-line program to access TWAIN devices and convert scanned pages to image files (BMP, JPEG, TIFF, etc).  This version is licensed under the Apache 2.0 license.  

Close to all of the functionality that is in the commercial version of TwainSave is implemented in the open source version.  

Note that there are two copies of the TwainSave executable files for each architecture and TWAIN device  (32-bit and 64-bit).  To access 32-bit TWAIN devices, you must run the 32-bit version of TwainSave, and similarly, to access 64-bit TWAIN devices, you must run the 64-bit version of TwainSave.

# Getting started
There currently isn't an installation package, so the installation will be manual (but very simple).

To get started, 

1) For 32-bit TwainSave, unzip the [32-bit binaries file](https://github.com/dynarithmic/Twainsave/releases/latest/download/twainsave-binaries-x86.zip) to an empty folder on your computer.
2) You can either run **twainsave.exe** directly from that folder, or set a PATH to the folder and run **twainsave.exe**.

3) For 64-bit TwainSave, unzip the [64-bit binaries file](https://github.com/dynarithmic/Twainsave/releases/latest/download/twainsave-binaries-x64.zip) to another empty folder.  You can also set a PATH to this folder, but be aware to run **twainsave64.exe** and not simply **twainsave.exe** if you have set a path to both the 32-bit and 64-bit folders.

4) If when running TwainSave, you receive an error stating that **VCRUNTIME140.dll and MSVCP140.dll are missing**, you will need to install the <a href="https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170" target="_blank">Visual C++ Redistributables</a> for <a href="https://aka.ms/vs/17/release/vc_redist.x86.exe" target="_blank">X86</a> if you are running the 32-bit TwainSave, and 
<a href="https://aka.ms/vs/17/release/vc_redist.x64.exe" target="_blank">X64</a> if you will be running the 64-bit version of TwainSave.

The 32-bit **twainsave.exe** is exactly the same as the 32-bit **twainsave-opensource.exe**, likewise the 64-bit **twainsave64.exe** is exactly the same executable as the 64 bit **twainsave-opensource.exe**.  The reason for the similar files is that the original help documentation referred to **twainsave.exe** and **twainsave64.exe** and not **twainsave-opensource.exe**.  

5) To get help for **Twainsave**:
    a) issue the **--help** option to list the various options available.  For example:<br>
    `twainsave --help`<br>
    or<br>
    `twainsave64 --help`<br>

   from the command-line will list the options available.
    
    b) Note that there is also a Windows-based [help file](https://github.com/dynarithmic/Twainsave/tree/master/help) that explains most of these options.  This help file is periodically updated.
    
    c) There is also [online help](https://www.dynarithmic.com/onlinehelp5/twainsave_opensource/Introduction.html).  Note that the online help is also periodically updated.

----
The current open source TwainSave version is **1.4.0**.

----

# Differences in this version and the commercial version of TwainSave

1) The command-line parameters use the traditional Unix-style double-hyphen option specifier.  So for example, instead of this:

    **twainsave -filename test.bmp**

    the option for the open source version will be

    **twainsave --filename test.bmp**

2) The **--details** listing differs from the commercial version's [**-details**](http://www.dynarithmic.com/onlinehelp/twainsave/_details.htm) setting in two major ways:<br>
    a) The **--details** is in JSON format.  
    b) The output will by default go to the console instead of having to specify a file.  
  
For item b), to output to a file just requires you to run twainsave-opensource and redirect the **--details** output to a file.  For example:  
 
**twainsave --details > details.log**

will write the details to the file **details.log**

3) The **--thumbnail** option has been renamed to **--scale**.

4) Running **twainsave.exe** without command-line parameters will default to displaying the TWAIN Select Source dialog box.  If a device is selected, will allow the user to acquire and save the file to a BMP file.  The resulting BMP file will have a randomly generated file name (using a **.BMP** extension).  

4) Running **twainsave.exe** without a **--filename** setting, but with a **--filetype** setting: If a device is selected, the name of the file will be randomly generated, with a file extension that matches the file type specified.

   For example:  

   **twainsave --filetype pdf**
     
    will result in a file created with a random name, using a **.pdf** extension.
   
6)   Note that the random names will have as the filename in a 128-bit GUID format.  For  example:  
  
       **6b29fc40-ca47-1067-b31d-00dd010662da.pdf**

       would be a randomly generated pdf file.

7) The **--filetype** command now supports SVG and SVGZ files being generated.  The new types are **svg** and **svgz** respectively.


 
----------
----------

## Building TwainSave from source

TwainSave uses CMake and Microsoft Visual Studio for building the application from source.

### Requirements

Before building TwainSave, ensure the following software is installed:

#### Microsoft Visual Studio

One of the following Visual Studio versions is required:

* Visual Studio 2019 Community or later
* Visual Studio 2022 Community or later
* Visual Studio 2026 Community or later

Community editions may be downloaded free of charge from Microsoft.

During installation, select the **Desktop Development with C++** workload.

#### CMake

CMake version 3.25.1 or later is required.

Download:

* https://cmake.org/download/

After installation, verify that CMake is available from a command prompt:

```text
cmake --version
```

### Boost Libraries

TwainSave uses the Boost C++ Libraries.

The supplied CMake configuration can automatically download and install the required Boost binaries for the selected Visual Studio version and platform. No separate Boost installation is normally required.

The default Boost download location is:

```text
C:\BoostDeps
```
### Optional
#### DTWAIN Library rebuild
The `dtwain32u.dll` and `dtwain64u.dll` files that are included in the TwainSave installation are satisfactory without having to rebuild those components.  However if desired (usually for in-depth debugging purposes), the `dtwain32u.dll` and `dtwain64u.dll` files can also be rebuilt.  The instructions [here](https://github.com/dynarithmic/twain_library_source#rebuild-source) detail rebuilding of the DTWAIN library.

Note that the TwainSave source code in the master branch is compatible with the DTWAIN library created with the DTWAIN source code found in the [main](https://github.com/dynarithmic/twain_library_source) branch of the source repository.  You should choose the DTWAIN source code in this branch for rebuilding if the DTWAIN libraries will need to be rebuilt.        

Usage of the code in the `development` or other branches in the DTWAIN source repository may yield DTWAIN libraries that could have various incompatibility issues with the current version of TwainSave.

## Building TwainSave

The repository includes CMake presets and batch files that simplify the build process.

#### Build all configurations for a Visual Studio version

To build both 32-bit and 64-bit versions using a specific Visual Studio compiler:

```text
build_all_vs2019.bat
build_all_vs2022.bat
build_all_vs2026.bat
```

Each batch file:

1. Configures the required CMake presets.
2. Downloads and installs Boost automatically if necessary.
3. Builds both Win32 and x64 versions.
4. Produces both Debug and MinSizeRel configurations.

#### Build a single platform

The following batch files build a single platform:

```text
build_vs2019-x32.bat
build_vs2019-x64.bat

build_vs2022-x32.bat
build_vs2022-x64.bat

build_vs2026-x32.bat
build_vs2026-x64.bat
```

Each batch file configures the corresponding CMake preset and builds:

```text
Debug
MinSizeRel
```

### Using CMake Directly

Advanced users may invoke CMake directly.

Examples:

#### Visual Studio 2022 x64

```text
cmake --preset vs2022-x64
cmake --build --preset vs2022-x64-release
```

#### Visual Studio 2022 Win32

```text
cmake --preset vs2022-x32
cmake --build --preset vs2022-x32-release
```

Equivalent presets exist for Visual Studio 2019 and Visual Studio 2026.

### Build Output

After a successful build, the executable files are placed in the selected configuration directory.

For 32-bit builds:

```text
twainsave-opensource.exe
twainsave.exe
```

For 64-bit builds:

```text
twainsave-opensource.exe
twainsave64.exe
```

The `twainsave.exe` and `twainsave64.exe` files are convenience copies of the main executable and exist to preserve compatibility with existing documentation and command-line examples.

### Running TwainSave after building

Please note that you should always run TwainSave using the version of `dtwain32u.dll` or `dtwain64u.dll` that exists in the original TwainSave `binaries` .zip files.  The reason is that these versions of the DTWAIN DLL may not match the current released versions of the DTWAIN library.

As a visual guide, when building TwainSave, diagnostic messages will be outputted to the console as to the version of DTWAIN that TwainSave will be using.  For example:  
  
`Compiling using DTWAIN Version 5.9.3 Build 002 (Release)`

or messages similar to this will appear when rebuilding TwainSave.


### Troubleshooting

#### Boost download fails

If the automatic Boost download fails:

1. Verify your Internet connection.
2. Re-run the build command.
3. Ensure the latest version of CMake is installed.
4. Delete the Boost cache directory if a partially downloaded installer exists:

#### After successfully building TwainSave.exe, the TwainSave program starts up but closes immediately
1. Verify that the correct version of `dtwain32u.dll` or `dtwain64u.dll` is being loaded at runtime.  This can be accomplished by running TwainSave with the following option:  
  
    `twainsave --version`

    This will output the TwainSave version and the version of the DTWAIN DLL being used.  If you have multiple DTWAIN DLL's installed on your system, verify that the DTWAIN DLL used is the correct one that should be used by TwainSave.

2. Verify that the text resources (`twaininfo.txt`, `dtwain32.ini`, for example) are the same as the text resources used in the TwainSave installation and reside in the same directory at the TwainSave executable.
 



```text
C:\BoostDeps
```

Then run the build again.

#### CMake cannot find Visual Studio

Verify that:

* Visual Studio is installed.
* The Desktop Development with C++ workload is installed.
* The selected Visual Studio version matches the batch file or preset being used.

#### Windows SDK errors

If Visual Studio reports missing Windows SDK components, open the Visual Studio Installer and update the Desktop Development with C++ workload to include the Windows SDK.


