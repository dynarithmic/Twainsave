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
template <int N>
struct source_selector
{
    enum { value = N };
};

template <>
struct source_selector<0>
{
    enum { value = 0 };
    static DTWAIN_SOURCE select() { return DTWAIN_SelectSource(); }
};

template <>
struct source_selector<1>
{
    enum { value = 1 };
    static DTWAIN_SOURCE select() { return DTWAIN_SelectDefaultSource(); }
};

template <>
struct source_selector<2>
{
    enum { value = 2 };
    std::string m_name;
    source_selector(std::string name) : m_name(std::move(name)) {}
    DTWAIN_SOURCE select() const { return DTWAIN_SelectSourceByNameA(m_name.c_str()); }
};

using select_prompt = source_selector<0>;
using select_default = source_selector<1>;
using select_byname = source_selector<2>;

// The general twain system.  An application must create one of these objects before attempting to
// do any TWAIN work.
class twain_session
{
    public:
        using source_basic_info = twain_app_info;
    private:
        bool m_isInitialized = false;
        twain_characteristics tw_characteristics;
        std::string dsm_path;
        DTWAIN_HANDLE m_Handle;
        using callback_map_type = std::unordered_map<twain_source*, twain_listener*>;
        using logger_callback_type = std::pair<twain_session*, twain_logger*>;
        logger_callback_type m_logger_callback;
        callback_map_type m_mapcallback;
        mutable std::vector<source_basic_info> m_source_cache;
        void setup_logging();
        static LRESULT CALLBACK callback_proc(WPARAM wParam, LPARAM lParam, DTWAIN_LONG64 UserData);
        static LRESULT CALLBACK logger_callback_proc(const char* msg, DTWAIN_LONG64 UserData);

        template <typename SourceSelector>
        DTWAIN_SOURCE select_source_impl(const SourceSelector& selector, bool bOpen = true)
        {
            if (m_Handle)
            {
                DTWAIN_OpenSourcesOnSelect(bOpen ? TRUE : FALSE);
                auto src = selector.select();
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
            if (DTWAIN_EnumSources(ta.get_array_ptr()))
            {
                DTWAIN_SOURCE src;
                const int nSources = ta.get_count();
                auto insert_iter = std::inserter(c, c.end());
                for (auto i = 0; i < nSources; ++i)
                {
                    twain_source_info tInfo;
                    DTWAIN_ArrayGetAt(ta.get_array(), i, &src);
                    char szBuf[256];
                    DTWAIN_GetSourceProductNameA(src, szBuf, 255);
                    tInfo.set_product_name(szBuf);
                    DTWAIN_GetSourceProductFamilyA(src, szBuf, 255);
                    tInfo.set_product_family(szBuf);
                    DTWAIN_GetSourceManufacturerA(src, szBuf, 255);
                    tInfo.set_manufacturer(szBuf);
                    DTWAIN_GetSourceVersionInfoA(src, szBuf, 255);
                    tInfo.set_version_info(szBuf);
                    (*insert_iter) = tInfo;
                    ++insert_iter;
                    m_source_cache.push_back(tInfo);
                }
            }
        }

    public:
        using callback_handle = callback_map_type::iterator;
        twain_session(const twain_session&) = delete;
        twain_session& operator=(const twain_session&) = delete;
        twain_session(twain_session&& rhs) noexcept;
        twain_session& operator=(twain_session&& rhs) noexcept;
        ~twain_session();
    
        explicit twain_session(bool bInit = false);
        explicit twain_session(const twain_characteristics& tc, bool bInit = false);
        operator bool() const { return m_isInitialized; }
        bool open_dsm();
        void call_logger(twain_logger* plogger, const char* msg);
        std::string get_dsm_path() const { return dsm_path; }
        static int call_dsm(TW_IDENTITY* pSource, TW_IDENTITY* pDest, LONG dg, LONG dat, LONG msg, void* pdata);
        bool close_dsm();

        template <typename Container>
        size_t get_twain_sources(Container& C) const
        {
            static_assert(std::is_same<typename Container::value_type, twain_source_info>::value == 1,
                "Container is not of type twain_source_info");
            if (m_isInitialized)
            {
                get_twain_sources_impl(C);
                return C.size();
            }
            return 0;
        }

        size_t get_twain_sources(twain_source_info* twInfo) const
        {
                if (m_isInitialized)
                {
                        std::vector<twain_session::source_basic_info> vInfo;
                        get_twain_sources(vInfo);
                        if (!twInfo)
                                return vInfo.size();
                        std::copy(vInfo.begin(), vInfo.end(), twInfo);
                        return vInfo.size();
                }
                return 0;
        }

        template <typename T=select_prompt>
        DTWAIN_SOURCE select_source(const T& selector = select_prompt(), bool open_source = true)
        { return select_source_impl(selector, open_source); }

        callback_handle register_listener(twain_source& src, twain_listener& listener);
        bool unregister_listener(callback_handle handle);
        void register_custom_logger(twain_logger& logger);
        void unregister_custom_logger();
        bool is_initialized() const { return m_isInitialized; }
        void enable_logger(bool bSet = true);
        twain_characteristics& get_twain_characteristics() { return tw_characteristics; }
        twain_identity& get_id();
        TW_IDENTITY* get_twain_id();
};

