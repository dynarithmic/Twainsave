# TwainSave
TwainSave is a free, open source version of Dynarithmic Software's [TwainSave](http://www.dynarithmic.com/onlinehelp5/twainsave_opensource/Introduction.html) command-line program to access TWAIN devices and convert scanned pages to image files (BMP, JPEG, TIFF, etc).  This version is licensed under the Apache 2.0 license.  

Close to all of the functionality that is in the commercial version of TwainSave is implemented in the open source version.  

Note that there are two copies of the TwainSave executable files for each architecture and TWAIN device  (32-bit and 64-bit).  To access 32-bit TWAIN devices, you must run the 32-bit version of TwainSave, and similarly, to access 64-bit TWAIN devices, you must run the 64-bit version of TwainSave.

In the <a href="https://github.com/dynarithmic/twainsave/tree/master/binaries/32-bit" target="_blank">32-bit version of TwainSave</a>, you will see **twainsave.exe**, and **twainsave-opensource.exe**, and for the <a href="https://github.com/dynarithmic/twainsave/tree/master/binaries/64-bit" target="_blank">64-bit version of TwainSave</a>, the executable files are **twainsave-opensource.exe** and **twainsave64.exe**.  

# Getting started
There currently isn't an installation package, so the installation will be manual (but very simple).

To get started, 

1) For 32-bit TwainSave, unpack the <a href="https://github.com/dynarithmic/TwainSave/tree/master/binaries/32-bit" target="_blank">twainsave32.zip</a> file to an empty folder on your computer.
2) You can either run twainsave.exe directly from that folder, or set a PATH to the folder and run twainsave.exe.

3) For <a href="https://github.com/dynarithmic/TwainSave/tree/master/binaries/64-bit" target="_blank">64-bit TwainSave</a>, create another empty folder and follow steps 1) and 2) by opening the twainsave64.zip file.  You can also set a PATH to this folder, but be aware to run **twainsave64.exe** and not simply **twainsave.exe** if you have set a path to both the 32-bit and 64-bit folders.

4) If when running TwainSave, you receive an error stating that **VCRUNTIME140.dll and MSVCP140.dll are missing**, you will need to install the <a href="https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170" target="_blank">Visual C++ Redistributables</a> for <a href="https://aka.ms/vs/17/release/vc_redist.x86.exe" target="_blank">X86</a> if you are running the 32-bit TwainSave, and 
<a href="https://aka.ms/vs/17/release/vc_redist.x64.exe" target="_blank">X64</a> if you will be running the 64-bit version of TwainSave.

The 32-bit **twainsave.exe** is exactly the same as the 32-bit **twainsave-opensource.exe**, likewise the 64-bit **twainsave64.exe** is exactly the same executable as the 64 bit **twainsave-opensource.exe**.  The reason for the similar files is that the original help documentation referred to **twainsave.exe** and **twainsave64.exe** and not **twainsave-opensource.exe**.  

----
The current open source TwainSave version is **1.0.11**.

----

# Differences in this version and the commercial version of TwainSave

1) The command-line parameters use the traditional Unix-style double-hyphen option specifier.  So for example, instead of this:
**twainsave -filename test.bmp**

The option for the open source version will be
**twainsave --filename test.bmp**

2) The **--details** listing differs from the commercial version's [**-details**](http://www.dynarithmic.com/onlinehelp/twainsave/_details.htm) setting in two major ways:<br>
    a) The **--details** is in JSON format.  
    b) The output will by default go to the console instead of having to specify a file.  
  
For item b), to output to a file just requires you to run twainsave-opensource and redirect the **--details** output to a file.  For example:  
 
**twainsave --details > details.log**

will write the details to the file **details.log**

3) The **--thumbnail** option has been renamed to **--scale**.

3) Running **twainsave.exe** without command-line parameters will default to displaying the TWAIN Select Source dialog box.  If a device is selected, will allow the user to acquire and save the file to a BMP file.  The resulting BMP file will be have a randomly generated file name (using a **.BMP** extension).  

4) Running **twainsave.exe** without a --filename setting, but with a --filetype setting: If a device is selected, the name of the file will be randomly generated, with a file extension that matches the file type specified.

   For example:
   **twainsave --filetype pdf**
   will result in a file created with a random name, with a **.pdf** extension.
   
6)   Note that the random names will have as the filename in a 128-bit GUID format.  For example:

    6b29fc40-ca47-1067-b31d-00dd010662da.pdf

   would be a randomly generated pdf file.
   

----------

# Building TwainSave from source

If you want to build the source code, the requirements are the same as building the source for the Dynarithmic TWAIN library found [here](https://github.com/dynarithmic/twain_library_source).  

1) Note that the building of the source is initially set up using [CMake](https://cmake.org/).  You can use CMake or CMake-GUI to configure and generate the Visual Studio project files.  Note that you will need either Visual Studio 2019 or Visual Studio 2022 to build the source.

2) You will need an installation of the Boost C++ library, version 1.70 or greater (see the link to building DTWAIN for more information).

3) The environment variables used by the build process are as follows:

```plaintext
BOOST_INCLUDE_DIR
BOOST_LIBRARY_DIR_V142_64
BOOST_LIBRARY_DIR_V142_32
BOOST_LIBRARY_DIR_V143_64
BOOST_LIBRARY_DIR_V143_32
DTWAIN_LIBRARY_DIR
```
The **DTWAIN_LIBRARY_DIR** is the directory where **dtwain32u.lib** and **dtwain64u.lib** are located.  

In addition, you must have the import libraries **dtwain32u.lib** and **dtwain64u.lib** available.  These libraries can be found in the DTWAIN binary folders, in the **release_libraries.zip** archive in both the 
[32-bit](https://github.com/dynarithmic/twain_library/tree/master/binaries/32bit) and [64-bit](https://github.com/dynarithmic/twain_library/tree/master/binaries/64bit) folders.

There is no need to rebuild DTWAIN first, however I would recommend you build DTWAIN first to ensure you have the correct requirements for building TwainSave.    


---------
## To-do list

- Implement the **--verboselog** option.


