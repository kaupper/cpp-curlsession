#ifndef REQUESTS_H_
#define REQUESTS_H_

#include <map>
#include <vector>
#include <string>
#include <mutex>
#include <algorithm>

#include "curl/curl.h"
#include "entities.h"
#include "Commons.h"
#include "RequestParams.h"

static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
    std::not1(std::ptr_fun<int, int>(std::isspace))));
}

static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
    std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}


namespace curl
{   
    class CurlSession
    {
    private:
        struct StaticConstructor
        {
            StaticConstructor();
        };
        static StaticConstructor _;
        
        static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userPointer);
        static size_t WriteHeaderCallback(void *contents, size_t size, size_t nitems, void *userPointer);
        
        static std::vector<std::string> ExtractCookies(CURL *curl_handle);
        
        CURL* curl_handle;
        Response lastResponse;

    public:
        CurlSession();
        virtual ~CurlSession();
        static void Cleanup() { curl_global_cleanup(); }
        
        static std::string GetCookieByName(const std::string &name, const std::vector<std::string> &cookies);
        
        static Response DoSingleRequest(RequestParams &params);
        static Response DoSingleRequest(RequestParams &&params);
        
        const Response& GetLastResponse() { return lastResponse; }
        
        Response DoRequest(RequestParams &params);
        Response DoRequest(RequestParams &&params);
    };
}

#endif // REQUESTS_H_
