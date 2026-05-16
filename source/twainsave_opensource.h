/*
This file is part of the Twainsave-opensource version
Copyright (c) 2002-2026 Dynarithmic Software.

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
#ifndef TWAINSAVE_OPENSOURCE_H
#define TWAINSAVE_OPENSOURCE_H

#define OVERWRITE_ALWAYS    1
#define OVERWRITE_EXIT      2
#define OVERWRITE_CREATENEW 3
#define OVERWRITE_PROMPT    4
#define OVERWRITE_PREFIXSTRING "__"

#define RETURN_OK                          0
#define RETURN_FILENAME_MISSING            1
#define RETURN_TWAIN_INIT_ERROR            2
#define RETURN_TWAIN_SOURCE_ERROR          3
#define RETURN_FILESAVE_ERROR              4
#define RETURN_BAD_COMMAND_LINE            5
#define RETURN_TWAIN_SOURCE_CANCEL         6
#define RETURN_TWAIN_ACQUIRE_ERROR         7
#define RETURN_COLORSPACE_NOT_SUPPORTED    8
#define RETURN_CONSOLE_NOT_AVAILABLE       9
#define RETURN_TIMEOUT_REACHED             10
#define RETURN_TWAIN_UIOPEN_ERROR          11
#define RETURN_FILESAVE_FILEEXISTS          12
#define RETURN_MODE2FILETYPE_NOT_SUPPORTED 13
#define RETURN_UIONLY_SUPPORT_ERROR     14  
#define RETURN_COMMANDFILE_NOT_FOUND    15
#define RETURN_COMMANDFILE_OPEN_ERROR   16
#define RETURN_INVALID_FILETYPE         17
#define RETURN_INVALID_MEASUREUNIT      18
#define RETURN_INVALID_PAPERSIZE        19
#define RETURN_INVALID_COLOR            20
#define RETURN_INVALID_ORIENTATION      21
#define RETURN_INVALID_JOBCONTROL       22
#define RETURN_BROKER_ERROR             23
#define RETURN_CODE_LAST (RETURN_BROKER_ERROR + 1)

#define TWAINSAVE_DEFAULT_TITLE "TwainSave - OpenSource"
#define TWAINSAVE_INI_FILE "twainsave.ini"

#endif
