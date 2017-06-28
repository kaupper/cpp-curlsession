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
        
        std::string GetHeader(const std::string &header)
        {
            auto it = std::find_if(headers.begin(), headers.end(), [&header](auto & e) {
                return header == e.key;
            });
            
            if (it == headers.end()) {
                throw "Header not found!";
            }
            
            return it->value;
        }
    };
}

#endif // CURL_COMMONS_H_