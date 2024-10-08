/*
This file is part of the Twainsave-opensource version
Copyright (c) 2002-2024 Dynarithmic Software.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

FOR ANY PART OF THE COVERED WORK IN WHICH THE COPYRIGHT IS OWNED BY
DYNARITHMIC SOFTWARE. DYNARITHMIC SOFTWARE DISCLAIMS THE WARRANTY OF NON INFRINGEMENT
OF THIRD PARTY RIGHTS.
*/
#ifndef TWAINSAVE_VERINFO_H
#define TWAINSAVE_VERINFO_H

#define TWAINSAVE_MAJOR_VERSION 1
#define TWAINSAVE_MINOR_VERSION 0
#define TWAINSAVE_PATCH_VERSION 11
#define TWAINSAVE_FULL_VERSION "1.0.11"

#define TWAINSAVE_VERINFO_COMMENTS          "TwainSave\0"
#define TWAINSAVE_VERINFO_COMPANYNAME       "Dynarithmic Software\0"
#define TWAINSAVE_VERINFO_LEGALCOPYRIGHT	"Copyright   2020-2024\0"
#define TWAINSAVE_VERINFO_PRODUCTNAME		"TwainSave\0"
#define TWAINSAVE_VERINFO_ORIGINALFILENAME  "twainsave-opensource.exe\0"
#define TWAINSAVE_VERINFO_INTERNALNAME      "twainsave-opensource.exe\0"
#define TWAINSAVE_VERINFO_LEGALTRADEMARKS   "Dynarithmic Software\0"
#define TWAINSAVE_VERINFO_FILEDESCRIPTION    "\0"
#define TWAINSAVE_VERINFO_SPECIALBUILD       "Open Source Version\0"
#define TWAINSAVE_VERINFO_PRIVATEBUILD        "Open Source Version\0"

#define TWAINSAVE_NUMERIC_FILE_VERSION         TWAINSAVE_MAJOR_VERSION,TWAINSAVE_MINOR_VERSION,TWAINSAVE_PATCH_VERSION
#define TWAINSAVE_VERINFO_FILEVERSION          TWAINSAVE_FULL_VERSION
#define TWAINSAVE_NUMERIC_PRODUCT_VERSION      TWAINSAVE_NUMERIC_FILE_VERSION
#define TWAINSAVE_VERINFO_PRODUCTVERSION       TWAINSAVE_VERINFO_SPECIALBUILD
#endif
