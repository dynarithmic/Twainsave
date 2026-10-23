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

#include "stdafx.h"
#define NOMINMAX
#include "twainsave_verinfo.h"
#include "twainsave.h"
#include "twainsave_broker.h"
#include "twainsave_opensource.h"
#include "twainsave_app.h"

#define WIDEN2(x) L##x
#define WIDEN(x)  WIDEN2(x)

static std::wstring create_version_error_string(const VersionNumbers& verNumbers, twainsave_app& theApp)
{
    std::wstring wide_retcode_error = theApp.get_resource_string(RETURN_DTWAINDLL_WRONG_VERSION_MSG).second;
    std::wstring ws = dynarithmic::twain::format_percent_args<std::wstring>(wide_retcode_error,
        { verNumbers.DLLName, verNumbers.FileVersion, verNumbers.FileVersionRequired });
    return ws;
}

bool set_console_to_UTF8()
{
    const BOOL outputOK = SetConsoleOutputCP(CP_UTF8);
    const BOOL inputOK = SetConsoleCP(CP_UTF8);

    return outputOK != FALSE && inputOK != FALSE;
}

void WriteUnicodeConsole(const std::wstring& text)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    if (hConsole == INVALID_HANDLE_VALUE || hConsole == nullptr)
        return;

    DWORD written = 0;

    WriteConsoleW(
        hConsole,
        text.c_str(),
        static_cast<DWORD>(text.size()),
        &written,
        nullptr
    );
}

int main(int argc, char *argv[])
{
    set_console_to_UTF8();
    twainsave_app ts_app;
    ts_app.load_custom_resources_from_ini();
    ts_app.load_resources_from_rc();
    bool dllGood = true;
    auto retval = ts_app.parse_options(argc, argv);
    auto& allOptions = ts_app.get_scanner_options();
    ts_app.load_language_strings();

    VersionNumbers verNumbers;
    verNumbers.DLLName = WIDEN(DTWAIN_DLLNAME);
    auto verOk = ts_app.check_dtwaindll_version(verNumbers);
    if (!verOk.first)
    {
        dllGood = false;
        allOptions.set_return_code(RETURN_DTWAINDLL_WRONG_VERSION);
    }
    if (dllGood)
    {
        if (retval.first)
        {
            if (allOptions.m_bStartBroker)
            {
                twainsave_broker broker;
                // Start the broker program
                auto retCode = broker.StartBroker(ts_app);
                allOptions.set_return_code(retCode);
            }
            else
            {
                if (!allOptions.m_strConfigFile.empty())
                    retval = ts_app.parse_config_options(allOptions.m_strConfigFile);
                if (retval.first)
                    ts_app.start_acquisitions();
            }
        }
    }
    auto retcode = allOptions.get_return_code();
    std::wstring retcode_error = ts_app.get_resource_string(retcode).second;
    std::wstring retcode_wide_error;
    if (!allOptions.m_strDTWAINError.empty())
        retcode_wide_error = allOptions.m_strDTWAINError;
    if (allOptions.m_bNoConsole && !allOptions.m_bNoPause)
    {
        ShowWindow(GetConsoleWindow(), SW_SHOW);

        // display a pause message
        std::wstring s = WIDEN("TwainSave returned code: ") + std::to_wstring(retcode);
        std::wstring s2 = WIDEN(": ") + std::wstring(WIDEN(DTWAIN_DLLNAME));
        if (retcode != RETURN_DTWAINDLL_NOT_FOUND)
            s2.clear();
        std::wstring s3;
        if (retcode == RETURN_DTWAINDLL_WRONG_VERSION)
            s3 = create_version_error_string(verNumbers, ts_app);
        std::wstring actual_error = retcode_error;
        if ( !retcode_wide_error.empty() )
        {
            actual_error = retcode_wide_error;
        }
        s += WIDEN(" (") + actual_error + s2 + s3 + WIDEN(")\nPress any key to continue...");
        WriteUnicodeConsole(s);
        DWORD d;
        char buffer[10];
        ReadConsoleA(GetStdHandle(STD_INPUT_HANDLE), buffer, 1, &d, NULL);
    }
    else
    {
		ShowWindow(GetConsoleWindow(), SW_SHOW);
        std::wstring s = L"TwainSave returned code: " + std::to_wstring(retcode);
        std::wstring s2 = L": ";
        s2 += WIDEN(DTWAIN_DLLNAME);
        if (retcode != RETURN_DTWAINDLL_NOT_FOUND)
            s2.clear();
        std::wstring s3;
        if (retcode == RETURN_DTWAINDLL_WRONG_VERSION)
            s3 = create_version_error_string(verNumbers, ts_app);
        std::wstring actual_error = retcode_error;
        if (!retcode_wide_error.empty())
        {
            actual_error = retcode_wide_error;
        }
        s += WIDEN(" (") + actual_error + s2 + s3 + WIDEN(")\nPress any key to continue...");
        WriteUnicodeConsole(s);
    }
    return retcode;
}

