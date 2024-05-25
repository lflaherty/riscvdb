#include "linenoise_wrapper.h"

#include <string>
#include <vector>
#include <memory>
#include <errno.h>
#include <cstring>

namespace linenoise_wrapper
{

Linenoise& Linenoise::get()
{
    static Linenoise instance;
    return instance;
}

Linenoise::Linenoise()
: m_hintColor(COLOR_MAGENTA),
  m_hintBold(false)
{
    // set callbacks (in C library)
    linenoiseSetHintsCallback(hints);
    linenoiseSetFreeHintsCallback(freeHint);
    linenoiseSetCompletionCallback(completion);
    linenoiseHistorySetMaxLen(128);
}

char* Linenoise::hints(const char* buf, int* color, int* bold)
{
    Linenoise& inst = get();
    std::string bufStr(buf);

    std::vector<std::string> completions = inst.findCompletions(bufStr);
    if (completions.size() > 0)
    {
        std::string& hintStr = completions.front();
        // raw pointer generally bad in C++ ... but required for C library
        char* completion = new char[hintStr.length() - bufStr.length()];
        strcpy(completion, hintStr.substr(bufStr.length()).c_str());

        *color = inst.m_hintColor;
        *bold = inst.m_hintBold;
        return completion;
    }
    else
    {
        return nullptr;
    }
}

void Linenoise::freeHint(void* buf)
{
    if (buf)
    {
        delete (char*)buf;
    }
}

void Linenoise::completion(const char* buf, linenoiseCompletions* lc)
{
    Linenoise& inst = get();
    std::string bufStr(buf);

    std::vector<std::string> completions = inst.findCompletions(bufStr, true);
    for (const std::string& completion : completions)
    {
        linenoiseAddCompletion(lc, completion.c_str());
    }
}

std::vector<std::string> Linenoise::findCompletions(const std::string& input, bool allowEmpty)
{
    std::vector<std::string> completions;
    if (input.empty() && !allowEmpty)
    {
        return completions;
    }

    for (const std::string& suggestion : m_completionHints)
    {
        if (suggestion.compare(0, input.length(), input) == 0)
        {
            completions.push_back(suggestion);
        }
    }

    return completions;
}

void Linenoise::setHintAppearance(Linenoise::HintColor color, bool bold)
{
    Linenoise& inst = get();
    inst.m_hintColor = color;
    inst.m_hintBold = bold;
}

Linenoise::ReturnCode Linenoise::next(std::string& inputLine, const std::string& prompt)
{
    char* line = linenoise(prompt.c_str());
    if (line == nullptr)
    {
        switch (errno)
        {
            case EAGAIN:
                // linenoise returns this for a Ctrl-C
                return ReturnCode::RETURN_CTRLC;
            
            case ENOENT:
                // linenoise returns this for a Ctrl-D
                return ReturnCode::RETURN_CTRLD;
            
            default:
                return ReturnCode::RETURN_QUIT;
        }
    }

    linenoiseHistoryAdd(line);

    inputLine = std::string(line);

    linenoiseFree(line);
    return ReturnCode::RETURN_OK;
}

void Linenoise::addCompletion(const std::string& completion)
{
    Linenoise& inst = get();
    inst.m_completionHints.push_back(completion);
}

} // namespace linenoise_wrapper
