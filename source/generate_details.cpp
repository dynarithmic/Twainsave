#include <dynarithmic/twain/twain_session.hpp> // for dynarithmic::twain::twain_session
#include <dynarithmic/twain/twain_source.hpp>
#include <dynarithmic/twain/types/twain_callback.hpp>
#include <dynarithmic/twain/info/paperhandling_info.hpp>
#include <sstream>
#include <string>
#include <vector>
#include <boost/algorithm/string/join.hpp>
using namespace dynarithmic::twain;

std::string generate_details(dynarithmic::twain::twain_session* pSession)
{
    twain_session ts(startup_mode::none);
    if ( !pSession )
        pSession = &ts;
    pSession->start();
    return pSession->get_details(details_info());
}

std::string generate_productnames(dynarithmic::twain::twain_session* pSession)
{
	twain_session ts(startup_mode::none);
    if (!pSession)
        pSession = &ts;
    pSession->start();
    auto sourceInfo = pSession->get_all_source_info();
    std::vector<std::string> vProductNames;
    auto iter = sourceInfo.begin();
    while (iter != sourceInfo.end())
    {
        vProductNames.push_back("\"" + iter->get_product_name() + "\"");
        ++iter;
    }
    return "[\n" + boost::algorithm::join(vProductNames, ",\n") + "\n]\n";
}
