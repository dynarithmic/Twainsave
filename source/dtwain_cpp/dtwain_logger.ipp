/*
This file is part of the Dynarithmic TWAIN Library (DTWAIN).
Copyright (c) 2002-2020 Dynarithmic Software.

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
enum class logger_destination
{
    tofile = DTWAIN_LOG_USEFILE,
    toconsole = DTWAIN_LOG_CONSOLE,
    todebug = DTWAIN_LOG_DEBUGMONITOR,
    tofile_and_console = tofile | toconsole,
    tofile_and_debug = tofile | todebug,
    toconsole_and_debug = tofile | todebug,
    toall = tofile | todebug | toconsole,
    tocustom = 0
};

enum class logger_verbosity
{
    verbose1 = DTWAIN_LOG_CALLSTACK,
    verbose2 = verbose1 | DTWAIN_LOG_DECODE_DEST | DTWAIN_LOG_DECODE_SOURCE,
    verbose3 = verbose2 | DTWAIN_LOG_DECODE_TWMEMREF,
    verbose4 = verbose3 | DTWAIN_LOG_DECODE_TWEVENT,
};

class logger_details
{
    logger_destination m_log_destination = logger_destination::tocustom;
    logger_verbosity m_log_verbosity = logger_verbosity::verbose4;
    std::string m_log_filename;
    bool m_bEnabled = false;
    public:
        logger_details& enable(bool bEnable = true) { m_bEnabled = bEnable; return *this; }
        logger_details& set_destination(logger_destination ld) { m_log_destination = ld; return *this; }
        logger_destination get_destination() const { return m_log_destination; }
        logger_details& set_verbosity(logger_verbosity lv) { m_log_verbosity = lv; return *this; }
        logger_verbosity get_verbosity() const { return m_log_verbosity; }
        logger_details& set_filename(const std::string& filename) { m_log_filename = filename; return *this; }
        std::string get_filename() const { return m_log_filename; }
        bool is_enabled() const { return m_bEnabled; }
};

