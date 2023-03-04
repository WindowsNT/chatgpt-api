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

#include "rest.h"
#include "jsonxx.h"

struct CHATGPT_RESULT
{
    jsonxx::Object o;
    std::string t;
    std::vector<char> data;
};

std::vector<char> Fetch(const char* TheLink)
{
    std::vector<char> aaaa;
    // Create thread that will show download progress
    DWORD Size;
    unsigned long bfs = 1000;
    TCHAR ss[1000];
    DWORD TotalTransferred = 0;

    int err = 1;

    HINTERNET hI = 0, hRead = 0;

    hI = InternetOpen(L"ChatGPT-API", INTERNET_OPEN_TYPE_DIRECT, 0, 0, 0);
    if (hI)
    {
        hRead = InternetOpenUrlA(hI, TheLink, 0, 0, INTERNET_FLAG_NO_CACHE_WRITE, 0);
        if (hRead)
        {

            if (!HttpQueryInfo(hRead, HTTP_QUERY_CONTENT_LENGTH, ss, &bfs, 0))
                Size = (DWORD)-1;
            else
                Size = _wtoi(ss);

            for (;;)
            {
                DWORD n;
                char Buff[100010] = { 0 };
                memset(Buff, 0, 100010);
                BOOL  F = InternetReadFile(hRead, Buff, 100000, &n);
                if (F == false)
                {
                    err = 2;
                    break;
                }
                if (n == 0)
                {
                    // End of file !
                    err = 0;
                    break;
                }
                TotalTransferred += n;

                //Write to File !
                //char xx = Buff[n];
                size_t olds = aaaa.size();
                aaaa.resize(olds + n);
                memcpy(aaaa.data() + olds, Buff, n);

                int NewPos = 0;
                if (Size != -1)
                    NewPos = (100 * TotalTransferred) / Size;
            }
            InternetCloseHandle(hRead);
        }
        InternetCloseHandle(hI);
    }
    return aaaa;
}

class CHATGPT_API
{
    std::string tok;
    std::string model = "text-davinci-003";
public:

    CHATGPT_API(const char* api_key)
    {
        tok = api_key;
    }

    void SetModel(const char* m)
    {
        model = m;
    }

    std::wstring Bearer()
    {
        wchar_t auth[200] = {};
        swprintf_s(auth, 200, L"Authorization: Bearer %S", tok.c_str());
        return auth;
    }

    std::optional<CHATGPT_RESULT> Image(const char* prompt,int wi = 1024,int he = 1024)
    {
        std::vector<char> data(10000);
        sprintf_s(data.data(), 10000, R"({
"prompt" : "%s",
"n" : 1,
"size" : "%ix%i"
})",  prompt, wi,he);

        data.resize(strlen(data.data()));
        RESTAPI::REST r;
        r.Connect(L"api.openai.com", true, 0, 0, 0, 0);
        std::initializer_list<std::wstring> hdrs = {
            Bearer(),
            L"Content-Type: application/json",
        };
        auto hi = r.RequestWithBuffer(L"/v1/images/generations", L"POST", hdrs, data.data(), data.size());
        std::vector<char> out;
        r.ReadToMemory(hi, out);
        out.resize(out.size() + 1);
        try
        {
            jsonxx::Object o;
            o.parse(out.data());
            CHATGPT_RESULT r;
            r.o = o;
            auto& data = o.get<jsonxx::Array>("data");
            auto& data0 = data.get<jsonxx::Object>(0);
            r.t = data0.get<jsonxx::String>("url");
            r.data = Fetch(r.t.c_str());
            return r;
        }
        catch (...)
        {
        }
        return {};
    }

    std::optional<CHATGPT_RESULT> Text(const char* prompt, int Temperature = 0, int max_tokens = 100)
    {
        std::vector<char> data(10000);
        sprintf_s(data.data(), 10000, R"({
"model": "%s",
"prompt" : "%s",
"temperature" : %i,
"max_tokens" : %i,
"top_p" : 1,
"frequency_penalty" : 0.2,
"presence_penalty" : 0
})", model.c_str(), prompt, Temperature, max_tokens);


        data.resize(strlen(data.data()));
        RESTAPI::REST r;
        r.Connect(L"api.openai.com", true, 0, 0, 0, 0);
        std::initializer_list<std::wstring> hdrs = {
            Bearer(),
            L"Content-Type: application/json",
        };
        auto hi = r.RequestWithBuffer(L"/v1/completions", L"POST", hdrs, data.data(), data.size());
        std::vector<char> out;
        r.ReadToMemory(hi, out);
        out.resize(out.size() + 1);
        try
        {
            jsonxx::Object o;
            o.parse(out.data());
            CHATGPT_RESULT r;
            r.o = o;
            auto& choices = o.get<jsonxx::Array>("choices");
            auto& choice0 = choices.get<jsonxx::Object>(0);
            r.t = choice0.get<jsonxx::String>("text");
            return r;
        }
        catch (...)
        {
        }
        return {};
    }
};