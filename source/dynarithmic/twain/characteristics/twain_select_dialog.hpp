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
// TWAIN characteristics when a session has been started
#ifndef TWAIN_SELECT_DIALOG_HPP
#define TWAIN_SELECT_DIALOG_HPP

#include <string>
#include <vector>
#include <utility>
#include <iterator>
#include <algorithm>
#include <dtwain.h>

namespace dynarithmic
{
    namespace twain {
        /**
            The twain_select_dialog class describes the attributes of the TWAIN "Select Source" dialog that is 
            shown when dynarithmic::twain::twain_session::select_source() is invoked.<br>
            To obtain a reference to a TWAIN Session's dialog box class, use dynarithmic::twain::twain_session::get_twain_characteristics().<br>

            There are two different dialog boxes that can be shown:
            
            -- The **legacy** dialog box is the one that already exists in the TWAIN Data Source Manager system.  This dialog
               cannot be changed (at least cannot be changed easily).  The dialog box cannot be positioned on the screen, cannot
               have the title changed, etc.  If your application desires to use the legacy dialog box:

         *   \code {.cpp}
         *       twain_session session;
         *       auto& dialog = session.get_twain_characteristics().get_twain_dialog();
         *       dialog.set_flags({twain_select_dialog::uselegacy}); 
         *       session.start();
         *       session.select_source(); // will show the legacy dialog
         *   \endcode
          
            -- The **custom** dialog box is the one that can be controlled by the application.  The dialog can be positioned on
               the screen, title changed, etc.  By default, the legacy TWAIN Select Source dialog will always be shown.<br>

         *   \code {.cpp}
         *       twain_session session;
         *       auto& dialog = session.get_twain_characteristics().get_twain_dialog();
         *       dialog.set_flags({twain_select_dialog::sortnames, twain_select_dialog::showcenterscreen});
         *       dialog.set_title("This is a custom title");
         *       session.start();
         *       session.select_source(); // will show the dialog centered, with sorted names and custom title
         *   \endcode
         * If no options are set, and the custom dialog is displayed, it will default to the position of being centered on the screen.<br>
         * If the legacy dialog is chosen, then the position is not controllable programatically, and the position that the TWAIN DSM uses is used.
        */
        class twain_select_dialog
        {
            std::string m_Title;
            std::pair<int, int> m_pos;
            std::vector<int> m_flags;
#ifdef _WIN32
            HFONT m_hFont = NULL;
            HWND m_hWndParent = NULL;
#endif  
        public:
            static constexpr int showcenter = DTWAIN_DLG_CENTER; /*! Show TWAIN Select Source dialog centered on the parent window */
            static constexpr int showcenterscreen = DTWAIN_DLG_CENTER_SCREEN; /*! Show TWAIN Select Source dialog centered on the entire screen */
            static constexpr int sortnames = DTWAIN_DLG_SORTNAMES; /*! Show TWAIN Select Source dialog with sorted Product Names */
            static constexpr int horzscroll = DTWAIN_DLG_HORIZONTALSCROLL; /*! Show TWAIN Select Source dialog with a horizontal scroll bar */
            static constexpr int useposition = 128; /*! Use the position defined by twain_select_dialog::get_position() */
            static constexpr int uselegacy = 512; /*! Use the legacy (default) TWAIN Select Source dialog. Customization is not possible if this flag is set */
            static constexpr int none = 1024; /*! No options are set (in essence, use the legacy version) */

            /// Default constructor.
            /// 
            /// The default constructor defaults to using the custom dialog with the dialog being centered on the screen when displayed
            twain_select_dialog() : m_Title("TWAIN Select Source"), m_pos{}, m_flags{ twain_select_dialog::none, twain_select_dialog::showcenterscreen } {}

            /// Sets the dialog's title
            /// 
            /// @returns The reference to the current object (**this**)
            /// @param[in] title The title to display in the TWAIN Select Source dialog
            twain_select_dialog& set_title(const std::string& title) { m_Title = title; return *this; }

