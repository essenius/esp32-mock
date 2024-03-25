include(FindGit)
find_package(Git)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG release-1.12.1
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
    INSTALL_COMMAND ""
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

set(testName ${libName}-test)

add_executable(${testName} "")

set(mySources 
    Adafruit_SSD1306Test.cpp 
    EEPROMTest.cpp
    ESPTest.cpp 
    FSTest.cpp
    freeRTOSTest.cpp
    HttpClientTest.cpp 
    HTTPUpdateTest.cpp 
    IPAddressTest.cpp 
    PreferencesTest.cpp
    PubSubClientTest.cpp
    ringbufTest.cpp
    WiFiClientSecureTest.cpp
    WiFiTest.cpp
    WireTest.cpp
)

target_sources (${testName} PRIVATE ${mySources})
target_include_directories(${libName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${safe-cstring_SOURCE_DIR}/src )
target_link_libraries(${testName} ${libName} gtest_main)

add_test(NAME ${testName} COMMAND ${testName})