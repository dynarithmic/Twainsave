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
// transfer type (native, buffered, file)
enum class transfer_type
{
    image_native = 0,
    image_buffered = 1,
    file_using_native = 2,
    file_using_buffered = 3,
    file_using_source = 4
};

// image color types
enum class color_type
{
    bw = DTWAIN_PT_BW,
    gray = DTWAIN_PT_GRAY,
    rgb = DTWAIN_PT_RGB,
    palette = DTWAIN_PT_PALETTE,
    cmy = DTWAIN_PT_CMY,
    cmyk = DTWAIN_PT_CMYK,
    yuv = DTWAIN_PT_YUV,
    yuvk = DTWAIN_PT_YUVK,
    default = DTWAIN_PT_DEFAULT
};

inline std::ostream& operator << (std::ostream& os, const color_type& ct)
{
    os << ct;
    return os;
}

enum class image_orientation
{
    orient_0 = DTWAIN_OR_ROT0,
    orient_90 = DTWAIN_OR_ROT90,
    orient_180 = DTWAIN_OR_ROT180,
    orient_270 = DTWAIN_OR_ROT270,
    portrait = orient_0,
    landscape = orient_90
};

inline image_orientation get_orientation(int orient)
{
    switch(orient)
    {
        case 0:
           return image_orientation::orient_0;
        case 90:
            return image_orientation::orient_90;
        case 180:
            return image_orientation::orient_180;
        case 270:
            return image_orientation::orient_270;
    }
    return image_orientation::orient_90;
}

inline std::ostream& operator <<(std::ostream& os, const image_orientation& io)
{
    os << io;
    return os;
}

enum class measure_unit
{
    inch = DTWAIN_INCHES,
    cm = DTWAIN_CENTIMETERS,
    pica = DTWAIN_PICAS,
    point = DTWAIN_POINTS,
    twip = DTWAIN_TWIPS,
    pixel = DTWAIN_PIXELS,
    mm = DTWAIN_MILLIMETERS
};

inline std::ostream& operator <<(std::ostream& os, const measure_unit& mu)
{
    os << mu;
    return os;
}

enum class lightsource_type
{
    red = DTWAIN_LS_RED,
    green = DTWAIN_LS_GREEN,
    blue = DTWAIN_LS_BLUE,
    none = DTWAIN_LS_NONE,
    white = DTWAIN_LS_WHITE,
    ultraviolet = DTWAIN_LS_UV,
    infrared = DTWAIN_LS_IR
};

enum class paper_size
{
    A0 = TWSS_A0,
    A1 = TWSS_A1,
    A10 = TWSS_A10,
    A2 = TWSS_A2,
    A3 = TWSS_A3,
    A4 = TWSS_A4,
    A4LETTER = TWSS_A4LETTER,
    A5 = TWSS_A5,
    A6 = TWSS_A6,
    A7 = TWSS_A7,
    A8 = TWSS_A8,
    A9 = TWSS_A9,
    B3 = TWSS_B3,
    B4 = TWSS_B4,
    B5LETTER = TWSS_B5LETTER,
    B6 = TWSS_B6,
    BUSINESSCARD = TWSS_BUSINESSCARD,
    C0 = TWSS_C0,
    C1 = TWSS_C1,
    C10 = TWSS_C10,
    C2 = TWSS_C2,
    C3 = TWSS_C3,
    C4 = TWSS_C4,
    C5 = TWSS_C5,
    C6 = TWSS_C6,
    C7 = TWSS_C7,
    C8 = TWSS_C8,
    C9 = TWSS_C9,
    FOURA0 = TWSS_4A0,
    ISOB0 = TWSS_ISOB0,
    ISOB1 = TWSS_ISOB1,
    ISOB10 = TWSS_ISOB10,
    ISOB2 = TWSS_ISOB2,
    ISOB3 = TWSS_ISOB3,
    ISOB4 = TWSS_ISOB4,
    ISOB5 = TWSS_ISOB5,
    ISOB6 = TWSS_ISOB6,
    ISOB7 = TWSS_ISOB7,
    ISOB8 = TWSS_ISOB8,
    ISOB9 = TWSS_ISOB9,
    JISB0 = TWSS_JISB0,
    JISB1 = TWSS_JISB1,
    JISB10 = TWSS_JISB10,
    JISB2 = TWSS_JISB2,
    JISB3 = TWSS_JISB3,
    JISB4 = TWSS_JISB4,
    JISB5 = TWSS_JISB5,
    JISB6 = TWSS_JISB6,
    JISB7 = TWSS_JISB7,
    JISB8 = TWSS_JISB8,
    JISB9 = TWSS_JISB9,
    MAXSIZE = TWSS_MAXSIZE,
    NONE = TWSS_NONE,
    TWOA0 = TWSS_2A0,
    USEXECUTIVE = TWSS_USEXECUTIVE,
    USLEDGER = TWSS_USLEDGER,
    USLEGAL = TWSS_USLEGAL,
    USLETTER = TWSS_USLETTER,
    USSTATEMENT = TWSS_USSTATEMENT,
    CUSTOM = DTWAIN_PDF_CUSTOMSIZE,
    VARIABLE = DTWAIN_PDF_VARIABLEPAGESIZE,
    PIXELSPERMETER = DTWAIN_PDF_PIXELSPERMETERSIZE
};

