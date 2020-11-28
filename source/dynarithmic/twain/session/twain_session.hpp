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

#ifndef DTWAIN_TWAIN_SESSION_HPP
#define DTWAIN_TWAIN_SESSION_HPP

#include <string>
#include <iterator>
#include <utility>
#include <unordered_map>
#include <vector>
#include <tuple>
#include <chrono>
#include <thread>
#include <type_traits>

#if __cplusplus >= 201703L
#include <optional>
#define optional_type_ std::optional
#define optional_null_ {}
#else
#include <boost/optional.hpp>
#define optional_type_ boost::optional
#define optional_null_ boost::none
#endif

#include <dynarithmic/twain/characteristics/twain_select_dialog.hpp>
#include <dynarithmic/twain/identity/twain_identity.hpp>
#include <dynarithmic/twain/dtwain_twain.hpp>
#include <dynarithmic/twain/characteristics/twain_characteristics.hpp>
#include <dynarithmic/twain/types/twain_listener.hpp>
#include <dynarithmic/twain/logging/twain_logger.hpp>
#include <dynarithmic/twain/logging/logger_callback.hpp>
#include <dynarithmic/twain/logging/error_logger.hpp>
#include <dynarithmic/twain/session/twain_session_base.hpp>
#include <dtwain.h>

#pragma warning( push )  // Stores the current warning state for every warning.
#pragma warning( disable:4996)

namespace dynarithmic
{
    namespace twain
    {
        class twain_session;
        using twain_source_info = twain_identity;

        /**
            Desribes the type returned when twain_session::select_source function is called
         */
        struct source_select_info
        {
            DTWAIN_SOURCE source_handle = nullptr; /**< [out] Low-level source_handle */
            twain_session* session_handle = nullptr; /**< [out] twain_session that opened the source */
            bool creation_status = false;       /**< [out] status indicator */
        };

        struct select_type
        {
            enum { use_dialog = 0 };
            enum { use_name = 1 };
            enum { use_default = 2 };
        };

        inline LRESULT CALLBACK callback_proc(WPARAM wParam, LPARAM lParam, DTWAIN_LONG64 UserData)
        {
            LRESULT retVal = 1;
            auto thisObject = reinterpret_cast<twain_session_base*>(UserData);
            if (thisObject)
            {
                std::for_each(thisObject->get_callback_map().begin(),
                    thisObject->get_callback_map().end(),
                    [&](twain_session_base::callback_map_type::value_type& vt)
                {
                    retVal = static_cast<LRESULT>(vt.second->call_func(wParam, lParam, vt.first));
                }
                );
            }
            return retVal;
        }

        class twain_listener;
        class twain_logger;
        class twain_source;
        class twain_identity;

        template <int N>
        struct source_selector
        {
            enum { value = N };
        };

        /**
            Template instantiation denoting to use the TWAIN Select Source dialog when selecting a TWAIN Data Source.
            This is aliased by dynarithmic::twain::select_usedialog()
         */
        template <>
        struct source_selector<select_type::use_dialog>
        {
            enum { value = 0 };

            static DTWAIN_SOURCE select(twain_select_dialog& dlg)
            {
#ifdef _WIN32
                auto position = dlg.get_position();
                LONG allFlags = 0;
                auto flags = dlg.get_flags();
                for (auto& f : flags)
                {
                    if (f == twain_select_dialog::uselegacy)
                        return API_INSTANCE DTWAIN_SelectSource();
                    allFlags |= static_cast<LONG>(f);
                }
                if (allFlags & static_cast<LONG>(twain_select_dialog::useposition))
                    allFlags &= ~(DTWAIN_DLG_CENTER_SCREEN | DTWAIN_DLG_CENTER);
                API_INSTANCE DTWAIN_SetTwainDialogFont(dlg.get_font());
                return API_INSTANCE DTWAIN_SelectSource2A(dlg.get_parent_window(),
                                            dlg.get_title().c_str(),
                                            position.first, 
                                            position.second,
                                            allFlags); 
                                                                                                  
                #else
                    return API_INSTANCE DTWAIN_SelectSource();
                #endif
            }
        };

