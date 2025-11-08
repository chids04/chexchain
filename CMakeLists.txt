cmake_minimum_required(VERSION 3.16)

if(WIN32)
    set(VCPKG_TARGET_TRIPLET x64-windows)
elseif(APPLE)
    set(VCPKG_TARGET_TRIPLET arm64-osx)
endif()

project(blockchain VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)
find_package(OpenSSL REQUIRED)

qt_standard_project_setup(REQUIRES 6.10)

qt_add_executable(appblockchain
    src/main.cpp

    src/HashTools/hashtools.cpp
    src/HashTools/hashtools.h

    src/Wallet/wallet.cpp
    src/Wallet/wallet.h

    src/Blockchain/blockchain.h
    src/Blockchain/blockchain.cpp
    src/Blockchain/miner.h


    src/Block/block.h
    src/Block/block.cpp

    src/Transaction/transaction.h
    src/Transaction/transaction.cpp
)

qt_add_qml_module(appblockchain
    URI blockchain
    VERSION 1.0
    QML_FILES
        ui/Main.qml

    SOURCES
        src/BlockchainApp/blockchainapp.cpp
        src/BlockchainApp/blockchainapp.h
)

if(WIN32)
    add_custom_target(kill_appblockchain
        COMMAND taskkill /IM appblockchain.exe /F || exit 0
        COMMENT "Killing previous instance of appblockchain.exe"
    )
else()  # Linux/macOS
    add_custom_target(kill_appblockchain
        COMMAND killall -9 appblockchain || true
        COMMENT "Killing previous instance of appblockchain"
    )
endif()

# Make the apphex-launcher target depend on the kill target
add_dependencies(appblockchain kill_appblockchain)



target_include_directories(appblockchain PRIVATE
    src
    src/HashTools
    src/Wallet
    src/Blockchain
    src/Block
    src/BlockchainApp
    src/Transaction
    src/Utility)
target_link_libraries(appblockchain PRIVATE Qt6::Quick OpenSSL::SSL OpenSSL::Crypto)

include(GNUInstallDirs)
install(TARGETS appblockchain
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
