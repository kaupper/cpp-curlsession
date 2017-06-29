#ifndef CURL_COMMONS_H_
#define CURL_COMMONS_H_

#include <vector>
#include <string>
#include <stdexcept>

#include "json.h"


namespace curlsession
{
    namespace util
    {
    
        static inline void ltrim(std::string &s)
        {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                            std::not1(std::ptr_fun<int, int>(std::isspace))));
        }
        
        static inline void rtrim(std::string &s)
        {
            s.erase(std::find_if(s.rbegin(), s.rend(),
                                 std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        }
        
        static inline void trim(std::string &s)
        {
            ltrim(s);
            rtrim(s);
        }
        
        template <typename Cont> inline typename Cont::value_type GetByName(
                    const std::string &name,
                    const Cont &container,
                    const std::string &error)
        {
            auto it = std::find_if(container.cbegin(), container.cend(), [&name](auto & a) {
                return name == a.name;
            });
            
            if (it == container.cend()) {
                throw std::out_of_range(error + " '" + name + "' not found!");
            }
            
            return *it;
        }
    }
    
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
        std::string name;
        std::string path;
    };
    
    struct Parameter {
        std::string name;
        std::string value;
    };
    
    struct Header {
        std::string name;
        std::string value;
        
        static Header FromString(const std::string &s);
        std::string ToString() const;
    };
    
    using Attribute = Parameter;
    
    struct Cookie {
        std::string name;
        std::string value;
        std::vector<Attribute> attributes;
        
        Attribute GetAttributeByName(const std::string &name) const;
        
        static Cookie FromHeaderString(const std::string &s);
        static Cookie FromNetscapeString(const std::string &s);
        std::string ToString() const;
    };
    
    struct Response {
        int code;
        std::string reason;
        std::string version;
        
        std::vector<Header> headers;
        std::string content;
        std::vector<Cookie> cookies;
        
        Cookie GetCookieByName(const std::string &name) const;
        Header GetHeaderByName(const std::string &name) const;
    };
    
    struct Request {
        std::string url;
        std::vector<Header> headers;
        std::string params;
        
        std::vector<File> files;
        std::vector<Parameter> multipartParams;
        
        Method method;
        Type type;
        bool cookies;
        bool followRedirects;
        bool setContentType;
        
        Header GetHeaderByName(const std::string &name) const;
        File GetFileByName(const std::string &name) const;
        Parameter GetMultipartParameterByName(const std::string &name) const;
        
        std::string GetURL() const;
    };
}

#endif // CURL_COMMONS_H_