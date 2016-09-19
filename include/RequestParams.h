#ifndef REQUEST_PARAMS_H_
#define REQUEST_PARAMS_H_

#include <string>
#include <map>

#include "curl/curl.h"

#include "Commons.h"
#include "include/Serializable.h"

namespace curl
{   
    class RequestParams
    {
    private:
        std::string url;
        std::map<std::string, std::string> headers;
        std::string params;
        Method method;
        Type type;
        bool cookies;
        bool followRedirects;
        
    public:
        RequestParams(const std::string &url) : RequestParams(url, Method::GET) {}
        RequestParams(const std::string &url, const Method &method) : url(url), headers({}), params(""), method(method), type(Type::NOTHING), cookies(false), followRedirects(false) {}
        
        RequestParams & SetUrl(const std::string &url);
        RequestParams & SetHeaders(const std::map<std::string, std::string> &headers);
        // set params as JSON
        RequestParams & SetParams(const jsonserializer::Serializable &params);
        // set params URL encoded
        RequestParams & SetParams(const std::map<std::string, std::string> &params);
        // copy params without further processing
        RequestParams & SetParams(const std::string &params);
        RequestParams & SetMethod(const Method &method);
        RequestParams & EnableCookies(const bool &cookies);
        RequestParams & FollowRedirects(const bool &cookies);

        auto & GetUrl() { return url; }
        auto & GetHeaders() { return headers; }
        auto & GetParams() { return params; }
        auto & GetMethod() { return method; }
        auto & GetType() { return type; }
        auto & CookiesEnabled() { return cookies; }
        auto & FollowRedirects() { return followRedirects; }
    };
}

#endif // REQUEST_PARAMS_H_