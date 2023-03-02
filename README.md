# chat-gpt API for Win32

This is code to use chatgpt API in Windows.

# Preparation

* Create an account with OpenAI
* Create API keys in https://platform.openai.com/account/api-keys
* Include "chatgpt.hpp" in your Win32 code
* Read and decide which models to use https://platform.openai.com/docs/models/overview. 

# Text generation

Example code now:
```
CHATGPT_API c("your_api_key");
auto off = c.Text("What is your name?");
auto& r = off.value();
std::cout << r.t << std::endl;
```

# Image generation

Example code now:
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
std::optional<CHATGPT_RESULT> Text(const char* prompt, int Temperature = 0, int max_tokens = 10);
// Accepts input, temperature (the smaller, the least random results), max tokens to use.
void SetModel(const char* model);
// Sets the model to use

std::optional<CHATGPT_RESULT> Image(const char* prompt,int wi = 512,int he = 512); // max is 1024x1024, can be also 256x256
// Accepts a prompt and returns the image as raw PNG data
```


More to be implemented soon!