        /**
            Template instantiation denoting to select the default TWAIN Data Source
         */
        template <>
        struct source_selector<select_type::use_default>
        {
            enum { value = 1 };
            static DTWAIN_SOURCE select(twain_select_dialog&) { return API_INSTANCE DTWAIN_SelectDefaultSource(); }
        };

        /**
            Template instantiation denoting to select the TWAIN source using the Product Name
         */
        template <>
        struct source_selector<select_type::use_name>
        {
            enum { value = 2 };
            private:
                std::string m_name;
            public:
                source_selector(std::string name /**< [in] Product Name of the TWAIN device to open */) : m_name(std::move(name)) {}
                DTWAIN_SOURCE select(twain_select_dialog&) const { return API_INSTANCE DTWAIN_SelectSourceByNameA(m_name.c_str()); }
        };


        /**
            Template instantiation denoting to use the TWAIN Select Source dialog when selecting a TWAIN Data Source
         */
        using select_usedialog = source_selector<select_type::use_dialog>;
        using select_default = source_selector<select_type::use_default>;
        using select_byname = source_selector<select_type::use_name>;

        /**
            The twain_session class is the main class that allows the startup and stopping of the TWAIN system.<br> 
            This is the main driver class, as no TWAIN interaction can occur without instantiating a twain_session.
            The twain_session class uses RAII (Resource Acquisition Is Initialization) techniques, meaning that when a 
            twain_session object goes out of scope, the destructor for the twain_session will automatically close out the TWAIN session.<br>
            Note that if there are TWAIN devices still active (acquiring images), the twain_session destructor will not return until all 
            the TWAIN sources are closed.  This may or may not be desirable, but currently this is how the twain_session is implemented.

        *   \code {.cpp}
        *   #include <dynarithmic\twain\twain_session.hpp>
        * 
        *   using namespace dynarithmic::twain;
        *   int main()
            {
        *       twain_session session;
        *       auto& tc = session.get_twain_characteristics();
        *       // example, set the language
        *       tc.set_language("english");
        *       //..
        *       //.. Now start the session
        *       if ( session.start() )
        *       {
        *          // TWAIN session is successfully started
        *          //...
        *       } 
        *   } // session will close automatically
        *   \endcode
        *

            Copy Semantics for **twain_session**:
            <ul>
                <li>Move Constructible</li>
                <li>Move Assignable</li>
            </ul>
         */
        class twain_session : public twain_session_base
        {
            public:

        #ifdef DTWAIN_USE_RUNTIME_LOADING
                static void set_dllhandle(HMODULE h) { RuntimeDLL::DTWAIN_API__.InitDTWAINInterface(h); }
        #endif  
    
            private:
                friend twain_source;
                error_logger m_error_logger;

                mutable std::vector<source_basic_info> m_source_cache;

                template <typename SourceSelector>
                DTWAIN_SOURCE select_source_impl(const SourceSelector& selector, twain_select_dialog& dlg, bool bOpen = true)
                {
                    if (m_Handle)
                    {
                        API_INSTANCE DTWAIN_OpenSourcesOnSelect(bOpen ? TRUE : FALSE);
                        auto src = selector.select(dlg);
                        return src;
                    }
                    return nullptr;
                }

                template <typename Container>
                struct CopyTraits1
                {
                    CopyTraits1(Container& c) : ref(c) {}
                    Container& ref;
                    auto getptr() { return std::inserter(ref, ref.end()); }
                };

                template <typename Container>
                struct CopyTraits2
                {
                    CopyTraits2(Container* c) : ref(c) {}
                    Container* ref;
                    auto getptr() { return ref; }
                };

