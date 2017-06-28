#ifndef REQUEST_PARAMS_H_
#define REQUEST_PARAMS_H_

#include <string>
#include <map>

#include "json.h"
#include "curl/curl.h"

#include "Commons.h"


namespace curl
{
    class RequestParams
    {
        public:
            std::string url;
            std::vector<Header> headers;
            std::string params;
            
            std::vector<File> files;
            std::vector<KeyValuePair> multipartParams;
            
            Method method;
            Type type;
            bool cookies;
            bool followRedirects;
            
            
            RequestParams(const std::string &url) : RequestParams(url, Method::GET) {}
            RequestParams(const std::string &url, const Method &method)
                : url(url), headers(), params(), files(), multipartParams(), method(method),
                  type(Type::NOTHING), cookies(false), followRedirects(false) {}
                  
            RequestParams &SetUrl(const std::string &url);
            RequestParams &SetHeaders(const std::vector<Header> &headers);
            
            // set params as JSON
            RequestParams &SetParams(const json &params);
            // set params URL encoded
            RequestParams &SetParams(const std::vector<KeyValuePair> &params);
            // set params as multipart/form-data
            RequestParams &SetParams(const std::vector<KeyValuePair> &params,
                                     const std::vector<File> &pathnames);
            RequestParams &SetParams(const json &params,
                                     const std::vector<File> &pathnames);
            // copy params without further processing
            RequestParams &SetParams(const std::string &params);
            
            RequestParams &SetMethod(const Method &method);
            RequestParams &EnableCookies(const bool &cookies);
            RequestParams &FollowRedirects(const bool &cookies);
            
            
            std::string GetUrl() const;
            std::vector<Header> GetHeaders() const;
            std::string GetParams() const;
            std::vector<File> GetFiles() const;
            std::vector<KeyValuePair> GetMultipartParams() const;
            Method GetMethod() const;
            Type GetType() const;
            bool CookiesEnabled() const;
            bool FollowRedirects() const;
    };
}

#endif // REQUEST_PARAMS_H_
