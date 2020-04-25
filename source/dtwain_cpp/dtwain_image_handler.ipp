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
enum class blank_image_discard_option
{
   discard_all_on_notification = DTWAIN_BP_AUTODISCARD_NONE,
   discard_all = DTWAIN_BP_AUTODISCARD_ANY,
   discard_all_from_device = DTWAIN_BP_AUTODISCARD_IMMEDIATE,
   discard_all_after_resampling =  DTWAIN_BP_AUTODISCARD_AFTERPROCESS
};

struct blank_image_handler
{
    bool enabled = false;
    double threshold = 98.0;
    blank_image_discard_option discard_option = blank_image_discard_option::discard_all;
};

struct image_information
{
    double x_resolution = 0;
    double y_resolution = 0;
    long width = 0;
    long length = 0;
    long numsamples = 0;
    std::vector<int> bitsPerSample;
    long bitsPerPixel = 0;
    bool planar = false;
    long pixelType = 0;
    long compression = 0;
};

// image handler class that is created after a device acquires images to memory.
// Note that this has only been tested in Windows, as it uses the Device Independent
// Bitmap (DIB) type.  
class image_handler
{
    using images_vector = std::vector<std::vector<HANDLE>>;
    std::shared_ptr<images_vector> vect_image_handle_ptr;
    std::vector<HANDLE> dummy;
    bool m_bAutoDestroy;

public:
    image_handler(bool containsImages = true);
    size_t get_num_acquisitions() const;
    image_handler& set_auto_destroy(bool b = true);
    size_t get_num_pages(size_t acq_number) const;
    std::vector<HANDLE>& get_acquisition_images(size_t acq_number);
    std::vector<unsigned char> get_image_as_BMP(size_t acquition, size_t page);
    static std::vector<unsigned char> get_image_as_BMP(HANDLE hDib);
    // Return a specific DIB
    HANDLE get_image_handle(size_t acquisition, size_t page);
    void add_new_page();
    void push_back_image(HANDLE h);
    void destroy_image_handles();
    ~image_handler();
};
