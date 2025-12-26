#include <WiFi.h>

namespace BearSSL {
    class X509List {
    public:
        explicit X509List(const char* pem) {
            (void)pem; // ignore certificate content
        }
    };
}

using ::BearSSL::X509List;