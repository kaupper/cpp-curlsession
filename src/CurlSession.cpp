#include "CurlSession.h"

using namespace curl;


CurlSession::StaticConstructor::StaticConstructor()
{
    curl_global_init(CURL_GLOBAL_ALL);
}

CurlSession::CurlSession()
{ 
    curl_handle = curl_easy_init(); 
    curl_easy_setopt(curl_handle, CURLOPT_COOKIEFILE, "");
    curl_easy_setopt(curl_handle, CURLOPT_COOKIEJAR, ""); 
}

CurlSession::~CurlSession()
{
    curl_easy_cleanup(curl_handle); 
    curl_handle = 0x0;
}

size_t CurlSession::WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userPointer)
{
    std::vector<char> *result = (std::vector<char> *) userPointer;
    result->insert(result->end(), (char *)contents, (char *)contents + (size * nmemb));
    return size * nmemb;
}

size_t CurlSession::WriteHeaderCallback(void *contents, size_t size, size_t nmemb, void *userPointer)
{
    std::map<std::string, std::string> *map = (std::map<std::string, std::string> *) userPointer;
    std::string header((const char *) contents, size * nmemb);
    
    auto splitPosition = header.find(":");
    std::string value, key;
    if (splitPosition != std::string::npos) {
        value = header.substr(splitPosition + 1);
        key = header.substr(0, splitPosition);
        trim(value);
        trim(key);
        (*map)[key] = value;
    } else if(map->find("statusLine") == map->end()) {
        trim(header);
        (*map)["statusLine"] = header;
    }
    
    return size * nmemb;
}


std::vector<std::string> CurlSession::ExtractCookies(CURL *curl_handle)
{
    std::vector<std::string> resultingCookies; 
    struct curl_slist *cookies = {0};
    struct curl_slist *nc;
    CURLcode res;

    res = curl_easy_getinfo(curl_handle, CURLINFO_COOKIELIST, &cookies);
    if(res != CURLE_OK) {
        return resultingCookies;
    }

    nc = cookies;
    while(nc) {
        resultingCookies.push_back(nc->data);
        nc = nc->next;
    }
    curl_slist_free_all(cookies);
    
    return resultingCookies;
}

std::string CurlSession::GetCookieByName(const std::string &name, const std::vector<std::string> &cookies)
{
	size_t last = 0; 
	size_t next = 0; 
    for(auto &cookie : cookies) {
	    last = next = 0;
        std::vector<std::string> splitted;
        
        const std::string delimiter = "\t";
        int count = 0;
        
        while ((next = cookie.find(delimiter, last)) != std::string::npos) { 
            if(count >= 5) {
                splitted.push_back(cookie.substr(last, next-last)); 
            }
            count++;
            last = next + 1; 
        } 
        splitted.push_back(cookie.substr(last));
        
        if (splitted[0] == name) {
            return splitted[1];
        }
    }   
    return "";
}


Response CurlSession::DoSingleRequest(RequestParams &&params)
{
    return DoSingleRequest(params);
}

Response CurlSession::DoSingleRequest(RequestParams &params)
{
    return CurlSession().DoRequest(params);
}


Response CurlSession::DoRequest(RequestParams &&params)
{
    return DoRequest(params);
}

Response CurlSession::DoRequest(RequestParams &requestParams)
{
    static std::mutex mutex;
    auto &url = requestParams.GetUrl();
    auto &headers = requestParams.GetHeaders();
    auto &params = requestParams.GetParams();
    auto &method = requestParams.GetMethod();
    
    Response response;
    mutex.lock();
    if(curl_handle) {
        std::vector<char>  &responseContent = response.content;
        std::map<std::string, std::string> &responseHeaders = response.headers;
        CURLcode res; 
        
        // append headers
        struct curl_slist *header = NULL;
        struct curl_httppost *formpost = nullptr;
        
        for(auto& entry : headers) {
            if(entry.first == "Content-Type") {
                switch(requestParams.GetType()) {
                    case Type::URL:
                        entry.second = "application/x-www-form-urlencoded";
                        break;
                    case Type::JSON:
                        entry.second = "application/json";
                        break;
                    case Type::MULTIPART:
                        entry.second = "multipart/form-data";
                        break;
                    case Type::NOTHING:
                    default:
                        break;
                }
            }
            std::string headerLine = entry.first + ": " + entry.second;
            header = curl_slist_append(header, headerLine.c_str());
        }
        curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, header);
 
        // set url depending on method
        if(method == Method::POST) {
            if(requestParams.GetType() != Type::MULTIPART) {
                curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, params.c_str());
                curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, params.size());
            } else {
                // set multipart form data
                struct curl_httppost *lastptr = nullptr;
                
                for(auto &file : requestParams.GetFiles()) {
                    curl_formadd(&formpost,
                                 &lastptr,
                                 CURLFORM_COPYNAME, file.first.c_str(),
                                 CURLFORM_FILE, file.second.c_str(),
                                 CURLFORM_END);
                }
                
                for(auto &param : requestParams.GetMultipartParams()) {
                    curl_formadd(&formpost,
                                 &lastptr,
                                 CURLFORM_COPYNAME, param.first.c_str(),
                                 CURLFORM_COPYCONTENTS, param.second.c_str(),
                                 CURLFORM_END);
                }
                curl_easy_setopt(curl_handle, CURLOPT_HTTPPOST, formpost);
            }
            curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
        } else if(method == Method::GET) {
            curl_easy_setopt(curl_handle, CURLOPT_URL, (url + '?' + params).c_str());
        }
        
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) &responseContent);
        curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, WriteHeaderCallback);
        curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, (void *) &responseHeaders);
        
        if(requestParams.FollowRedirects()) {
            curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
        }
        //curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/52.0.2743.116 Safari/537.36");
        
		res = curl_easy_perform(curl_handle);
        curl_slist_free_all(header);
        if(formpost != nullptr) {
            curl_formfree(formpost);
        }
        
        if(res == CURLE_OK && requestParams.CookiesEnabled()) {
            response.cookies = ExtractCookies(curl_handle);
		}
	}
    mutex.unlock();
    
    lastResponse = response;
    return response;
}
