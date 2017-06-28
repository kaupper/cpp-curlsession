#include "RequestParams.h"

using namespace curl;


static std::string UrlEncode(const std::vector<KeyValuePair> &map)
{
    std::string result = "";
    char *first, *second;
    
    for (size_t i = 0; i < map.size(); ++i) {
        auto &pair = map[i];
        first = curl_easy_escape(nullptr, pair.key.c_str(), 0);
        second = curl_easy_escape(nullptr, pair.value.c_str(), 0);
        result += std::string(first) + '=' + std::string(second);
        
        if (i < map.size() - 1) {
            result += '&';
        }
        
        curl_free(first);
        curl_free(second);
    }
    
    return result;
}


RequestParams &RequestParams::SetUrl(const std::string &url)
{
    this->url = url;
    return *this;
}

RequestParams &RequestParams::SetHeaders(const std::vector<Header> &headers)
{
    this->headers = headers;
    return *this;
}

RequestParams &RequestParams::SetParams(const json &params)
{
    type = Type::JSON;
    this->params = params.dump();
    return *this;
}

RequestParams &RequestParams::SetParams(const std::vector<KeyValuePair> &params)
{
    type = Type::URL;
    this->params = UrlEncode(params);
    return *this;
}

RequestParams &RequestParams::SetParams(const std::string &params)
{
    type = Type::NOTHING;
    this->params = params;
    return *this;
}

RequestParams &RequestParams::SetParams(const std::vector<KeyValuePair> &params,
                                        const std::vector<File> &pathnames)
{
    type = Type::MULTIPART;
    files = pathnames;
    multipartParams = params;
    return *this;
}

RequestParams &RequestParams::SetParams(const json &params,
                                        const std::vector<File> &pathnames)
{
    std::vector<KeyValuePair> mappedParams;
    
    for (auto it = params.begin(); it != params.end(); ++it) {
        std::string value = it->dump();
        
        if (it->is_string()) {
            mappedParams.push_back({it.key(), value.substr(1, value.size() - 2)});
        } else {
            mappedParams.push_back({it.key(), value});
        }
    }
    
    return SetParams(mappedParams, pathnames);
}


RequestParams &RequestParams::SetMethod(const Method &method)
{
    this->method = method;
    return *this;
}

RequestParams &RequestParams::EnableCookies(const bool &cookies)
{
    this->cookies = cookies;
    return *this;
}

RequestParams &RequestParams::FollowRedirects(const bool &follow)
{
    this->followRedirects = follow;
    return *this;
}


std::string RequestParams::GetUrl() const
{
    return url;
}

std::vector<Header> RequestParams::GetHeaders() const
{
    return headers;
}

std::string RequestParams::GetParams() const
{
    return params;
}

std::vector<File> RequestParams::GetFiles() const
{
    return files;
}

std::vector<KeyValuePair> RequestParams::GetMultipartParams() const
{
    return multipartParams;
}

Method RequestParams::GetMethod() const
{
    return method;
}

Type RequestParams::GetType() const
{
    return type;
}

bool RequestParams::CookiesEnabled() const
{
    return cookies;
}

bool RequestParams::FollowRedirects() const
{
    return followRedirects;
}