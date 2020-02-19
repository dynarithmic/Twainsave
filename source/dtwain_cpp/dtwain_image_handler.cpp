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
#include "dtwain_cpp_interface.hpp"

namespace dynarithmic
{
    namespace twain
    {
        image_handler::image_handler(bool containsImages) : vect_image_handle_ptr(
                                                                containsImages ? new image_handler::images_vector : nullptr),
                                                            m_bAutoDestroy(false)
        {
        }

        size_t image_handler::get_num_acquisitions() const { return vect_image_handle_ptr->size(); }

        image_handler& image_handler::set_auto_destroy(bool b)
        {
            m_bAutoDestroy = b;
            return *this;
        }

        size_t image_handler::get_num_pages(size_t acq_number) const
        {
            if (acq_number >= vect_image_handle_ptr->size())
                return 0;
            return (*vect_image_handle_ptr)[acq_number].size();
        }

        std::vector<HANDLE>& image_handler::get_acquisition_images(size_t acq_number)
        {
            if (get_num_pages(acq_number) == 0)
                return dummy;
            return (*vect_image_handle_ptr)[acq_number];
        }

        // Return a specific DIB
        HANDLE image_handler::get_image_handle(size_t acquisition, size_t page)
        {
            auto& images = get_acquisition_images(acquisition);
            if (page < images.size())
                return images[page];
            return nullptr;
        }

        static int CalculateUsedPaletteEntries(int bit_count) {
            if ((bit_count >= 1) && (bit_count <= 8))
                return 1 << bit_count;
            return 0;
        }

        std::vector<unsigned char> image_handler::get_image_as_BMP(HANDLE hDib)
        {
            std::vector<unsigned char> retval;
            if (!hDib)
                return retval;
            BITMAPFILEHEADER fileheader;
            LPBITMAPINFOHEADER lpbi = NULL;
            memset((char *)&fileheader, 0, sizeof(BITMAPFILEHEADER));

            fileheader.bfType = 'MB';

            /* Fill in the fields of the file header */
            BYTE *pImage2 = (BYTE *)GlobalLock(hDib);

            lpbi = reinterpret_cast<LPBITMAPINFOHEADER>(pImage2);

            fileheader.bfSize = (DWORD)GlobalSize(hDib) + sizeof(BITMAPFILEHEADER);
            fileheader.bfReserved1 = 0;
            fileheader.bfReserved2 = 0;
            fileheader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) +
                lpbi->biSize + CalculateUsedPaletteEntries(lpbi->biBitCount) * sizeof(RGBQUAD);

            // Write the file header
            char *ptrFileheader = reinterpret_cast<char *>(&fileheader);
            std::copy(ptrFileheader, ptrFileheader + sizeof(BITMAPFILEHEADER), std::back_inserter(retval));

            // Write the data
            DWORD gSize = static_cast<DWORD>(GlobalSize(hDib));
            std::copy(pImage2, pImage2 + gSize, std::back_inserter(retval));
            return retval;
        }

        // Return a DIB as a BMP in memory
        std::vector<unsigned char> image_handler::get_image_as_BMP(size_t acquisition, size_t page)
        {
            return get_image_as_BMP(get_image_handle(acquisition, page));
        }

        void image_handler::add_new_page()
        {
            vect_image_handle_ptr->resize(vect_image_handle_ptr->size() + 1);
        }

        void image_handler::push_back_image(HANDLE h)
        {
            vect_image_handle_ptr->back().push_back(h);
        }

        void image_handler::destroy_image_handles()
        {
            if (vect_image_handle_ptr)
            {
                auto iter = vect_image_handle_ptr->begin();
                while (iter != vect_image_handle_ptr->end())
                {
                    auto& vImages = *iter;
                    auto inner = vImages.begin();
                    while (inner != vImages.end())
                    {
                        ::GlobalUnlock(*inner);
                        ::GlobalFree(*inner);
                        ++inner;
                    }
                    ++iter;
                }
                vect_image_handle_ptr->clear();
            }
        }

        image_handler::~image_handler()
        {
            if (vect_image_handle_ptr.use_count() == 1)
                destroy_image_handles();
        }
    };
}
