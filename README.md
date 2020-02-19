# twainsave-opensource
Free, open source version of Dynarithmic Software's [TwainSave-OpenSource](http://www.dynarithmic.com/details_twainsave.html) command-line program to access TWAIN devices and convert scanned pages to image files (BMP, JPEG, TIFF, etc).  This version is licensed under the Apache 2.0 license.  

Note that this version of TwainSave currently lacks some features of the commercial version of TwainSave.  We are in the process of adding these features to this version periodically.


# Differences in this version and the commercial version of TwainSave

1) The command-line parameters use the traditional Unix-style double-hyphen option specifier.  So for example, instead of this:
**twainsave-opensource -filename test.bmp**

The option for the open source version will be
**twainsave-opensource --filename test.bmp**

2)  The [32-bit DTWAIN dtwain32u.dll](https://github.com/dynarithmic/twain_library/tree/master/binaries/32bit) along with the resource files [twaininfo.txt](https://github.com/dynarithmic/twain_library/blob/master/binaries/32bit/twaininfo.txt) and [twainresourcestrings_english.txt](https://github.com/dynarithmic/twain_library/blob/master/binaries/32bit/twainresourcestrings_english.txt) must be available on the system PATH or in the same directory as twainsave-opensource.exe

----------

# Building TwainSave from source

If you want to build the source code, the requirements are the same as building the source for the Dynarithmic TWAIN library found [here](https://github.com/dynarithmic/twain_library/tree/master/source).  There is no need to rebuild DTWAIN first, however I would recommend you build DTWAIN first to ensure you have the correct requirements for building TwainSave.    
  
The only addition to the instructions above would be to define the following environment variables before starting Visual Studio:

* DTWAIN_LIBRARY_DIR_32 -- Directory to the 32-bit DTWAIN import libraries.
* DTWAIN_LIBRARY_DIR_64 -- Directory to the 64-bit DTWAIN import libraries.
 

These directories will usually be the same one you will find dtwain32u.lib, dtwain64u.lib, etc.  The building of TwainSave will be using these environment variables to find the correct DTWAIN import libraries during the build process.

----------


## To-do list

- Implement the **-verbose** and **-verboselog** option.
- Implement the **-details** option.  The new version will have a JSON layout. 

