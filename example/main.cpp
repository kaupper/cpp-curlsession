#include <iostream>

#include "curlsession/CurlSession.h"

using namespace curl;

int main()
{
    auto resp = CurlSession::DoSingleRequest({"google.com"});
    std::string c(resp.content.begin(), resp.content.end());
    std::cout << c << std::endl;
    return 0;
}