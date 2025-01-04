#include <dynarithmic/twain/twain_session.hpp> // for dynarithmic::twain::twain_session
#include <dynarithmic/twain/twain_source.hpp>
#include <dynarithmic/twain/types/twain_callback.hpp>
#include <dynarithmic/twain/info/paperhandling_info.hpp>
#include <sstream>
#include <string>
#include <vector>

using namespace dynarithmic::twain;

std::string generate_details()
{
    twain_session ts(startup_mode::autostart);
    return ts.get_details(details_info());
}
