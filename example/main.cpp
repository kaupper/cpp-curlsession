#include <iostream>

#include "curlsession/CurlSession.h"

using namespace curlsession;

int main()
{
    auto session = CurlSession();
    auto request = RequestBuilder("example.com")
                   .FollowRedirects()
                   .EnableCookies()
                   .GetRequest();
    auto resp = session.DoRequest(request);
    std::cout << "HTTP Version: " << resp.version << std::endl;
    std::cout << "Status code: " << resp.code << std::endl;
    std::cout << "Status reason: " << resp.reason << std::endl;
    std::cout << std::endl;
    std::cout << "Cookies (formatted as http header ready to send to a server):";
    std::cout << std::endl;
    
    for (auto &c : resp.cookies) {
        std::cout << c.ToString() << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "All session cookies:" << std::endl;
    
    for (auto &c : session.GetCookies()) {
        std::cout << c.ToString() << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Headers:" << std::endl;
    
    for (auto &h : resp.headers) {
        std::cout << h.ToString() << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "HTTP body:" << std::endl;
    std::cout << resp.content << std::endl << std::endl;
    return 0;
}