#ifndef RISCVDB_LINENOISE_WRAPPER_H
#define RISCVDB_LINENOISE_WRAPPER_H

#include <string>
#include <vector>
#include "linenoise.h"

namespace linenoise_wrapper
{

const std::string DEFAULT_PROMPT = "";

class Linenoise
{
public:
    enum ReturnCode
    {
        RETURN_OK,
        RETURN_CTRLC,
        RETURN_CTRLD,
        RETURN_QUIT,
    };

    enum HintColor
    {
        COLOR_RED = 31,
        COLOR_GREEN = 32,
        COLOR_YELLOW = 33,
        COLOR_BLUE = 34,
        COLOR_MAGENTA = 35,
        COLOR_CYAN = 36,
        COLOR_WHITE = 37,
    };

    // Linenoise is a single instance library, so the wrapper forces a singleton
    static Linenoise& get();

    static ReturnCode next(std::string& inputLine, const std::string& prompt = DEFAULT_PROMPT);
    static void addCompletion(const std::string& completion);
    static void setHintAppearance(HintColor color, bool bold);

private:
    Linenoise();
    static char* hints(const char* buf, int* color, int* bold);
    static void freeHint(void* buf);
    static void completion(const char* buf, linenoiseCompletions* lc);

    std::vector<std::string> findCompletions(const std::string& input, bool allowEmpty = false);

    std::vector<std::string> m_completionHints;
    int m_hintColor;
    int m_hintBold;
};

} // namespace linenoise_wrapper

#endif  // RISCVDB_LINENOISE_WRAPPER_H
