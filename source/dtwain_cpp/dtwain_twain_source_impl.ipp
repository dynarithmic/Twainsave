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
template <typename Container, typename SourceType, typename Fn>
bool twain_source::get_values_impl(Container& ct, SourceType st, Fn fn) const
{
    std::vector<SourceType> vs;
    ct.clear();
    const auto retval = fn(vs).first;
    if (retval)
        std::transform(vs.begin(), vs.end(), std::inserter(ct, ct.end()), [&](SourceType& v)
    { return static_cast<typename Container::value_type>(v); });
    return retval;
}

template <typename Container, typename Fn>
bool twain_source::get_values_impl(Container& ct, Fn fn) const
{
    ct.clear();
    return fn(ct).first;
}

template <typename Container>
bool twain_source::get_pixel_types(Container& ct) const
{
    using stype = capability_interface::pixeltype_type;
    static_assert(std::is_same<typename Container::value_type, stype>(), "Container type mismatch");
    return get_values_impl(ct, stype(), [this](std::vector<stype>& c)
                            {return get_capability_interface().get_pixeltype(c); });
}

template <typename Container>
bool twain_source::get_paper_sizes(Container& ct) const 
{
    using stype = capability_interface::supportedsizes_type;
    static_assert(std::is_same<typename Container::value_type, paper_size>(), "Container must have a value_type of paper_size");
    return get_values_impl(ct, stype(), [this](std::vector<stype>& c)
                            {return get_capability_interface().get_supportedsizes(c); });
}

template <typename Container>
bool twain_source::get_measure_units(Container& ct) const
{
    using stype = capability_interface::units_type;
    static_assert(std::is_same<typename Container::value_type, measure_unit>(), "Container must have a value_type of measure_unit");
    return get_values_impl(ct, stype(), [this](std::vector<stype>& c)
                            {return get_capability_interface().get_units(c); });
}

template <typename Container>
bool twain_source::get_resolution_values(Container& ct) const
{
    using stype = capability_interface::xresolution_type;
    static_assert(std::is_same<typename Container::value_type, stype>(), "Container type mismatch");
    return get_values_impl(ct, stype(), [this](std::vector<stype>& c)
                            {return get_capability_interface().get_xresolution(c); });
}

template <typename Container>
bool twain_source::get_contrast_values(Container& ct) const
{
    using stype = capability_interface::contrast_type;
    static_assert(std::is_same<typename Container::value_type, stype>(), "Container type mismatch");
    return get_values_impl(ct, stype(), [this](std::vector<stype>& c)
                            {return get_capability_interface().get_contrast(c); });
}

template <typename Container>
bool twain_source::get_brightness_values(Container& ct) const
{
    using stype = capability_interface::brightness_type;
    static_assert(std::is_same<typename Container::value_type, stype>(), "Container type mismatch");
    return get_values_impl(ct, [this](std::vector<stype>& c)
                            {return get_capability_interface().get_brightness(c); });
}

template <typename Container>
bool twain_source::get_halftone_values(Container& ct) const
{
    using stype = capability_interface::halftones_type;
    static_assert(std::is_same<typename Container::value_type, stype>(), "Container type mismatch");
    return get_values_impl(ct, [this](std::vector<stype>& c)
                            {return get_capability_interface().get_halftones(c); });
}

template <typename Container>
bool twain_source::get_lightsource_values(Container& ct) const
{
    using stype = capability_interface::lightsource_type;
    static_assert(std::is_same<typename Container::value_type, lightsource_type>(), "Container type mismatch");
    return get_values_impl(ct, stype(), [this](std::vector<stype>& c)
                            {return get_capability_interface().get_lightsource(c); });
}

template <typename Container>
bool twain_source::get_compression_types(Container& ct) const
{
    using stype = capability_interface::compression_type;
    static_assert(std::is_same<typename Container::value_type, compression_type>(), "Container type mismatch");
    return get_values_impl(ct, stype(), [this](std::vector<stype>& c)
                            {return get_capability_interface().get_compression(c); });
}

template <typename Container>
bool twain_source::get_filexfer_formats(Container& ct) const
{
    using stype = capability_interface::imagefileformat_type;
    static_assert(std::is_same<typename Container::value_type, file_type>(), "Container type mismatch");
    return get_values_impl(ct, stype(), [this](std::vector<stype>& c)
                            {return get_capability_interface().get_imagefileformat(c); });
}

template <typename Container>
bool twain_source::get_orientation_values(Container& ct) const
{
    using stype = capability_interface::orientation_type;
    static_assert(std::is_same<typename Container::value_type, image_orientation>(), "Container type mismatch");
    return get_values_impl(ct, stype(), [this](std::vector<stype>& c)
    {return get_capability_interface().get_orientation(c); });
}

template <typename Container>
bool twain_source::get_rotation_values(Container& ct) const
{
    using stype = capability_interface::rotation_type;
    static_assert(std::is_same<typename Container::value_type, stype>(), "Container type mismatch");
    return get_values_impl(ct, [this](std::vector<stype>& c)
    {return get_capability_interface().get_rotation(c); });
}
