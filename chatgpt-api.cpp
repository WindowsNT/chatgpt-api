#include <iostream>
#include <Windows.h>
#include <wininet.h>
#include <vector>
#include <optional>
#include <string>
#include <sstream>
#include <map>
#include <memory>
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"wininet.lib")

#include "chatgpt.hpp"

int main()
{
    WSADATA w = {};
    WSAStartup(MAKEWORD(2, 2), &w);
    CHATGPT_API c("Your_API_Key");
    c.SetModel("code-davinci-002");
    for (;;)
    {  
        std::string input;
        std::cout << "Enter question:";
        std::getline(std::cin, input);
        if (input.empty())
            break;
        auto off = c.Text(input.c_str());
        if (!off.has_value())
            continue;
        auto& r = off.value();
        std::cout << r.t << std::endl;
    }
}