                template <typename Container>
                void get_twain_sources_impl(Container& c) const
                {
                    c.clear();
                    if ( !m_source_cache.empty() )
                    {
                        std::copy(m_source_cache.begin(),
                                    m_source_cache.end(),
                                    std::inserter(c, c.end()));
                        return;
                    }

                    twain_array ta;
                    if (API_INSTANCE DTWAIN_EnumSources(ta.get_array_ptr()))
                    {
                        DTWAIN_SOURCE src;
                        const int nSources = ta.get_count();
                        auto insert_iter = std::inserter(c, c.end());
                        for (auto i = 0; i < nSources; ++i)
                        {
                            twain_source_info tInfo;
                            API_INSTANCE DTWAIN_ArrayGetAt(ta.get_array(), i, &src);
                            char szBuf[256];
                            API_INSTANCE DTWAIN_GetSourceProductNameA(src, szBuf, 255);
                            tInfo.set_product_name(szBuf);
                            API_INSTANCE DTWAIN_GetSourceProductFamilyA(src, szBuf, 255);
                            tInfo.set_product_family(szBuf);
                            API_INSTANCE DTWAIN_GetSourceManufacturerA(src, szBuf, 255);
                            tInfo.set_manufacturer(szBuf);
                            API_INSTANCE DTWAIN_GetSourceVersionInfoA(src, szBuf, 255);
                            tInfo.set_version_info(szBuf);
                            (*insert_iter) = tInfo;
                            ++insert_iter;
                            m_source_cache.push_back(tInfo);
                        }
                    }
                }

                void setup_error_logging()
                {
                    API_INSTANCE DTWAIN_SetErrorCallback64(dynarithmic::twain::error_callback_proc, reinterpret_cast<DTWAIN_LONG64>(this));
                }

                void setup_logging()
                {
                    auto& details = m_twain_characteristics.get_logger_characteristics();
                    const LONG log_destination = static_cast<LONG>(details.get_destination());
                    const LONG log_verbosity = static_cast<LONG>(details.get_verbosity_aslong());
                    API_INSTANCE DTWAIN_SetTwainLogA(log_destination | log_verbosity, details.get_filename().c_str());
                    if ( m_logger_callback.second )
                        API_INSTANCE DTWAIN_SetLoggerCallbackA(dynarithmic::twain::logger_callback_proc, reinterpret_cast<DTWAIN_LONG64>(this));
                }

                void mover(twain_session&& rhs) noexcept
                {
                    m_twain_characteristics = std::move(rhs.m_twain_characteristics);
                    m_bStarted = rhs.m_bStarted;
                    m_Handle = rhs.m_Handle;
                    m_mapcallback = std::move(rhs.m_mapcallback);
                    m_logger_callback = std::move(rhs.m_logger_callback);
                    m_source_cache = std::move(rhs.m_source_cache);
                    API_INSTANCE DTWAIN_SetCallback64(dynarithmic::twain::callback_proc, reinterpret_cast<DTWAIN_LONG64>(this));
                    API_INSTANCE DTWAIN_SetErrorCallback64(dynarithmic::twain::error_callback_proc, reinterpret_cast<DTWAIN_LONG64>(this));
                    rhs.m_Handle = nullptr;
                }

            public:
                using callback_handle = callback_map_type::iterator;
                /** 
                * \hidecallgraph
                * \hidecallergraph
                */
                twain_session(const twain_session&) = delete;
                /**
                * \hidecallgraph
                * \hidecallergraph
                */
                twain_session& operator=(const twain_session&) = delete;
                /**
                * \hidecallgraph
                * \hidecallergraph
                */
                twain_session() = default;
                /**
                * \hidecallgraph
                * \hidecallergraph
                */
                twain_session(twain_session&& rhs) noexcept
                {
                    mover(std::forward<twain_session>(rhs));
                }

                /**
                * \hidecallgraph
                */
                twain_session& operator=(twain_session&& rhs) noexcept
                {
                    mover(std::forward<twain_session>(rhs));
                    return *this;
                }

                /// Destructor --  Automatically shuts down TWAIN Session
                /// 
                /// The destructor effectively calls the stop() function.  
                /// @see stop()
                ~twain_session()
                {
                    try {
                        stop();
                    }
                    catch (...) {}
                }

                /// Test to see if the TWAIN session has been started.
                /// @returns **true** if the TWAIN session has been started, **false** otherwise.
                /// @see operator bool()
                bool started() const noexcept { return m_bStarted; }

