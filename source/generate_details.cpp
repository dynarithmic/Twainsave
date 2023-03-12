#include <dynarithmic/twain/twain_session.hpp> // for dynarithmic::twain::twain_session
#include <dynarithmic/twain/twain_source.hpp>
#include <dynarithmic/twain/types/twain_callback.hpp>
#include <dynarithmic/twain/info/paperhandling_info.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/range/adaptor/transformed.hpp>
//#include <nlohmann/json.hpp>
#include <dynarithmic/twain/tostring/tojson.hpp>
#include <sstream>
#include <string>
#include <vector>

using namespace dynarithmic::twain;

std::string generate_details()
{
    twain_session ts(startup_mode::autostart);
    auto vSources = ts.get_all_source_info();
    std::vector<std::string> vProductNames;
    for (auto& s : vSources)
        vProductNames.push_back(s.get_product_name());
    return dynarithmic::twain::json_generator::generate_details(ts, vProductNames, true);
}
