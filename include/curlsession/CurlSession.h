#ifndef REQUESTS_H_
#define REQUESTS_H_

#include <vector>
#include <string>
#include <mutex>
#include <algorithm>
#include <stdexcept>

#include "curl/curl.h"

#include "entities.h"
#include "RequestBuilder.h"


namespace curlsession
{
    class CurlSession
    {
        private:
            static size_t WriteMemoryCallback(void *contents, size_t size,
                                              size_t nmemb, void *userPointer);
            static size_t WriteHeaderCallback(void *contents, size_t size,
                                              size_t nitems, void *userPointer);
                                              
            CURL *curl_handle;
            Response lastResponse;
            
        public:
            CurlSession();
            ~CurlSession();
            
            static void Cleanup();
            
            
            static Response DoSingleRequest(Request &&);
            static Response DoSingleRequest(Request &);
            
            Response DoRequest(Request &&);
            Response DoRequest(Request &);
            
            Response GetLastResponse() const;
            
            std::vector<Cookie> GetCookies() const;
            Cookie GetCookieByName(const std::string &name) const;
    };
}

#endif // REQUESTS_H_
