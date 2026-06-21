TwainSave CMake presets and batch files
======================================

This package contains CMake presets and simple batch wrappers for:

- Visual Studio 2019, x32 / x64
- Visual Studio 2022, x32 / x64
- Visual Studio 2026, x32 / x64

Each batch file configures the matching preset, then builds:

- MinSizeRel
- Debug

The attached TwainSave CMakeLists.txt already contains the Boost auto-download
logic and maps the Visual Studio toolsets as follows:

- VS2019 / v142 -> Boost msvc-14.2
- VS2022 / v143 -> Boost msvc-14.3
- VS2026 / v145 -> Boost msvc-14.5

The presets provide the normal Boost cache/download options:

- TWAIN_AUTO_DOWNLOAD_BOOST=ON
- TWAIN_BOOST_CACHE_ROOT=$env{SystemDrive}/BoostDeps
- TWAIN_DELETE_BOOST_INSTALLER_AFTER_INSTALL=ON
- TWAIN_DELETE_BOOST_INSTALL_LOG_AFTER_INSTALL=OFF
- TWAIN_EXISTING_BOOST_ROOT=""

To use an existing Boost installation instead, edit the preset and set:

- TWAIN_AUTO_DOWNLOAD_BOOST=OFF
- TWAIN_EXISTING_BOOST_ROOT=<your Boost root>