            /// Sets the non-legacy dialog's position on the screen using absolute row and column positioning.  
            /// The positioning is oriented from the upper left corner of the display being row 0, column 0, and
            /// the lower right corner of the screen being (maximum row, maximum column).
            /// 
            /// set.  
            /// \code{ .cpp }
            /// twain_session session;
            /// auto& dialog = session.get_twain_dialog();
            /// // positions the dialog at row 100, column 100 and sets the title
            /// dialog.set_position({100,100}).set_title("This is a custom title").set_flags({twain_select_dialog::useposition}); 
            /// session.start();
            /// session.select_source(); // will show the dialog at position 100, 100 on the screen
            /// \endcode
            /// @returns The reference to the current object (**this**)
            /// @param[in] A std::pair<int, int> denoting the row and column to set the dialog
            /// @see set_flags()
            /// @note The position is overridden if the flags (see set_flags()) has the twain_select_dialog::showcenterscreen 
            ///       or twain_select_dialog::showcenter enabled
            twain_select_dialog& set_position(std::pair<int, int> pos)  noexcept { m_pos = pos; return *this; }

            /// Sets the dialog flags that will be used when the "Select Source" dialog is shown
            /// 
            /// \code{ .cpp }
            /// twain_session session;
            /// auto& dialog = session.get_twain_characteristics().get_twain_dialog();
            /// dialog.set_flags({ twain_select_dialog::sortnames, twain_select_dialog::showcenter });
            /// dialog.set_title("This is a custom title");
            /// session.start();
            /// session.select_source(); // will show the dialog centered, with sorted names and custom title
            /// \endcode
            /// @returns The reference to the current object (**this**)
            /// @param[in] flags An STL-compliant container that contains a list of all the flags to set
            template <typename Container = std::vector<int>>
            twain_select_dialog& set_flags(const Container& flags)
            {
                m_flags.clear();
                std::copy(flags.begin(), flags.end(), std::inserter(m_flags, m_flags.begin()));
                return *this;
            }

            /// Gets the title to be used when the TWAIN Select Source dialog is shown
            /// 
            /// @returns The title that will be used when displaying the custom dialog
            /// @see set_title()
            std::string get_title() const { return m_Title; }

            /// Gets the title to be used when the TWAIN Select Source dialog is shown
            /// 
            /// @returns The position that will be used when displaying the custom dialog
            /// @see set_title()
            std::pair<int, int> get_position() const  noexcept { return m_pos; }

            /// Gets the flags to be used when the TWAIN Select Source dialog is shown
            /// 
            /// @returns The flags used for the TWAIN Select Source dialog
            /// @see set_flags()
            std::vector<int> get_flags() const { return m_flags; }

#ifdef _WIN32
            /// Sets the dialog font to use.
            /// @returns The reference to the current object (**this**)
            /// @param[in] hFont A windows HFONT to use 
            twain_select_dialog& set_font(HFONT hFont) noexcept { m_hFont = hFont; return *this; }

            /// Returns the font used by the TWAIN Select Source dialog
            /// @returns HFONT describing the font used by the Select Source dialog
            /// @see set_font()
            HFONT get_font() const noexcept { return m_hFont; }

            /// Sets the parent window for the TWAIN Select Source
            /// 
            /// Sets the parent window when determining how to center the TWAIN Select Source dialog when the 
            /// twain_select_dialog::showcenter is enabled.
            /// @returns The reference to the current object (**this**)
            /// @note if the **hWndParent** is NULL, then the entire display will be the parent display
            /// @see set_font() set_flags()
            twain_select_dialog& set_parent_window(HWND hWndParent) noexcept { m_hWndParent = hWndParent; return *this; }

            /// Gets the parent window for the TWAIN Select Source
            /// 
            /// @returns The handle to the parent window, or NULL if no parent window
            /// @see set_parent_window()
            HWND get_parent_window() const noexcept { return m_hWndParent; }
#endif  
        };
    }
}
#endif
