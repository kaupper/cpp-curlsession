#include "RequestParams.h"

using namespace curl;

static std::string UrlEncode(const std::map<std::string, std::string> &map)
{
    std::string result = "";
    char *first, *second;
    
    for(auto it = map.begin(); it != map.end(); ++it) {
        first = curl_easy_escape(nullptr, it->first.c_str(), 0);
        second = curl_easy_escape(nullptr, it->second.c_str(), 0);
        
        result += std::string(first) + '=' + std::string(second);
        
        if(std::next(it, 1) != map.end()) {
            result += '&';
        }
        
        curl_free(first);
        curl_free(second);
    }
    return result;
}


RequestParams & RequestParams::SetUrl(const std::string &url)
{
    this->url = url;
    return *this;
}

RequestParams & RequestParams::SetHeaders(const std::map<std::string, std::string> &headers)
{
    this->headers = headers;
    return *this;
}

RequestParams & RequestParams::SetParams(const Serializable &params)
{
    type = Type::JSON;
    this->params = params.Serialize();
    return *this;
}

RequestParams & RequestParams::SetParams(const std::map<std::string, std::string> &params)
{
    type = Type::URL;
    this->params = UrlEncode(params);
    return *this;
}

RequestParams & RequestParams::SetParams(const std::string &params)
{
    type = Type::MULTIPART;
    this->params = params;
    return *this;
}

RequestParams & RequestParams::SetMethod(const Method &method)
{
    this->method = method;
    return *this;
}

RequestParams & RequestParams::EnableCookies(const bool &cookies)
{
    this->cookies = cookies;
    return *this;
}

RequestParams & RequestParams::FollowRedirects(const bool &follow)
{
    this->followRedirects = follow;
    return *this;
}