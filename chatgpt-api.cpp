
#include "chatgpt.hpp"

int main()
{
    WSADATA w = {};
    WSAStartup(MAKEWORD(2, 2), &w);
    CHATGPT_API c("api_key");
    c.SetModel("code-davinci-002");
    for (;;)
    {  
        std::string input;
        std::cout << "Enter question:";
        std::getline(std::cin, input);
        if (input.empty())
            break;
//        auto off2 = c.Image(input.c_str());
        auto off = c.Text(input.c_str());
        if (!off.has_value())
            continue;
        auto& r = off.value();
        std::cout << r.t << std::endl;
    }
}

