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
#include "dtwain_cpp_interface.hpp"

namespace dynarithmic
{
    namespace twain
    {
        // The general twain system.  An application must create one of these objects before attempting to
        // do any TWAIN work.
        LRESULT CALLBACK twain_session::callback_proc(WPARAM wParam, LPARAM lParam, DTWAIN_LONG64 UserData)
        {
            LRESULT retVal = 1;
            auto thisObject = reinterpret_cast<twain_session*>(UserData);
            if (thisObject)
            {
                std::for_each(thisObject->m_mapcallback.begin(),
                              thisObject->m_mapcallback.end(),
                              [&](const callback_map_type::value_type& vt)
                              {
                                  retVal = static_cast<LRESULT>(vt.second->call_func(wParam, lParam, vt.first));
                              }
                );
            }
            return retVal;
        }

        LRESULT CALLBACK twain_session::logger_callback_proc(const char* msg, DTWAIN_LONG64 UserData)
        {
            const auto thisObject = reinterpret_cast<twain_session*>(UserData);
            if (thisObject)
            {
                logger_callback_type& sesObject = thisObject->m_logger_callback;
                if (thisObject->get_twain_characteristics().get_logger_details().is_enabled() && sesObject.second)
                    thisObject->call_logger(sesObject.second, msg);
            }
            return 1;
        }

        void twain_session::call_logger(twain_logger* plogger, const char* msg)
        {
            plogger->log_message(msg);
        }

        twain_session::twain_session(twain_session&& rhs) noexcept
        {
            tw_characteristics = std::move(rhs.tw_characteristics);
            m_isInitialized = rhs.m_isInitialized;
            m_Handle = rhs.m_Handle;
            m_mapcallback = std::move(rhs.m_mapcallback);
            m_logger_callback = rhs.m_logger_callback;
            m_source_cache = std::move(rhs.m_source_cache);
            DTWAIN_SetCallback64(callback_proc, reinterpret_cast<DTWAIN_LONG64>(this));
            rhs.m_Handle = nullptr;
        }

        twain_session& twain_session::operator=(twain_session&& rhs) noexcept
        {
            tw_characteristics = std::move(rhs.tw_characteristics);
            m_isInitialized = rhs.m_isInitialized;
            m_Handle = rhs.m_Handle;
            m_mapcallback = std::move(rhs.m_mapcallback);
            m_logger_callback = rhs.m_logger_callback;
            m_source_cache = std::move(rhs.m_source_cache);
            DTWAIN_SetCallback64(callback_proc, reinterpret_cast<DTWAIN_LONG64>(this));
            rhs.m_Handle = nullptr;
            return *this;
        }

        twain_session::twain_session(bool bInit) : m_Handle(nullptr)
        {
            if (bInit)
                m_isInitialized = open_dsm();
        }

        twain_session::twain_session(const twain_characteristics& tc, bool bInit) : tw_characteristics(tc), m_Handle(nullptr)
        {
            if (bInit)
                m_isInitialized = open_dsm();
        }

        void twain_session::setup_logging()
        {
            auto& details = tw_characteristics.get_logger_details();
            const LONG log_destination = static_cast<LONG>(details.get_destination());
            const LONG log_verbosity = static_cast<LONG>(details.get_verbosity_aslong());
            DTWAIN_SetTwainLogA(log_destination | log_verbosity, details.get_filename().c_str());
            if ( m_logger_callback.second )
                DTWAIN_SetLoggerCallbackA(logger_callback_proc, reinterpret_cast<DTWAIN_LONG64>(this));
        }