                /// Test to see if the TWAIN session has been started.
                /// @returns **true** if the TWAIN session has been started, **false** otherwise.
                /// @see started()
                operator bool() const noexcept { return m_bStarted; }

                /// Starts a TWAIN session by opening the TWAIN Data Source Manager (DSM).
                /// 
                /// When the TWAIN DSM is started, the current set of twain_characteristics are used when initializing the TWAIN session.
                /// @returns **true** if successful, **false** if TWAIN session could not be started.  
                /// @note Only a single TWAIN session can be started per thread.
                /// @see stop() get_twain_characteristics()
                bool start()
                {
                    m_error_logger.clear();
                    m_error_logger.set_maxsize(m_twain_characteristics.get_errorlogger_details().get_maxsize());

                    if (!API_INSTANCE DTWAIN_IsTwainAvailable())
                    {
                        m_error_logger.add_error(DTWAIN_ERR_TWAIN_NOT_INSTALLED);
                        return false;
                    }

                    API_INSTANCE DTWAIN_SetResourcePathA(m_twain_characteristics.get_resource_directory().c_str());
                    if (!API_INSTANCE DTWAIN_IsInitialized())
                    {
                        m_Handle = API_INSTANCE DTWAIN_SysInitialize();
                        if (!m_Handle)
                        {
                            m_error_logger.add_error(DTWAIN_ERR_NOT_INITIALIZED);
                            return false;
                        }

                        API_INSTANCE DTWAIN_SetErrorCallback64(dynarithmic::twain::error_callback_proc, reinterpret_cast<DTWAIN_LONG64>(this));
                        API_INSTANCE DTWAIN_LoadCustomStringResourcesA(m_twain_characteristics.get_language().c_str());

                        auto sz = API_INSTANCE DTWAIN_GetShortVersionStringA(nullptr, 0);
                        std::vector<char> retBuf(sz + 1);
                        API_INSTANCE DTWAIN_GetShortVersionStringA(retBuf.data(), static_cast<LONG>(retBuf.size()));
                        m_short_name = retBuf.data();

                        sz = API_INSTANCE DTWAIN_GetVersionStringA(nullptr, 0);
                        retBuf.resize(sz + 1);
                        API_INSTANCE DTWAIN_GetVersionStringA(retBuf.data(), static_cast<LONG>(retBuf.size()));
                        m_long_name = retBuf.data();

                        retBuf.resize(1024);
                        API_INSTANCE DTWAIN_GetLibraryPathA(retBuf.data(), static_cast<LONG>(retBuf.size()));
                        m_dtwain_path = retBuf.data();

                        if (m_twain_characteristics.get_logger_characteristics().is_enabled())
                            setup_logging();
                        API_INSTANCE DTWAIN_SetTwainDSM(static_cast<LONG>(m_twain_characteristics.get_dsm()));
                        twain_app_info aInfo = m_twain_characteristics.get_app_info();
                        API_INSTANCE DTWAIN_SetAppInfoA(aInfo.get_version_info().c_str(),
                                           aInfo.get_manufacturer().c_str(),
                                           aInfo.get_product_family().c_str(),
                                           aInfo.get_product_name().c_str());
                        std::string searchDir = m_twain_characteristics.get_dsm_user_directory();
                        API_INSTANCE DTWAIN_SetDSMSearchOrderExA(m_twain_characteristics.get_dsm_search_order().c_str(), 
                                                                 searchDir.empty() ? nullptr : searchDir.c_str());
                        API_INSTANCE DTWAIN_SetLanguage(aInfo.get_language());
                        API_INSTANCE DTWAIN_SetCountry(aInfo.get_country());

                        // Temporary directory
                        std::string sDir = m_twain_characteristics.get_temporary_directory();
                        if (!sDir.empty())
                            API_INSTANCE DTWAIN_SetTempFileDirectoryA(sDir.c_str());
                        else
                        {
                            LONG retVal = API_INSTANCE DTWAIN_GetTempFileDirectoryA(NULL, 0);
                            if (retVal > 0)
                            {
                                sDir.resize(retVal);
                                API_INSTANCE DTWAIN_GetTempFileDirectoryA(&sDir[0], retVal);
                                m_twain_characteristics.set_temporary_directory(sDir);
                            }
                        }
                        const bool twainStarted = API_INSTANCE DTWAIN_StartTwainSession(nullptr, nullptr) != 0;
                        if (twainStarted)
                        {
                            auto& app_info = m_twain_characteristics.get_app_info();
                            app_info = *static_cast<TW_IDENTITY*>(API_INSTANCE DTWAIN_GetTwainAppID());

                            auto len = API_INSTANCE DTWAIN_GetDSMFullNameA(static_cast<LONG>(m_twain_characteristics.get_dsm()), nullptr, 0,
                                                              nullptr);
                            if (len > 0)
                            {
                                std::vector<char> szBuffer(len);
                                API_INSTANCE DTWAIN_GetDSMFullNameA(static_cast<LONG>(m_twain_characteristics.get_dsm()), szBuffer.data(), len,
                                    nullptr);
                                m_dsm_path = szBuffer.data();
                            }

                            API_INSTANCE DTWAIN_EnableMsgNotify(TRUE);
                            API_INSTANCE DTWAIN_SetCallback64(callback_proc, reinterpret_cast<DTWAIN_LONG64>(this));
                            m_source_cache.clear();
                            m_bStarted = true;
                            return true;
                        }
                        return false;
                    }
                    return false;
                }

