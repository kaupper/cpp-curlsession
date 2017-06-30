#include "RequestBuilder.h"

using namespace curlsession;
using namespace nlohmann;


static inline std::string to_string(const json &j)
{
    if (j.type() == json::value_t::string) {
        return j.get<std::string>();
    }

    return j.dump();
}

static std::string UrlEncode(const std::vector<Parameter> &map)
{
    std::string result = "";
    char *first, *second;

    for (size_t i = 0; i < map.size(); ++i) {
        auto &pair = map[i];
        first = curl_easy_escape(nullptr, pair.name.c_str(), 0);
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


RequestBuilder::RequestBuilder(const std::string &url)
    : RequestBuilder(url, Method::GET)
{
}


RequestBuilder::RequestBuilder(const std::string &url, const Method &method)
{
    request.url = url;
    request.method = method;
    request.type = Type::NOTHING;
    request.cookies = false;
    request.followRedirects = false;
    request.setContentType = false;
}

RequestBuilder &RequestBuilder::SetUrl(const std::string &url)
{
    request.url = url;
    return *this;
}

RequestBuilder &RequestBuilder::SetHeaders(const std::vector<Header> &headers)
{
    request.headers = headers;
    return *this;
}

RequestBuilder &RequestBuilder::SetJSONParams(const json &params)
{
    request.type = Type::JSON;
    request.params = params.dump();
    return *this;
}

RequestBuilder &RequestBuilder::SetURLParams(const std::vector<Parameter>
        &params)
{
    request.type = Type::URL;
    request.params = UrlEncode(params);
    return *this;
}

RequestBuilder &RequestBuilder::SetParams(const std::string &params)
{
    request.type = Type::NOTHING;
    request.params = params;
    return *this;
}

RequestBuilder &RequestBuilder::SetMultipartParams(
            const std::vector<Parameter> &params,
            const std::vector<File> &pathnames)
{
    request.type = Type::MULTIPART;
    request.files = pathnames;
    request.multipartParams = params;
    return *this;
}

RequestBuilder &RequestBuilder::SetMultipartParams(const json &params,
        const std::vector<File> &pathNames)
{
    std::vector<Parameter> mappedParams;

    for (auto it = params.begin(); it != params.end(); ++it) {
        mappedParams.push_back({it.key(), to_string(*it)});
    }

    return SetMultipartParams(mappedParams, pathNames);
}


RequestBuilder &RequestBuilder::SetMethod(Method method)
{
    request.method = method;
    return *this;
}

RequestBuilder &RequestBuilder::EnableCookies()
{
    request.cookies = true;
    return *this;
}

RequestBuilder &RequestBuilder::FollowRedirects()
{
    request.followRedirects = true;
    return *this;
}

RequestBuilder &RequestBuilder::AutoSetContentType()
{
    request.setContentType = true;
    return *this;
}

Request RequestBuilder::GetRequest() const
{
    return request;
}