        bool twain_session::open_dsm()
        {
			if (!DTWAIN_IsTwainAvailable())
				return false;
            if (!DTWAIN_IsInitialized())
            {
                m_Handle = DTWAIN_SysInitialize();
                if (!m_Handle)
                    return false;
				
				auto sz = DTWAIN_GetShortVersionStringA(nullptr, 0);
				std::vector<char> retBuf(sz + 1);
				DTWAIN_GetShortVersionStringA(retBuf.data(), retBuf.size());
				short_name = retBuf.data();

                if (tw_characteristics.get_logger_details().is_enabled())
                    setup_logging();
                DTWAIN_SetTwainDSM(static_cast<LONG>(tw_characteristics.get_used_dsm()));
                twain_app_info aInfo = tw_characteristics.get_app_info();
                DTWAIN_SetAppInfoA(aInfo.get_version_info().c_str(),
                                   aInfo.get_manufacturer().c_str(),
                                   aInfo.get_product_family().c_str(),
                                   aInfo.get_product_name().c_str());
                DTWAIN_SetDSMSearchOrder(static_cast<LONG>(tw_characteristics.get_dsm_search_order()));
                DTWAIN_SetLanguage(aInfo.get_language());
                DTWAIN_SetCountry(aInfo.get_country());
                std::string sDir = tw_characteristics.get_temporary_directory();
                if (!sDir.empty())
                    DTWAIN_SetTempFileDirectoryA(sDir.c_str());
                const bool twainStarted = DTWAIN_StartTwainSession(nullptr, nullptr) != 0;
                if (twainStarted)
                {
                    auto& app_info = tw_characteristics.get_app_info();
                    app_info = *static_cast<TW_IDENTITY*>(DTWAIN_GetTwainAppID());

                    auto len = DTWAIN_GetDSMFullNameA(static_cast<LONG>(tw_characteristics.get_used_dsm()), nullptr, 0,
                                                      nullptr);
                    if (len > 0)
                    {
                        std::vector<char> szBuffer(len);
                        DTWAIN_GetDSMFullNameA(static_cast<LONG>(tw_characteristics.get_used_dsm()), szBuffer.data(), len,
                            nullptr);
                        dsm_path = szBuffer.data();
                    }

                    DTWAIN_EnableMsgNotify(TRUE);
                    DTWAIN_SetCallback64(callback_proc, reinterpret_cast<DTWAIN_LONG64>(this));
                    m_source_cache.clear();
                    m_isInitialized = true;
                    return true;
                }
                return false;
            }
            return false;
        }

        twain_identity& twain_session::get_id()
        {
            return tw_characteristics.get_app_info();
        }

        TW_IDENTITY* twain_session::get_twain_id()
        {
            return static_cast<TW_IDENTITY*>(get_id());
        }

        int twain_session::call_dsm(TW_IDENTITY* pSource, TW_IDENTITY* pDest, LONG dg, LONG dat, LONG msg, void* pdata)
        {
            return DTWAIN_CallDSMProc(pSource, pDest, dg, dat, msg, pdata);
        }

        bool twain_session::close_dsm()
        {
            if (m_Handle)
            {
                while (DTWAIN_IsAcquiring())
                    ;
                DTWAIN_SetCallback64(nullptr, 0);
                DTWAIN_SetLoggerCallbackA(nullptr, 0);
                if (DTWAIN_SysDestroy())
                {
                    m_Handle = nullptr;
                    m_logger_callback = { nullptr, nullptr };
                    m_source_cache.clear();
                    return true;
                }
            }
            return false;
        }

        twain_session::~twain_session() { close_dsm(); }

        twain_session::callback_handle twain_session::register_listener(twain_source& src, twain_listener& listener)
        {
            return m_mapcallback.insert({&src, &listener}).first;
        }

        bool twain_session::unregister_listener(callback_handle handle)
        {
            auto iter = m_mapcallback.begin();
            while (iter != m_mapcallback.end())
            {
                if (iter == handle)
                {
                    m_mapcallback.erase(handle);
                    return true;
                }
                ++iter;
            }
            return false;
        }

        void twain_session::register_custom_logger(twain_logger& logger)
        {
            m_logger_callback = {this, &logger};
        }

        void twain_session::unregister_custom_logger()
        {
            m_logger_callback = { nullptr, nullptr };
        }

        void twain_session::enable_logger(bool b)
        {
            tw_characteristics.get_logger_details().enable(b);
            if (m_Handle)
                setup_logging();
        }

		std::string twain_session::get_short_version_name() const 
		{
			return short_name;
        }
    }
};