                /// Stops the TWAIN Data Source Manager (DSM).  
                /// 
                /// Once the DSM is stopped, a call to start() must be issued to restart the TWAIN DSM.  
                /// @returns **true** if successful, **false** if unsuccessful
                /// @see start() get_twain_characteristics()
                /// @warning Calling stop() while a device is in the acquisition state will place your program in a busy-wait loop until the device(s) are out of the acquisition state.
                bool stop()
                {
                    using namespace std::chrono_literals;
                    if (m_Handle)
                    {
                        while (API_INSTANCE DTWAIN_IsAcquiring())
                        {
                            std::this_thread::sleep_for(1ms);
                        }
                        API_INSTANCE DTWAIN_SetCallback64(nullptr, 0);
                        API_INSTANCE DTWAIN_SetLoggerCallbackA(nullptr, 0);
                        if (API_INSTANCE DTWAIN_SysDestroy())
                        {
                            m_Handle = nullptr;
                            m_logger_callback = { nullptr, nullptr };
                            m_source_cache.clear();
                            return true;
                        }
                    }
                    return false;
                }

                /// (For advanced TWAIN programmers) Allows low-level TWAIN triplet calls to the TWAIN Data Source Manager.
                /// 
                /// This function is intended for advanced or highly specialized calls to the TWAIN DSM, and is not usually necessary for almost all TWAIN-enabled applications.
                /// @returns The TWAIN return code that the TWAIN triplet is documented to return (for example: TWRC_SUCCESS, TWRC_FAILURE, etc.)
                /// @param[in,out] pSource Source TW_IDENTITY
                /// @param[in,out] pDest Destination TW_IDENTITY
                /// @param[in] dg TWAIN Triplet Data Group (DG)
                /// @param[in] dat TWAIN Triplet Data (DAT)
                /// @param[in] msg TWAIN Triplet Message (MSG)
                /// @param[in,out] pdata TWAIN data (depends on the triplet DG/DAT/MSG)
                /// @see get_twain_id()
                /// @note See the Twain Specification 2.4 for more information on TWAIN triplets.
                /// @warning Do not use this function if you are not highly familiar with the TWAIN API.
                static int call_dsm(TW_IDENTITY* pSource, 
                                    TW_IDENTITY* pDest,  
                                    LONG dg,  
                                    LONG dat, 
                                    LONG msg, 
                                    void* pdata 
                                   )
                {
                    return API_INSTANCE DTWAIN_CallDSMProc(pSource, pDest, dg, dat, msg, pdata);
                }

