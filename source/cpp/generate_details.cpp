#define NOMINMAX
#include <dynarithmic/twain/twain_session.hpp> // for dynarithmic::twain::twain_session
#include <dynarithmic/twain/twain_source.hpp>
#include <dynarithmic/twain/types/twain_callback.hpp>
#include <dynarithmic/twain/info/paperhandling_info.hpp>
#include <sstream>
#include <string>
#include <vector>
#include <boost/algorithm/string/join.hpp>
#include "generate_details.h"
#include "twainsave_app.h"

using namespace dynarithmic::twain;

std::string generate_details(dynarithmic::twain::twain_session* pSession, twainsave_app* pTheApp)
{
    twain_session ts(startup_mode::none);
    if ( !pSession )
        pSession = &ts;
    twain_source temp{};
    pSession->register_callback(temp, pTheApp->get_callback());
    start_twain_session(*pSession);
    return pSession->get_details(details_info());
}

std::string generate_dtwainversion_info(dynarithmic::twain::twain_session* pSession)
{
	twain_session ts(startup_mode::none);
	if (!pSession)
		pSession = &ts;
    bool started = start_twain_session(*pSession, true);
    auto all_errors = pSession->get_error_logger().get_errors();

    // Test specifically for a DTWAIN DLL Loading error
    auto iter = std::find(all_errors.begin(), all_errors.end(), DTWAIN_ERR_DTWAINDLL_LOADERROR);

    // Get the DTWAIN version loaded
    if (started || (!started && iter == all_errors.end()))
    {
        char szShortVersion[256] = {};
        API_INSTANCE DTWAIN_GetShortVersionStringA(szShortVersion, 256);
        return szShortVersion;
    }
    return {};
}

std::string generate_productnames(dynarithmic::twain::twain_session* pSession)
{
	twain_session ts(startup_mode::none);
    if (!pSession)
        pSession = &ts;
    start_twain_session(*pSession);
    auto sourceInfo = pSession->get_all_source_info();
    std::vector<std::string> vProductNames;
    auto iter = sourceInfo.begin();
    while (iter != sourceInfo.end())
    {
        vProductNames.push_back("\"" + iter->get_product_name() + "\"");
        ++iter;
    }
    std::sort(vProductNames.begin(), vProductNames.end());
    return "[\n" + boost::algorithm::join(vProductNames, ",\n") + "\n]\n";
}
