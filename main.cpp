#include <iostream>
#include <cstring>
#include <cstdint>
#include <fstream>
#include <algorithm>

struct Options {
  uint64_t lines = UINT64_MAX;
  bool is_tail = false;
  char delimiter = '\n';
  char* file_name{};
};

void PrintFile(const Options& options) {
    std::fstream fs(options.file_name, std::fstream::in);

    if (!fs.is_open()) {
        std::cerr << "File opening error! Check if the arguments are written correctly" << std::endl;
        return;
    }

    uint64_t count = 0;
    uint64_t start = 0;
    char ch;

    if (options.is_tail) {
        while (fs.get(ch)) {
            if (ch == options.delimiter) {
                start++;
            }
        }

        start = (start >= options.lines) ? (start - options.lines) : 0;

        if (ch != options.delimiter) {
            start++;
        }

        fs.clear();
        fs.seekg(0, std::ios::beg);
    }

    while (fs.get(ch) && options.lines > 0 && count < options.lines + start) {
        if (count >= start) {
            std::cout << ch;
        }

        if (ch == options.delimiter) {
            count++;
        }
    }
}

bool IsDigits(const std::string& str) {
    return str.find_first_not_of("0123456789") == std::string::npos;
}

uint64_t ParseLines(const char* arg) {
    if (std::strlen(arg) == 0 || !IsDigits(arg)) {
        return UINT64_MAX;
    }

    return std::strtoll(arg, nullptr, 10);
}

char ParseDelimiter(char* arg) {
    if (std::strlen(arg) == 0) {
        return '\n';
    }

    if ((arg[0] == '\'' && arg[std::strlen(arg) - 1] == '\'') ||
        (arg[0] == '\"' && arg[std::strlen(arg) - 1] == '\"')) {
        arg++; // Сдвиг указателя вперёд на один, чтобы пропустить начальную кавычку
        arg[std::strlen(arg) - 1] = '\0';
    }

    if (arg[0] == '\\' && std::strlen(arg) >= 2) {
        switch (arg[1]) {
            case 'a': return '\a';
            case 'b': return '\b';
            case 'f': return '\f';
            case 'n': return '\n';
            case 'r': return '\r';
            case 't': return '\t';
            case 'v': return '\v';
            default: return arg[0];
        }
    }

    return arg[0];
}

Options ParseArguments(int argc, char** argv) {
    Options options;

    const char* kArgLinesLong = "--lines=";
    const char* kArgTailLong = "--tail";
    const char* kArgDelimiterLong = "--delimiter=";

    const char* kArgLinesShort = "-l";
    const char* kArgTailShort = "-t";
    const char* kArgDelimiterShort = "-d";

    const size_t kArgLinesLength = std::strlen(kArgLinesLong);
    const size_t kArgDelimiterLength = std::strlen(kArgDelimiterLong);

    for (int i = 1; i < argc; i++) {
        char* arg = argv[i];

        if (!std::strncmp(arg, kArgLinesLong, kArgLinesLength)) {
            arg += kArgLinesLength;
            options.lines = ParseLines(arg);
        } else if (!std::strcmp(arg, kArgLinesShort) && i + 1 < argc) {
            arg = argv[++i];
            options.lines = ParseLines(arg);
        } else if (!std::strcmp(arg, kArgTailLong) || !std::strcmp(arg, kArgTailShort)) {
            options.is_tail = true;
        } else if (!std::strncmp(arg, kArgDelimiterLong, kArgDelimiterLength)) {
            arg += kArgDelimiterLength;
            options.delimiter = ParseDelimiter(arg);
        } else if (!std::strcmp(arg, kArgDelimiterShort) && i + 1 < argc) {
            arg = argv[++i];
            options.delimiter = ParseDelimiter(arg);
        } else {
            options.file_name = arg;
        }
    }

    return options;
}

int main(int argc, char** argv) {
    if (argc <= 1) {
        std::cerr << "A file is required as an argument. Ex: PrintFile.exe [OPTION] filename" << std::endl;
        return EXIT_FAILURE;
    }

    Options options = ParseArguments(argc, argv);

    PrintFile(options);

    return EXIT_SUCCESS;
}