                /// Returns an error string that describes the error given by **error_number**
                /// 
                /// @param[in] error_number The number of the error.
                /// @returns An error string that describes the error
                /// @see get_last_error() twain_characteristics.get_language()
                /// @note The error string will be in the language specified by twain_characteristics::get_language()
                static std::string get_error_string(int32_t error_number)
                {
                    char sz[1024] = {};
                    API_INSTANCE DTWAIN_GetErrorStringA(error_number, sz, 1024);
                    return sz;
                }

                /// Returns the last error encountered by the underlying DTWAIN library
                /// 
                /// @returns An error number that represents the last error
                /// @see get_error_string()
                static int32_t get_last_error()
                {
                    return API_INSTANCE DTWAIN_GetLastError();
                }

                error_logger& get_error_logger() noexcept { return m_error_logger; }

                /// Returns a short string that identifies the name of the Dynarithmic TWAIN Library version
                /// 
                /// @returns a string that identifies the version of the underlying DTWAIN library in use.
                std::string get_short_version_name() const  noexcept { return m_short_name; }

                /// Returns a long version string that identifies the name of the Dynarithmic TWAIN Library version
                /// 
                /// @returns a long version string that identifies the version of the underlying DTWAIN library in use.
                std::string get_long_version_name() const noexcept { return m_long_name; }

                /// Returns the full path of the TWAIN Data Source Manager in use.
                /// 
                /// @returns string that identifies the path of the TWAIN Data Source Manager
                std::string get_dsm_path() const noexcept { return m_dsm_path; }

                /// Returns the full path of the DTWAIN shared library being utilized.
                /// 
                /// @returns Full path of the DTWAIN shared library being utilized.
                std::string get_dtwain_path() const noexcept { return m_dtwain_path; }

                /// Registers a twain_listener object with a TWAIN source for this TWAIN session.
                /// 
                /// The twain_listener class allows your application to monitor and trap TWAIN events when the acquire is invoked.  
                /// The twain_session keeps track of all the registered listeners.
                /// @param[in] source The twain_source that the listener will be registering the listener.
                /// @param[in] listener The user-defined listener object.
                /// @returns A handle to the registered listener (to be used later to unregister the listener), or optional_null_ if the registration fails
                /// @note Use the returned callback_handle to unregister a twain_listener
                /// @see unregister_listener()
                template <typename Listener>
                optional_type_<callback_handle> register_listener(twain_source& source, const Listener& listener)
                {
                    static_assert(std::is_base_of<twain_listener, Listener>::value == 1, "Listener is not derived from twain_listener");
                    auto iter = m_mapcallback.find(&source);
                    if (iter == m_mapcallback.end())
                    {
                        auto ptr = std::make_unique<Listener>(listener);
                        auto pr = m_mapcallback.insert({ &source, std::move(ptr) });
                        if (pr.second)
                            return pr.first;
                    }
                    return optional_null_;
                }

                /// Removes a twain_listener for this twain_session.
                /// 
                /// @param[in] handle The callback_handle returned by register_listener.
                /// @returns **true** if the twain_listener is successfully removed, **false** otherwise.
                /// @see register_listener()
                bool unregister_listener(callback_handle handle)
                {
                    auto iter = m_mapcallback.find(handle->first);
                    if (iter != m_mapcallback.end())
                        m_mapcallback.erase(handle);
                    return iter != m_mapcallback.end();
                }

                /// Registers a custom logging object derived from twain_logger with this TWAIN session.
                /// 
                /// @param[in] logger custom logger object
                /// @see unregister_custom_logger
                template <typename Logger>
                void register_custom_logger(const Logger& logger)
                {
                    static_assert(std::is_base_of<twain_logger, Logger>::value == 1, "Logger is not derived from twain_logger");
                    auto ptr = std::make_unique<Logger>(logger);
                    m_logger_callback = {this, std::move(ptr)};
                }

                /// Removes custom logger from this TWAIN session
                /// 
                /// @see register_custom_logger
                void unregister_custom_logger()
                {
                    m_logger_callback = { nullptr, nullptr };
                }

