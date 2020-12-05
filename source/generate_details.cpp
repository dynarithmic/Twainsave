#include <dynarithmic/twain/twain_session.hpp> // for dynarithmic::twain::twain_session
#include <dynarithmic/twain/twain_source.hpp>
#include <dynarithmic/twain/types/twain_listener.hpp>
#include <dynarithmic/twain/info/paperhandling_info.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <vector>

using namespace dynarithmic::twain;

template <typename Iter>
std::string join_string(Iter it1, Iter it2, char val = ',')
{
    std::stringstream strm;
    int i = 0;
    while (it1 != it2)
    {
        if (i != 0)
            strm << val;
        strm << *it1;
        ++it1;
        ++i;
    }
    return strm.str();
}

std::string remove_quotes(std::string s)
{
    std::string ret;
    s.erase(std::remove(s.begin(), s.end(), '\"'), s.end());
    return s;
}

template <typename T>
void create_stream(std::stringstream& strm, const capability_interface& capInfo, int capValue)
{
    std::vector<T> imageVals;
    imageVals = capInfo.get_cap_values<std::vector<T>>(capValue);
    if (imageVals.empty())
    {
        strm << "\"<not available>\"";
    }
    else
    {
        strm << "{";
        // check if range
        if (is_valid_range(imageVals))
            strm << "\"data-type\":\"range\",";
        else
            strm << "\"data-type\":\"discrete\",";
        strm << "\"data-values\":[" << join_string(imageVals.begin(), imageVals.end()) << "]}";
    }
}

template <typename T, typename S>
void create_stream(std::stringstream& strm, const capability_interface& capInfo, int capValue, bool createStringNames)
{
    std::vector<T> imageVals;
    imageVals = capInfo.get_cap_values<std::vector<T>>(capValue);
    if (imageVals.empty())
    {
        strm << "\"<not available>\"";
    }
    else
    {
        strm << "{";
        // check if range
        if (is_valid_range(imageVals))
            strm << "\"data-type\":\"range\",";
        else
            strm << "\"data-type\":\"discrete\",";
        auto vInfo = S::to_string(imageVals.begin(), imageVals.end());
        std::vector<std::string> vSizeNames;
        std::transform(vInfo.begin(), vInfo.end(), std::back_inserter(vSizeNames),
                       [](const std::pair<const char*, const char*>& p) { return "\"" + std::string(p.second) + "\""; });
        std::string paperSizesStr = join_string(vSizeNames.begin(), vSizeNames.end());
        strm << "\"data-values\":[" << join_string(vSizeNames.begin(), vSizeNames.end()) << "]}";
    }
}

