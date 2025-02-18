cmake_minimum_required(VERSION 3.16)



if(WIN32)
    set(VCPKG_TARGET_TRIPLET x64-windows)
elseif(APPLE)
    set(VCPKG_TARGET_TRIPLET arm64-osx)
endif()

include("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

project(blockchain VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)
find_package(OpenSSL REQUIRED)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appblockchain
    main.cpp
    src/HashTools/hashtools.cpp
    src/wallet/wallet.cpp
)

qt_add_qml_module(appblockchain
    URI blockchain
    VERSION 1.0
    QML_FILES
        Main.qml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appblockchain PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appblockchain
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_include_directories(appblockchain PRIVATE src/HashTools src/Wallet)
target_link_libraries(appblockchain PRIVATE Qt6::Quick OpenSSL::SSL OpenSSL::Crypto)

include(GNUInstallDirs)
install(TARGETS appblockchain
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