                /// Allows logging to be turned on or off during a TWAIN Session.
                /// 
                /// To set the details of the logging setting, use the get_twain_characteristics().get_logger_details() to set the various details.
                /// @note enable_logger() is the only mechanism that can be used to enable or disable logging after a TWAIN
                /// session has started.
                /// @param[in] enable if **true** the logging is enabled, **false**, logging is disabled.
                void enable_logger(bool enable = true)
                {
                    m_twain_characteristics.get_logger_characteristics().enable(enable);
                    if (m_Handle)
                        setup_logging();
                }

                /// Returns the complete object that represents this TWAIN session's identity.
                /// 
                /// @returns reference to the object that identifies this session by TWAIN.
                /// @note Unlike get_twain_id(), get_id() returns an object that describes the TWAIN session's name, language, etc.
                /// @see get_twain_id()
                twain_identity& get_id() { return m_twain_characteristics.get_app_info(); }

                /// Returns the TW_IDENTITY* that represents this TWAIN session.
                /// 
                /// @returns a pointer to the **TW_IDENTITY** that represents this TWAIN session.  
                /// @note The return value can be used in a call to call_dsm().
                /// @see call_dsm() get_id()
                TW_IDENTITY* get_twain_id() { return static_cast<TW_IDENTITY*>(&get_id().get_identity()); }

                /// Returns a container of twain_source_info, which describes each installed TWAIN device.
                /// 
                /// The container defaults to std::vector<twain_source_info>
                /// @returns A container (default is std::vector<twain_source_info>) of twain_source_info, which describes each installed TWAIN device
                template <typename Container = std::vector<twain_source_info>>
                Container get_twain_sources() const
                {
                    Container C;
                    static_assert(std::is_same<typename Container::value_type, twain_source_info>::value == 1,
                        "Container is not of type twain_source_info");
                    if (m_bStarted)
                       get_twain_sources_impl(C);
                    return C;
                }

                /// Selects a TWAIN Device that will be used to acquire images.
                /// 
                /// Allows selection of a TWAIN Source using one of 3 methods, as denoted by the selector parameter:
                /// <ul>
                ///     <li>1. Using the TWAIN Select Source dialog</li>
                ///     <li>2. Select a source by using the product name of the device</li>
                ///     <li>3. Select the default TWAIN Source</li>
                /// </ul>
                /// If no device is selected, the returned source_select_info will have the source_select_info::creation_status set to **false**.
                /// @param[in] selector The type of selection to use (dialog, by product name, or default)
                /// @param[in] open_source If **true**, and a TWAIN device is successfully selected, automatically opens the device for further operations. 
                /// @returns A source_select_info that describes the DTWAIN_SOURCE selected.
                /// @note if the **open_source** parameter is **false**, the program must call the twain_source::open() function.
                /// @see dynarithmic::twain::source_selector<select_type::use_dialog>() dynarithmic::twain::source_selector<select_type::use_name>() dynarithmic::twain::source_selector<select_type::use_default>()
                /// 
                /**
                \code {.cpp}
                 #include <dynarithmic\twain\twain_session.hpp>
                 #include <dynarithmic\twain\twain_source.hpp>
                 using namespace dynarithmic::twain;
                 int main()
                 {
                    twain_session session;
                    if (session.start())
                    {
                       twain_source source = session.select_source(); // select a source and automatically open
                       if ( source.is_selected() )
                       {
                          // Source was selected
                       }
                    }
                } 
                \endcode
                */
                template <typename T=select_usedialog>
                source_select_info select_source(const T& selector = dynarithmic::twain::source_selector<select_type::use_dialog>(), 
                                                 bool open_source = true)
                { 
                    bool isCanceled = false;
                    auto ret = select_source_impl(selector, m_twain_characteristics.get_twain_dialog(), open_source);
                    if (DTWAIN_GetLastError() == DTWAIN_ERR_SOURCESELECTION_CANCELED)
                        isCanceled = true;
                    source_select_info sRet;
                    sRet.source_handle = ret;
                    sRet.session_handle = this;
                    sRet.creation_status = isCanceled;
                    return sRet;
                }

                /** Adds an error value to the error log
                *   
                */
                void log_error(LONG msg /**< [in] Number of the error message */)
                {
                    m_error_logger.add_error(msg);
                }
        };

    }
}
#pragma warning(pop)
#endif
