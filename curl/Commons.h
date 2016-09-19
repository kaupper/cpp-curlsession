#ifndef CURL_COMMONS_H_
#define CURL_COMMONS_H_

#include <vector>
#include <map>
#include <string>

namespace curl
{
    enum class Type
    {
        NOTHING,
        JSON,
        URL,
        MULTIPART
    };
    
    enum class Method
    {
        GET = 0,
        POST = 1
    };
    
    struct Response
    {
        std::map<std::string, std::string> headers;
        std::vector<char> content;
        std::vector<std::string> cookies;
    };
}

#endif // CURL_COMMONS_H_