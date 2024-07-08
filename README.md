# twainsave
Free, open source version of Dynarithmic Software's [TwainSave](http://www.dynarithmic.com/onlinehelp/twainsave/index.html) command-line program to access TWAIN devices and convert scanned pages to image files (BMP, JPEG, TIFF, etc).  This version is licensed under the Apache 2.0 license.  

Note that this version of TwainSave currently lacks some features of the commercial version of TwainSave.  We are in the process of adding these features to this version periodically.  

The current open source Twainsave version is **1.08**.

----

# Differences in this version and the commercial version of TwainSave

1) The command-line parameters use the traditional Unix-style double-hyphen option specifier.  So for example, instead of this:
**twainsave-opensource -filename test.bmp**

The option for the open source version will be
**twainsave-opensource --filename test.bmp**

2) The **--details** listing differs from the commercial version's [**-details**](http://www.dynarithmic.com/onlinehelp/twainsave/_details.htm) setting in two major ways:<br>
    a) The **--details** is in JSON format.  
    b) The output will by default go to the console instead of having to specify a file.  
  
For item b), to output to a file just requires you to run twainsave-opensource and redirect the **--details** output to a file.  For example:  
 
**twainsave-opensource --details > details.log**

will write the details to the file **details.log**

3) Running **twainsave-opensource.exe** without command-line parameters will default to displaying the TWAIN Select Source dialog box, and if a device is selected, will allow the user to acquire and save the file to a BMP file.  The resulting BMP file will be have a randomly generated file name (using a **.BMP** extension).  

This differs from the commercial version of TwainSave, where the file name is required as a command-line argument.

----------

# Building TwainSave from source

If you want to build the source code, the requirements are the same as building the source for the Dynarithmic TWAIN library found [here](https://github.com/dynarithmic/twain_library_source). 

This means that you will need an installation of the Boost C++ libraries, as well as set environment variables that point to the Boost header files and library files.

There is no need to rebuild DTWAIN first, however I would recommend you build DTWAIN first to ensure you have the correct requirements for building TwainSave.    
  
The only addition to the instructions above would be to define the following environment variables before starting Visual Studio:

* DTWAIN_LIBRARY_DIR_32 -- Directory to the 32-bit DTWAIN import libraries.
* DTWAIN_LIBRARY_DIR_64 -- Directory to the 64-bit DTWAIN import libraries.

These directories will usually be the same one you will find dtwain32u.lib, dtwain64u.lib, etc.  The building of TwainSave will be using these environment variables to find the correct DTWAIN import libraries during the build process.

----------

## To-do list

- Implement the **-verbose** and **-verboselog** option.


