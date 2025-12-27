# sys/time.h is there on Linux, but not on Windows 
if (WIN32)
    target_sources(${libName}-esp32 PUBLIC time.h PRIVATE time.cpp)
    target_sources(${libName}-esp8266 PUBLIC time.h PRIVATE time.cpp)   
    install (FILES time.h DESTINATION include/sys)
endif()