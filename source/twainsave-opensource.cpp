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

int main(int argc, char *argv[])
{
    twainsave_app ts_app;
    ts_app.load_custom_resources_from_ini();
    auto retval = ts_app.parse_options(argc, argv);
	auto& allOptions = ts_app.get_scanner_options();

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
    auto retcode = allOptions.get_return_code();
    if (allOptions.m_bNoConsole && !allOptions.m_bNoPause)
    {
        ShowWindow(GetConsoleWindow(), SW_SHOW);
        // display a pause message
        std::string s = "TwainSave returned code: " + std::to_string(retcode);
        s += " (" + allOptions.m_ReturnCodesMap[retcode] + ")\nPress any key to continue...";
        DWORD d;
        WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), s.c_str(), static_cast<DWORD>(s.size()), &d, nullptr);
        char buffer[10];
        ReadConsoleA(GetStdHandle(STD_INPUT_HANDLE), buffer, 1, &d, NULL);
    }
    else
    {
		ShowWindow(GetConsoleWindow(), SW_SHOW);
        std::string s = "TwainSave returned code: " + std::to_string(retcode);
        s += " (" + allOptions.m_ReturnCodesMap[retcode] + ")";
        std::cout << s;
    }
    return retcode;
}

