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
// This class is used to listen for TWAIN events when an acquisition is occurring.  Use this to do special
// processing on acquisition startup, UI opening and closing, etc.  See the DTWAIN help manual for a list of
// the various events.

#ifndef DTWAIN_TWAIN_LISTENER_HPP
#define DTWAIN_TWAIN_LISTENER_HPP

#include <unordered_map>
#include <dtwain.h>
#include <dynarithmic/twain/twain_values.hpp>
#include <dynarithmic/twain/source/twain_source_base.hpp>
namespace dynarithmic
{
    namespace twain
    {
        class twain_source;
        class twain_listener
        {
            private:
                typedef int (twain_listener::*twain_listener_func)(twain_source&);
                typedef LRESULT (twain_listener::*twain_error_func)(LONG, LONG);
                using twain_listener_map = std::unordered_map<LONG, twain_listener_func>;
    
                LONG m_UserData;
                bool m_bDefaultHandler;
                LONG m_nNotificationID;
                twain_listener_map m_listenerMap;
    
            protected:
                virtual bool starthandler(twain_source&, WPARAM, LPARAM, int&) { return true; }
                virtual int preacquire(twain_source&)  { return 1; }
                virtual int preacquire_terminate(twain_source&)  { return 1; }
                virtual int acquiredone(twain_source&) { return 1; }
                virtual int acquirefailed(twain_source&) { return 1; }
                virtual int acquirecancelled(twain_source&) { return 1; }
                virtual int acquirestarted(twain_source&) { return 1; }
                virtual int pagecontinue(twain_source&) { return 1; }
                virtual int pagefailed(twain_source&) { return 1; }
                virtual int pagecancelled(twain_source&) { return 1; }
                virtual int transferready(twain_source&) { return 1; }
                virtual int transferdone(twain_source&) { return 1; }
                virtual int uiclosing(twain_source&) { return 1; }
                virtual int uiclosed(twain_source&) { return 1; }
                virtual int uiopened(twain_source&) { return 1; }
                virtual int uiopenfailure(twain_source&) { return 1; }
                virtual int defaulthandler(twain_source&, WPARAM, LPARAM, LONG) { return 1; }
                virtual int cliptransferdone(twain_source&) { return 1; }
                virtual int invalidimageformat(twain_source&) { return 1; }
                virtual int acquireterminated(twain_source&) { return 1; }
                virtual int transferstripready(twain_source&) { return 1; }
                virtual int transferstripdone(twain_source&) { return 1; }
                virtual int transferstripfailed(twain_source&) { return 1; }
                virtual int imageinfoerror(twain_source&) { return 1; }
                virtual int transfercancelled(twain_source&) { return 1; }
                virtual int filesavecancelled(twain_source&) { return 1; }
                virtual int filesaveok(twain_source&) { return 1; }
                virtual int filesaveerror(twain_source&) { return 1; }
                virtual int filepagesaveok(twain_source&) { return 1; }
                virtual int filepagesaveerror(twain_source&) { return 1; }
                virtual int processeddib(twain_source&) { return 1; }
                virtual int deviceevent(twain_source&) { return 1; }
                virtual int eojdetected(twain_source&) { return 1; }
                virtual int eojdetectedtransferdone(twain_source&) { return 1; }
                virtual int twainpagecancelled(twain_source&) { return 1; }
                virtual int twainpagefailed(twain_source&) { return 1; }
                virtual int querypagediscard(twain_source&) { return 1; }
                virtual int pagediscarded(twain_source&) { return 1; }
                virtual int appupdateddib(twain_source&) { return 1; }
                virtual int filepagesaving(twain_source&) { return 1; }
                virtual int processeddibfinal(twain_source&) { return 1; }
                virtual int manualduplexside1start(twain_source&) { return 1; }
                virtual int manualduplexside2start(twain_source&) { return 1; }
                virtual int manualduplexside1done(twain_source&) { return 1; }
                virtual int manualduplexside2done(twain_source&) { return 1; }
                virtual int manualduplexmergeerror(twain_source&) { return 1; }
                virtual int manualduplexcounterror(twain_source&) { return 1; }
                virtual int manualduplexmemoryerror(twain_source&) { return 1; }
                virtual int manualduplexfileerror(twain_source&) { return 1; }
                virtual int manualduplexfilesaveerror(twain_source&) { return 1; }
                virtual int blankpagedetected1(twain_source&) { return 1; }
                virtual int blankpagedetected2(twain_source&) { return 1; }
                virtual int blankpagediscarded1(twain_source&) { return 1; }
                virtual int blankpagediscarded2(twain_source&) { return 1; }
                virtual int filenamechanging(twain_source&) { return 1; }
                virtual int filenamechanged(twain_source&) { return 1; }
    
