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
    a) issue the **--help** option to list the various options available.  For example:
    **twainsave --help** or **twainsave64 --help** from the command-line will list the options available.
    
    b) Note that there is also a Windows-based [help file](https://github.com/dynarithmic/Twainsave/tree/master/help) that explains most of these options.  This help file is periodically updated.

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

# Building TwainSave from source

If you want to build the source code, note that the building of the source is initially set up using [CMake](https://cmake.org/).  The Boost C++ library is required for the build, so the next section clarifies what is done to install the Boost library.

---

## Boost Dependency Handling Changes

TwainSave now supports automatic Boost discovery, download, installation, and reuse through CMake.

Manual Boost installation and environment-variable setup are no longer required, as was the case with previous versions of the `CMakeList.txt` script.

### Automatic Boost Installation

When enabled, CMake can:

```text
Download Boost binaries
→ Install Boost silently
→ Configure include/library paths
→ Generate Visual Studio project files
```

Supported configurations:

* Visual Studio 2019 (MSVC 14.2)
* Visual Studio 2022 (MSVC 14.3)
* Win32
* x64

---

## Shared Boost Installation Layout

Boost installations are now merged into a common installation root to reduce disk usage.

Example:

```text
BoostDeps/
    boost_1_91_0/
        boost/
        lib32-msvc-14.2/
        lib64-msvc-14.2/
        lib32-msvc-14.3/
        lib64-msvc-14.3/
```

This means:

* Boost headers are installed once.
* Documentation is installed once.
* Only compiler-specific libraries are added.

Win32 and x64 Boost libraries coexist in the same directory.

Visual Studio 2019 and Visual Studio 2022 libraries also coexist.

---

## Existing Boost Installation Support

Users may optionally point CMake to an existing Boost installation.

Configuration variable:

```text
TWAIN_EXISTING_BOOST_ROOT
```

Example:

```text
TWAIN_EXISTING_BOOST_ROOT=D:/boost_1_90_0
```

The existing installation must follow the same directory layout as the automatically downloaded Boost installation.

Minimum required layout:

```text
<boost_root>/
    boost/
    lib32-msvc-<toolset>/
    lib64-msvc-<toolset>/
```

Example:

```text
D:/boost_1_90_0/
    boost/
    lib32-msvc-14.3/
    lib64-msvc-14.3/
```

Only the library directory required for the current build must exist.

Examples:

* VS2022 x64 → `lib64-msvc-14.3`
* VS2022 Win32 → `lib32-msvc-14.3`
* VS2019 x64 → `lib64-msvc-14.2`
* VS2019 Win32 → `lib32-msvc-14.2`

If the required library directory is missing, CMake will stop during Configure and report the missing path.

---

## Optional Cleanup

After a successful automatic installation, CMake may optionally remove:

* downloaded Boost installer (`.exe`)
* installer log file

Configuration options:

```text
TWAIN_DELETE_BOOST_INSTALLER_AFTER_INSTALL
TWAIN_DELETE_BOOST_INSTALL_LOG_AFTER_INSTALL
```

This helps reduce disk usage after Boost installation completes.

---

## Notes

* Environment variables are not required.
* Boost include/library paths are written directly into generated Visual Studio project files.
* Reconfiguration is typically only required when changing:

  * Visual Studio version
  * target architecture
  * Boost version
  * Boost installation location

---------
## To-do list

- Implement the **--verboselog** option.


