#ifndef REQUEST_BUILDER_H
#define REQUEST_BUILDER_H

#include <string>

#include "curl/curl.h"
#include "json.h"

#include "Commons.h"


namespace curlsession
{
    class RequestBuilder
    {
            Request request;
            
        public:
            RequestBuilder(const std::string &url);
            RequestBuilder(const std::string &url, const Method &method);
            
            
            RequestBuilder &SetUrl(const std::string &url);
            RequestBuilder &SetHeaders(const std::vector<Header> &headers);
            
            // set params as JSON
            
            RequestBuilder &SetJSONParams(const nlohmann::json &params);
            
            // set params URL encoded
            RequestBuilder &SetURLParams(const std::vector<Parameter> &params);
            
            // set params as multipart/form-data
            RequestBuilder &SetMultipartParams(const std::vector<Parameter> &params,
                                               const std::vector<File> &pathnames);
            RequestBuilder &SetMultipartParams(const nlohmann::json &params,
                                               const std::vector<File> &pathnames);
                                               
            // copy params without further processing
            RequestBuilder &SetParams(const std::string &params);
            
            RequestBuilder &SetMethod(Method method);
            RequestBuilder &EnableCookies();
            RequestBuilder &FollowRedirects();
            RequestBuilder &AutoSetContentType();
            
            Request GetRequest() const;
    };
}

#endif // REQUEST_BUILDER_H
