#include "rest.h"
#include "jsonxx.h"

struct CHATGPT_RESULT
{
    jsonxx::Object o;
    std::string t;
};

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


    std::optional<CHATGPT_RESULT> Text(const char* prompt, int Temperature = 0, int max_tokens = 10)
    {
        jsonxx::Object o;

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