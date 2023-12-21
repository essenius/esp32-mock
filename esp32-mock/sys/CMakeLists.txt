# sys/time.h is there on Linux, but not on Windows 
if (WIN32)
    target_sources(${libName} PUBLIC time.h PRIVATE time.cpp)
    install (FILES time.h DESTINATION include/sys)
endif()