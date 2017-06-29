#include "CurlSession.h"

using namespace curlsession;


static std::vector<Cookie> ExtractCookies(CURL *curl_handle)
{
    std::vector<Cookie> resultingCookies;
    struct curl_slist *cookies = {0};
    struct curl_slist *nc;
    CURLcode res;
    res = curl_easy_getinfo(curl_handle, CURLINFO_COOKIELIST, &cookies);
    
    if (res != CURLE_OK) {
        return resultingCookies;
    }
    
    nc = cookies;
    
    while (nc) {
        resultingCookies.push_back(Cookie::FromNetscapeString(nc->data));
        nc = nc->next;
    }
    
    curl_slist_free_all(cookies);
    return resultingCookies;
}


CurlSession::CurlSession()
{
    static bool inited = false;
    
    if (!inited) {
        inited = true;
        curl_global_init(CURL_GLOBAL_ALL);
    }
    
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_COOKIEFILE, "");
    curl_easy_setopt(curl_handle, CURLOPT_COOKIEJAR, "");
}

CurlSession::~CurlSession()
{
    curl_easy_cleanup(curl_handle);
    curl_handle = 0x0;
}

void CurlSession::Cleanup()
{
    curl_global_cleanup();
}

size_t CurlSession::WriteMemoryCallback(void *contents, size_t size,
                                        size_t nmemb, void *userPointer)
{
    std::stringstream *result = static_cast<std::stringstream *>(userPointer);
    result->write(static_cast<const char *>(contents), size * nmemb);
    return size * nmemb;
}

size_t CurlSession::WriteHeaderCallback(void *contents, size_t size,
                                        size_t nmemb, void *userPointer)
{
    std::vector<Header> *headers = static_cast<std::vector<Header> *>(userPointer);
    std::string headerStr((const char *) contents, size * nmemb);
    
    try {
        headers->push_back(Header::FromString(headerStr));
    } catch (const std::invalid_argument &ex) {
        if (size * nmemb > 2) {
            headers->clear();
            util::trim(headerStr);
            headers->push_back({"statusLine", headerStr});
        }
    }
    
    return size * nmemb;
}


Response CurlSession::GetLastResponse() const
{
    return lastResponse;
}

Response CurlSession::DoSingleRequest(Request &request)
{
    return CurlSession().DoRequest(request);
}

Response CurlSession::DoSingleRequest(Request &&request)
{
    return DoSingleRequest(request);
}

Response CurlSession::DoRequest(Request &&request)
{
    return DoRequest(request);
}

Response CurlSession::DoRequest(Request &request)
{
    static std::mutex mutex;
    Response response;
    mutex.lock();
    
    if (curl_handle) {
        std::stringstream content;
        CURLcode res;
        // append headers
        struct curl_slist *header = NULL;
        struct curl_httppost *formpost = nullptr;
        
        // set content type if known, not existent and desired
        if (request.type != Type::NOTHING && request.setContentType) {
            try {
                request.GetHeaderByName("Content-Type");
            } catch (const std::out_of_range &ex) {
                Header h;
                h.name = "Content-Type";
                
                switch (request.type) {
                    case Type::URL:
                        h.value = "application/x-www-form-urlencoded";
                        break;
                        
                    case Type::JSON:
                        h.value = "application/json";
                        break;
                        
                    case Type::MULTIPART:
                        h.value = "multipart/form-data";
                        break;
                }
                
                request.headers.push_back(h);
            }
        }
        
        // add all headers
        for (auto &h : request.headers) {
            header = curl_slist_append(header, h.ToString().c_str());
        }
        
        curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, header);
        
        // set url depending on method
        if (request.method == Method::POST) {
            if (request.type != Type::MULTIPART) {
                curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, request.params.c_str());
                curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, request.params.size());
            } else {
                // set multipart form data
                struct curl_httppost *lastptr = nullptr;
                
                for (auto &file : request.files) {
                    curl_formadd(&formpost,
                                 &lastptr,
                                 CURLFORM_COPYNAME, file.name.c_str(),
                                 CURLFORM_FILE, file.path.c_str(),
                                 CURLFORM_END);
                }
                
                for (auto &param : request.multipartParams) {
                    curl_formadd(&formpost,
                                 &lastptr,
                                 CURLFORM_COPYNAME, param.name.c_str(),
                                 CURLFORM_COPYCONTENTS, param.value.c_str(),
                                 CURLFORM_END);
                }
                
                curl_easy_setopt(curl_handle, CURLOPT_HTTPPOST, formpost);
            }
            
            curl_easy_setopt(curl_handle, CURLOPT_URL, request.GetURL().c_str());
            curl_easy_setopt(curl_handle, CURLOPT_POST, 1);
        } else if (request.method == Method::GET) {
            curl_easy_setopt(curl_handle, CURLOPT_HTTPGET, 1);
            curl_easy_setopt(curl_handle, CURLOPT_URL, request.GetURL().c_str());
        }
        
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) &content);
        curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, WriteHeaderCallback);
        curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, (void *) &response.headers);
        
        if (request.followRedirects) {
            curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
        }
        
        //curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT,
                         "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/52.0.2743.116 Safari/537.36");
        res = curl_easy_perform(curl_handle);
        curl_slist_free_all(header);
        
        if (formpost != nullptr) {
            curl_formfree(formpost);
        }
        
        if (res == CURLE_OK && request.cookies) {
            for (auto &h : response.headers) {
                if (h.name == "Set-Cookie") {
                    response.cookies.push_back(Cookie::FromHeaderString(h.value));
                }
            }
        }
        
        // set content
        response.content = content.str();
        // extract status line and delete it from the header vector
        auto statusLineHeader = response.GetHeaderByName("statusLine");
        auto statusLine = statusLineHeader.value;
        auto split = statusLine.find(' ');
        // version
        response.version = statusLine.substr(0, split);
        statusLine = statusLine.substr(split + 1);
        split = statusLine.find(' ');
        // status code
        response.code = std::stoi(statusLine.substr(0, split));
        statusLine = statusLine.substr(split + 1);
        // reason
        response.reason = statusLine;
        // delete from headers
        auto it = std::remove_if(response.headers.begin(),
        response.headers.end(), [](auto & h) {
            return h.name == "statusLine";
        });
        response.headers.resize(std::distance(response.headers.begin(), it));
    }
    
    lastResponse = response;
    mutex.unlock();
    return response;
}


std::vector<Cookie> CurlSession::GetCookies() const
{
    return ExtractCookies(curl_handle);
}

Cookie CurlSession::GetCookieByName(const std::string &name) const
{
    return util::GetByName(name, GetCookies(), "Cookie");
}