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
#ifndef DTWAIN_TWAIN_LOGGER_HPP
#define DTWAIN_TWAIN_LOGGER_HPP

#include <string>
#include <dtwain.h>

namespace dynarithmic
{
    namespace twain
    {
        /**
        Base class for use with custom application logging.  The application can derive from this base class, implement the log_message()
        to handle the log message being sent.  To establish the custom logger, call the twain_session::register_custom_logger() function
        */
        class twain_logger
        {
            public:
                virtual ~twain_logger() = default;
                /// Called by this framework for each message logged by internal DTWAIN logging system.
                /// 
                /// The user will override the twain_logger class, and will implement the log_message function.
                /// @param[in] msg The message sent by the Dynarithmic.TWAIN logging system
                virtual void log_message(const char* msg ){}

                /// Logs a custom error message to the logging system
                /// 
                /// This function will pass a message to the logging system.  The message will subsequentally show up in the log with time stamp.
                /// @param[in] msg Message that will be logged to the logging system.
                /// @returns **true** if the message was sent to the logging system successfully, **false** otherwise.
                static bool log_custom_message(std::string msg) { return DTWAIN_LogMessageA(msg.c_str()) ? true : false; }
        };
    }
}
#endif        
