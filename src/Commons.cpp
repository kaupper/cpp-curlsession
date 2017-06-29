#include "Commons.h"

using namespace curlsession;
using namespace curlsession::util;


Attribute Cookie::GetAttributeByName(const std::string &name) const
{
    return GetByName(name, attributes, "Attribute");
}

Cookie Response::GetCookieByName(const std::string &name) const
{
    return GetByName(name, cookies, "Cookie");
}

Header Response::GetHeaderByName(const std::string &name) const
{
    return GetByName(name, headers, "Header");
}


Header Request::GetHeaderByName(const std::string &name) const
{
    return GetByName(name, headers, "Header");
}

File Request::GetFileByName(const std::string &name) const
{
    return GetByName(name, files, "File");
}

Parameter Request::GetMultipartParameterByName(const std::string &name) const
{
    return GetByName(name, multipartParams, "Multipart parameter");
}

Cookie Cookie::FromNetscapeString(const std::string &s)
{
    Cookie c;
    std::vector<std::string> fields;
    std::string tmp = s;
    size_t split = 0;
    
    while ((split = tmp.find('\t')) != std::string::npos) {
        fields.push_back(tmp.substr(0, split));
        tmp = tmp.substr(split + 1);
    }
    
    fields.push_back(tmp);
    
    if ((split = fields[0].find("#HttpOnly_")) == 0) {
        c.attributes.push_back({"HTTPOnly", ""});
        fields[0] = fields[0].substr(split + 1);
    }
    
    c.attributes.push_back({"Domain", fields[0]});
    
    if (fields[1] == "TRUE") {
        c.attributes.push_back({"Flag", ""});
    }
    
    c.attributes.push_back({"Path", fields[2]});
    
    if (fields[3] == "TRUE") {
        c.attributes.push_back({"Secure", ""});
    }
    
    c.attributes.push_back({"Expires", fields[4]});
    c.name = fields[5];
    c.value = fields[6];
    return c;
}

Cookie Cookie::FromHeaderString(const std::string &s)
{
    Cookie c;
    std::string cookie = s;
    auto semicolon = cookie.find(';');
    auto split = cookie.rfind('=', semicolon);
    
    if (split == std::string::npos) {
        throw std::invalid_argument("String '" + s + "' is not a cookie!");
    }
    
    std::string name, value;
    name = cookie.substr(0, split);
    value = cookie.substr(split + 1, semicolon - split - 1);
    trim(name), trim(value);
    c.name = name;
    c.value = value;
    std::string attribute;
    
    while (semicolon != std::string::npos) {
        cookie = cookie.substr(semicolon + 1);
        semicolon = cookie.find(';');
        attribute = cookie.substr(0, semicolon);
        split = attribute.find('=');
        
        if (split == std::string::npos) {
            c.attributes.push_back({attribute, ""});
            continue;
        }
        
        name = attribute.substr(0, split);
        value = attribute.substr(split + 1);
        trim(name), trim(value);
        c.attributes.push_back({name, value});
    }
    
    return c;
}

std::string Cookie::ToString() const
{
    return "Cookie: " + name + "=" + value;
}

Header Header::FromString(const std::string &s)
{
    auto split = s.find(':');
    
    if (split == std::string::npos) {
        throw std::invalid_argument("String '" + s + "' is not a header!");
    }
    
    std::string name, value;
    name = s.substr(0, split);
    value = s.substr(split + 1);
    trim(name), trim(value);
    return {name, value};
}

std::string Header::ToString() const
{
    return name + ": " + value;
}


std::string Request::GetURL() const
{
    if (method == Method::GET) {
        return url + '?' + params;
    } else {
        return url;
    }
}