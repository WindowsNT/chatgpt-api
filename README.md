# chat-gpt API for Win32

This is code to use chatgpt API in Windows.

# Text completion

* Create an account with OpenAI
* Create API keys in https://platform.openai.com/account/api-keys
* Include "chatgpt.hpp" in your Win32 code

Example code now:
```
    CHATGPT_API c("your_api_key");
    c.SetModel("code-davinci-002"); // This is free for the moment (beta)
    auto off = c.Text("What is your name?");
    auto& r = off.value();
    std::cout << r.t << std::endl;
    }
```
* API keys cost, remember to pick the correct one for your application.
* Currently, in the library, these functions are implemented:

```
std::optional<CHATGPT_RESULT> Text(const char* prompt, int Temperature = 0, int max_tokens = 10)
```

* Accepts input, temperature (the smaller, the least random results), max tokens to use.


