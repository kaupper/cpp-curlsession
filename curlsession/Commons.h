#ifndef CURL_COMMONS_H_
#define CURL_COMMONS_H_

#include <vector>
#include <map>
#include <string>

namespace curl
{
    enum class Type {
        NOTHING,
        JSON,
        URL,
        MULTIPART
    };
    
    enum class Method {
        GET = 0,
        POST = 1
    };
    
    struct File {
        std::string paramName;
        std::string path;
    };
    
    struct KeyValuePair {
        std::string key;
        std::string value;
    };
    
    typedef KeyValuePair Header;
    
    struct Response {
        std::vector<Header> headers;
        std::vector<char> content;
        std::vector<std::string> cookies;
    };
}

#endif // CURL_COMMONS_H_