inline long to_long(paper_size ps) { return static_cast<long>(ps); }
inline std::ostream& operator << (std::ostream& os, const paper_size& ps)
{
    os << ps;
    return os;
}

enum class jobcontrol_type
{
    none = TWJC_NONE,
    include_separator = TWJC_JSIC,
    include_separator_stop = TWJC_JSIS,
    exclude_separator = TWJC_JSXC,
    exclude_separator_stop = TWJC_JSXS
};

inline std::ostream& operator <<(std::ostream& os, const jobcontrol_type& jc)
{
    os << jc;
    return os;
}

enum class getcap_type
{
    GET = DTWAIN_CAPGET,
    GET_CURRENT = DTWAIN_CAPGETCURRENT,
    GET_DEFAULT = DTWAIN_CAPGETDEFAULT
};

enum class setcap_type
{
    SET = DTWAIN_CAPSET,
    RESET = DTWAIN_CAPRESET,
    SET_CONSTRAINT = DTWAIN_CAPSETCONSTRAINT
};

enum class twain_container_type
{
    CONTAINER_INVALID = 0,
    CONTAINER_ONEVALUE = DTWAIN_CONTONEVALUE,
    CONTAINER_ARRAY = DTWAIN_CONTARRAY,
    CONTAINER_ENUMERATION = DTWAIN_CONTENUMERATION,
    CONTAINER_RANGE = DTWAIN_CONTRANGE,
    CONTAINER_DEFAULT = DTWAIN_CONTDEFAULT
};

enum class twain_data_type
{
    DATATYPE_DEFAULT = -1,
    DATATYPE_INT8 = TWTY_INT8,
    DATATYPE_UINT8 = TWTY_UINT8,
    DATATYPE_BOOL = TWTY_BOOL,
    DATATYPE_INT16 = TWTY_INT16,
    DATATYPE_INT32 = TWTY_INT32,
    DATATYPE_UINT16 = TWTY_UINT16,
    DATATYPE_UINT32 = TWTY_UINT32,
    DATATYPE_STR32 = TWTY_STR32,
    DATATYPE_STR64 = TWTY_STR64,
    DATATYPE_STR128 = TWTY_STR128,
    DATATYPE_STR255 = TWTY_STR255,
    DATATYPE_STR1024 = TWTY_STR1024,
    DATATYPE_FIX32 = TWTY_FIX32,
    DATATYPE_FRAME = TWTY_FRAME
};

enum class twain_compression_type
{
    none = DTWAIN_CP_NONE,
    packbits = DTWAIN_CP_PACKBITS,
    group3_1d = DTWAIN_CP_GROUP31D,
    group3_1deol = DTWAIN_CP_GROUP31DEOL,
    group3_2d = DTWAIN_CP_GROUP32D,
    group4 = DTWAIN_CP_GROUP4,
    jpeg = DTWAIN_CP_JPEG,
    lzw = DTWAIN_CP_LZW,
    jbig = DTWAIN_CP_JBIG,
    png = DTWAIN_CP_PNG,
    rle4 = DTWAIN_CP_RLE4,
    rle8 = DTWAIN_CP_RLE8,
    bitfields = DTWAIN_CP_BITFIELDS,
    zip = TWCP_ZIP,
    jpeg2k = TWCP_JPEG2000
};

enum class high_level_cap
{
    duplexenabled,
    feederenabled,
    feederloaded,
    paperdetectable,
    filmscan,
    uionly,
    indicators,
    customdata,
    orientation,
    rotation,
    lightsource,
    brightness,
    contrast,
    gamma,
    highlight,
    threshold,
    deviceonline,
    jobcontrol,
    automaticdeskew,
    automaticrotate,
    autobright,
    overscan,
    shadow,
    printer,
    printerstring,
    printercharrotation,
    printersuffix,
    printerindex, 
    printerindexstep,
    printerindexmaxvalue, 
    printerindexnumdigits,
    printerindexleadchar, 
    printermode, 
    printerverticaloffset,
    printerindextrigger,
    printerstringpreview
};
