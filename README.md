# chat-gpt API for Win32

A very simple single-header library to use chatgpt API in Windows. Just include **chatgpt.hpp** somewhere and you are set.

I describe more details in the CodeProject article: https://www.codeproject.com/Articles/5355950/ChatGPT-API-for-Windows-Use-ChatGPT-in-your-Window

I use this API in my full scale video/audio sequencer: https://www.turbo-play.com

# Preparation

* Create an account with OpenAI
* Create API keys in https://platform.openai.com/account/api-keys
* Include "chatgpt.hpp" in your Win32 code
* Read and decide which models to use https://platform.openai.com/docs/models/overview

# Text generation

Example code:
```
CHATGPT_API c("your_api_key");
auto off = c.Text("What is your name?");
auto& r = off.value();
std::cout << r.t << std::endl;
```

You can also set a model (https://platform.openai.com/docs/models/overview)
```
c.SetModel("code-davinci-002");
```

Code models are free at this beta stage of the API.


# Image generation

Example code:
```
CHATGPT_API c("your_api_key");
auto off = c.Image("White cat");
auto& r = off.value();
// r.data has the PNG data of the cat
```

# Summary 

* API keys cost, remember to pick the correct one for your application.
* Currently, in the library, these functions are implemented:

```
// Sets the model to use for text generation
void SetModel(const char* model);

// Accepts input, temperature (the smaller, the least random results), max tokens to use.
std::optional<CHATGPT_RESULT> Text(const char* prompt, int Temperature = 0, int max_tokens = 100);

// Accepts a prompt and returns the image as raw PNG data
std::optional<CHATGPT_RESULT> Image(const char* prompt,int wi = 1024,int he = 1024); // can be also 512x512 or 256x256
```

For example, this simple cpp is included in the repo:
```
#include <...>
#include "chatgpt.hpp"
int main()
{
    WSADATA w = {};
    WSAStartup(MAKEWORD(2, 2), &w);
    CHATGPT_API c("api_key");
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
```


More to be implemented soon!