std::string generate_details()
{
    using boost::algorithm::join;
    using boost::adaptors::transformed;
    using json = nlohmann::ordered_json;
    json glob_json;

    twain_session ts;
    ts.start();
    auto allSources = ts.get_twain_sources();
    glob_json["device-count"] = allSources.size();
    json array_twain_identity;
    json array_source_names;
    std::vector<std::string> sNames;
    std::transform(allSources.begin(), allSources.end(), std::back_inserter(sNames), [](auto& info) { return info.get_product_name(); });
    glob_json["device-names"] = sNames;
    std::string jsonString;
    std::string imageInfoString[9];
    std::string deviceInfoString[9];

    for (auto& curSource : allSources)
    {
        std::string jColorInfo;
        std::string resUnitInfo;
        deviceInfoString[0] = "\"feeder-supported\":false";
        deviceInfoString[1] = "\"feeder-sensitive\":false";
        deviceInfoString[2] = "\"ui-controllable\":false";
        deviceInfoString[3] = "\"autobright-supported\":false";
        deviceInfoString[4] = "\"autodeskew-supported\":false";
        deviceInfoString[5] = "\"imprinter-supported\":false";
        deviceInfoString[6] = "\"duplex-supported\":false";
        deviceInfoString[7] = "\"jobcontrol-supported\":false";
        deviceInfoString[8] = "\"transparencyunit-supported\":false";
        bool devOpen[] = { false, false };
        twain_source theSource = ts.select_source(select_byname(curSource.get_product_name()), false);
        if (theSource.is_selected())
        {
            devOpen[0] = true;
            if (curSource.get_product_name() == "FUJITSU fi-6230Zdj #2")
            {
                jsonString = curSource.to_json();
                jColorInfo = "\"color-info\":\"<not available>\",";
                resUnitInfo = "\"resolution-info\":\"<not available>\",";
                imageInfoString[0] = "\"brightness-values\":\"<not available>\"";
                imageInfoString[1] = "\"contrast-values\":\"<not available>\"";
                imageInfoString[2] = "\"gamma-values\":\"<not available>\"";
                imageInfoString[3] = "\"highlight-values\":\"<not available>\"";
                imageInfoString[4] = "\"shadow-values\":\"<not available>\"";
                imageInfoString[5] = "\"threshold-values\":\"<not available>\"";
                imageInfoString[6] = "\"rotation-values\":\"<not available>\"";
                imageInfoString[7] = "\"orientation-values\":\"<not available>\"";
                imageInfoString[8] = "\"overscan-values\":\"<not available>\"";
            }
            else
            {
                theSource.open();
                if (theSource.is_open())
                {
                    devOpen[1] = true;
                    jsonString = theSource.get_source_info().to_json();
                    jColorInfo = "\"color-info\":{";
                    std::stringstream strm;

                    // Get the pixel information
                    auto& capInfo = theSource.get_capability_interface();
                    auto pixInfo = capInfo.get_pixeltype();
                    strm << "\"num-colors\":" << pixInfo.size() << ",";
                    jColorInfo += strm.str();

                    strm.str("");
                    auto colorStr = join_string(pixInfo.begin(), pixInfo.end());
                    std::string joinStr = colorStr; // join(colorStrV, ",");
                    strm << "\"color-types\":[" << joinStr << "],";

                    std::stringstream strm2;
                    strm2 << "\"bitdepthinfo\":{";

                    for (auto p : pixInfo)
                    {
                        strm2 << "\"depth_" << p << "\":[";
                        capInfo.set_pixeltype({ p });
                        auto bdepth = capInfo.get_bitdepth();
                        std::string bdepthStr = join_string(bdepth.begin(), bdepth.end());
                        strm2 << bdepthStr << "],";
                    }
                    std::string allbdepths = strm2.str();
                    allbdepths.pop_back();
                    allbdepths += "}";

                    strm2.str("");

                    // get the paper sizes
                    auto paperSizes = capInfo.get_supportedsizes();
                    auto vSizeInfo = supportedsizes_value::to_string(paperSizes.begin(), paperSizes.end());
                    std::vector<std::string> vSizeNames;
                    std::transform(vSizeInfo.begin(), vSizeInfo.end(), std::back_inserter(vSizeNames),
                                    [](const std::pair<const char*, const char*>& p) { return "\"" + std::string(p.second) + "\""; });

                    std::string paperSizesStr = join_string(vSizeNames.begin(), vSizeNames.end());
                    strm2 << "\"paper-sizes\":[" << paperSizesStr << "],";
                    std::string allSizes = strm2.str();
                    jColorInfo += strm.str() + allbdepths + "}," + allSizes;

                    // get the resolution info
                    strm.str("");
                    strm << "\"resolution-info\": {";

                    auto allUnits = capInfo.get_units();
                    strm << "\"resolution-count\":" << allUnits.size() << ",";
                    strm << "\"resolution-units\":";
                    auto unitNames = units_value::to_string(allUnits);
                    std::vector<std::string> unitNameV;
                    std::transform(unitNames.begin(), unitNames.end(), std::back_inserter(unitNameV),
                                    [](const std::pair<const char*, const char*>& p) { return "\"" + std::string(p.second) + "\""; });
                    std::string unitNameStr = "[" + join_string(unitNameV.begin(), unitNameV.end(), ',') + "]";
                    strm << unitNameStr << ",";
                    int i = 0;
                    std::string resolutionTotalStr;
                    for (auto u : allUnits)
                    {
                        strm2.str("");
                        if (i > 0)
                            strm2 << ",";
                        strm2 << "\"resolution-" << remove_quotes(unitNameV[i]) << "\": {";

                        // set the unit of measure
                        capInfo.set_units({ u });

                        // get all the values
                        auto allUnitValues = capInfo.get_xresolution();

                        // check if range
                        if (is_valid_range(allUnitValues))
                            strm2 << "\"data-type\":\"range\",";
                        else
                            strm2 << "\"data-type\":\"discrete\",";
                        strm2 << "\"data-values\":[" << join_string(allUnitValues.begin(), allUnitValues.end()) << "]}";
                        resolutionTotalStr += strm2.str();
                        ++i;
                    }

                    resUnitInfo = strm.str() + resolutionTotalStr + "},";

                    int imageInfoCaps[] = { ICAP_BRIGHTNESS, ICAP_CONTRAST, ICAP_GAMMA, ICAP_HIGHLIGHT, ICAP_SHADOW,
                        ICAP_THRESHOLD, ICAP_ROTATION, ICAP_ORIENTATION, ICAP_OVERSCAN };
                    std::string imageInfoCapsStr[] = { "\"brightness-values\":", "\"contrast-values\":", "\"gamma-values\":",
                        "\"highlight-values\":", "\"shadow-values\":", "\"threshold-values\":",
                        "\"rotation-values\":", "\"orientation-values\":", "\"overscan-values\":" };
                    for (int i = 0; i < sizeof(imageInfoCaps) / sizeof(imageInfoCaps[0]); ++i)
                    {
                        strm.str("");
                        strm << imageInfoCapsStr[i];
                        if (imageInfoCaps[i] == ICAP_ORIENTATION)
                            create_stream<ICAP_ORIENTATION_::value_type>(strm, capInfo, ICAP_ORIENTATION);
                        else
                            if (imageInfoCaps[i] == ICAP_OVERSCAN)
                                create_stream<ICAP_OVERSCAN_::value_type, overscan_value>(strm, capInfo, ICAP_OVERSCAN, true);
                            else
                                create_stream<double>(strm, capInfo, imageInfoCaps[i]);
                        imageInfoString[i] = strm.str();
                    }

                    strm.str("");
                    int deviceInfoCaps[] = { CAP_FEEDERENABLED, CAP_FEEDERLOADED, CAP_UICONTROLLABLE,
                        ICAP_AUTOBRIGHT, ICAP_AUTOMATICDESKEW,
                        CAP_PRINTER, CAP_DUPLEX, CAP_JOBCONTROL, ICAP_LIGHTPATH
                    };

                    std::string deviceInfoCapsStr[sizeof(deviceInfoCaps) / sizeof(deviceInfoCaps[0])];
                    std::copy(deviceInfoString, deviceInfoString + sizeof(deviceInfoString) / sizeof(deviceInfoString[0]), deviceInfoCapsStr);
                    for (auto& s : deviceInfoCapsStr)
                        s.resize(s.size() - 5);
                    paperhandling_info pinfo(theSource);
                    for (int i = 0; i < sizeof(deviceInfoCaps) / sizeof(deviceInfoCaps[0]); ++i)
                    {
                        if (i > 0)
                            strm << ",";
                        bool value = false;
                        if (deviceInfoCaps[i] == CAP_FEEDERENABLED)
                            value = pinfo.is_feedersupported();
                        else
                        if (deviceInfoCaps[i] == CAP_UICONTROLLABLE)
                        {
                            auto vValue = capInfo.get_uicontrollable();
                            if (!vValue.empty())
                                value = vValue.front();
                        }
                        else
                        if (deviceInfoCaps[i] == CAP_PRINTER)
                        {
                            auto vValue = capInfo.get_printer();
                            value = (!vValue.empty() && vValue.front() != TWDX_NONE);
                        }
                        else
                        if (deviceInfoCaps[i] == CAP_JOBCONTROL)
                        {
                            auto vValue = capInfo.get_jobcontrol();
                            value = (!vValue.empty() && vValue.front() != TWJC_NONE);
                        }
                        else
                            value = capInfo.is_cap_supported(deviceInfoCaps[i]);
                        strm << deviceInfoCapsStr[i] << (value ? "true" : "false");
                        deviceInfoString[i] = strm.str();
                    }
                }
                else
                {
                    jsonString = curSource.to_json();
                    jColorInfo = "\"color-info\":\"<not available>\",";
                    resUnitInfo = "\"resolution-info\":\"<not available>\",";
                    imageInfoString[0] = "\"brightness-values\":\"<not available>\"";
                    imageInfoString[1] = "\"contrast-values\":\"<not available>\"";
                    imageInfoString[2] = "\"gamma-values\":\"<not available>\"";
                    imageInfoString[3] = "\"highlight-values\":\"<not available>\"";
                    imageInfoString[4] = "\"shadow-values\":\"<not available>\"";
                    imageInfoString[5] = "\"threshold-values\":\"<not available>\"";
                    imageInfoString[6] = "\"rotation-values\":\"<not available>\"";
                    imageInfoString[7] = "\"orientation-values\":\"<not available>\"";
                    imageInfoString[8] = "\"overscan-values\":\"<not available>\"";

                    deviceInfoString[0] = "\"feeder-supported\":false";
                    deviceInfoString[1] = "\"feeder-sensitive\":false";
                    deviceInfoString[2] = "\"ui-controllable\":false";
                    deviceInfoString[3] = "\"autobright-supported\":false";
                    deviceInfoString[4] = "\"autodeskew-supported\":false";
                    deviceInfoString[5] = "\"imprinter-supported\":false";
                    deviceInfoString[6] = "\"duplex-supported\":false";
                    deviceInfoString[7] = "\"jobcontrol-supported\":false";
                    deviceInfoString[8] = "\"transparencyunit-supported\":false";
                }
            }

            std::string partString;
            if (devOpen[0] && devOpen[1])
                partString = "\"<selected,opened>\"";
            else
            if (!devOpen[0] && !devOpen[1])
                partString = "\"<error>\"";
            else
            if (devOpen[0] && !devOpen[1])
                partString = "\"<selected>\"";

            partString = "\"device-status\":" + partString + ",";
            std::string imageInfoStringVal = join_string(imageInfoString, imageInfoString +
                                                            sizeof(imageInfoString) / sizeof(imageInfoString[0])) + ",";
            std::string deviceInfoStringVal = join_string(deviceInfoString, deviceInfoString +
                                                            sizeof(deviceInfoString) / sizeof(deviceInfoString[0])) + ",";
            jsonString = "{" + partString + jColorInfo + resUnitInfo + imageInfoStringVal + deviceInfoStringVal + jsonString.substr(1);
            array_twain_identity.push_back(json::parse(jsonString));
        }
    }

    glob_json["device-info"] = array_twain_identity; 
    return glob_json.dump(4);
 }