            public:
                twain_listener() : m_UserData(0), m_bDefaultHandler(false), m_nNotificationID(0), m_listenerMap({
                    {twain_listener_values::DTWAIN_PREACQUIRE_START, &twain_listener::preacquire},
                    { twain_listener_values::DTWAIN_PREACQUIRE_TERMINATE, &twain_listener::preacquire_terminate},
                    {DTWAIN_TN_ACQUIREDONE, &twain_listener::acquiredone},
                    {DTWAIN_TN_ACQUIREFAILED, &twain_listener::acquirefailed},
                    {DTWAIN_TN_ACQUIRECANCELLED, &twain_listener::acquirecancelled},
                    {DTWAIN_TN_ACQUIRESTARTED, &twain_listener::acquirestarted},
                    {DTWAIN_TN_PAGECONTINUE, &twain_listener::pagecontinue},
                    {DTWAIN_TN_PAGEFAILED, &twain_listener::pagefailed},
                    {DTWAIN_TN_PAGECANCELLED, &twain_listener::pagecancelled},
                    {DTWAIN_TN_TRANSFERREADY, &twain_listener::transferready},
                    {DTWAIN_TN_TRANSFERDONE, &twain_listener::transferdone},
                    {DTWAIN_TN_UICLOSING, &twain_listener::uiclosing},
                    {DTWAIN_TN_UICLOSED, &twain_listener::uiclosed},
                    {DTWAIN_TN_UIOPENED, &twain_listener::uiopened},
                    {DTWAIN_TN_CLIPTRANSFERDONE, &twain_listener::cliptransferdone},
                    {DTWAIN_TN_INVALIDIMAGEFORMAT, &twain_listener::invalidimageformat},
                    {DTWAIN_TN_ACQUIRETERMINATED, &twain_listener::acquireterminated},
                    {DTWAIN_TN_TRANSFERSTRIPREADY, &twain_listener::transferstripready},
                    {DTWAIN_TN_TRANSFERSTRIPDONE, &twain_listener::transferstripdone},
                    {DTWAIN_TN_TRANSFERSTRIPFAILED, &twain_listener::transferstripfailed},
                    {DTWAIN_TN_IMAGEINFOERROR, &twain_listener::imageinfoerror},
                    {DTWAIN_TN_TRANSFERCANCELLED, &twain_listener::transfercancelled},
                    {DTWAIN_TN_FILESAVECANCELLED, &twain_listener::filesavecancelled},
                    {DTWAIN_TN_FILESAVEOK, &twain_listener::filesaveok},
                    {DTWAIN_TN_FILESAVEERROR, &twain_listener::filesaveerror},
                    {DTWAIN_TN_FILEPAGESAVEOK, &twain_listener::filepagesaveok},
                    {DTWAIN_TN_FILEPAGESAVEERROR, &twain_listener::filepagesaveerror},
                    {DTWAIN_TN_PROCESSEDDIB, &twain_listener::processeddib},
                    {DTWAIN_TN_DEVICEEVENT, &twain_listener::deviceevent},
                    {DTWAIN_TN_ENDOFJOBDETECTED, &twain_listener::eojdetected},
                    {DTWAIN_TN_EOJDETECTED_XFERDONE, &twain_listener::eojdetectedtransferdone},
                    {DTWAIN_TN_TWAINPAGECANCELLED, &twain_listener::twainpagecancelled},
                    {DTWAIN_TN_TWAINPAGEFAILED, &twain_listener::twainpagefailed},
                    {DTWAIN_TN_QUERYPAGEDISCARD, &twain_listener::querypagediscard},
                    {DTWAIN_TN_PAGEDISCARDED, &twain_listener::pagediscarded},
                    {DTWAIN_TN_APPUPDATEDDIB, &twain_listener::appupdateddib},
                    {DTWAIN_TN_FILEPAGESAVING, &twain_listener::filepagesaving},
                    {DTWAIN_TN_PROCESSEDDIBFINAL, &twain_listener::processeddibfinal},
                    {DTWAIN_TN_MANDUPSIDE1START, &twain_listener::manualduplexside1start},
                    {DTWAIN_TN_MANDUPSIDE2START, &twain_listener::manualduplexside2start},
                    {DTWAIN_TN_MANDUPSIDE1DONE, &twain_listener::manualduplexside1done},
                    {DTWAIN_TN_MANDUPSIDE2DONE, &twain_listener::manualduplexside2done},
                    {DTWAIN_TN_MANDUPMERGEERROR, &twain_listener::manualduplexmergeerror},
                    {DTWAIN_TN_MANDUPPAGECOUNTERROR, &twain_listener::manualduplexcounterror},
                    {DTWAIN_TN_MANDUPMEMORYERROR, &twain_listener::manualduplexmemoryerror},
                    {DTWAIN_TN_MANDUPFILEERROR, &twain_listener::manualduplexfileerror},
                    {DTWAIN_TN_MANDUPFILESAVEERROR, &twain_listener::manualduplexfilesaveerror},
                    {DTWAIN_TN_BLANKPAGEDETECTED1, &twain_listener::blankpagedetected1},
                    {DTWAIN_TN_BLANKPAGEDETECTED2, &twain_listener::blankpagedetected2},
                    {DTWAIN_TN_BLANKPAGEDISCARDED1, &twain_listener::blankpagediscarded1},
                    {DTWAIN_TN_BLANKPAGEDISCARDED2, &twain_listener::blankpagediscarded2},
                    {DTWAIN_TN_FILENAMECHANGING, &twain_listener::filenamechanging},
                    {DTWAIN_TN_FILENAMECHANGED, &twain_listener::filenamechanged},
                    {DTWAIN_TN_UIOPENFAILURE, &twain_listener::uiopenfailure}
                })
                {}
    
                LRESULT call_func(WPARAM wParm, LPARAM lParm, twain_source* pSource)
                {
                    // Always called when handler starts
                    int status = 0;
                    m_nNotificationID = static_cast<LONG>(wParm);
                    if (!starthandler(*pSource, wParm, lParm, status))
                        return status;
    
                    const auto it = m_listenerMap.find(static_cast<LONG>(wParm));
                    if (it != m_listenerMap.end())
                        return (this->*((*it).second))(*pSource);
                    return defaulthandler(*pSource, wParm, lParm, m_UserData);
                }
    
                virtual ~twain_listener() {}
        };
    }
}
#endif
