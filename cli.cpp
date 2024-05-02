#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>

class CmdLineParser {
public:
    struct Option {
        std::string name;
        std::string short_name;
        std::string description;
        std::string value;
        bool required;
    };

    CmdLineParser() = default;

    void parse(int argc, char* argv[]) {
        if (__program_name.empty()) {
            __program_name = argv[0];
        }

        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (__has_prefix(arg, __option_prefix)) {
                std::string name = __get_option_name_no_prefix(arg, __option_prefix);
                auto it = __options.find(name);
                if (it == __options.end()) {
                    std::cerr << "Error: unknown option '" << arg << "'\n";
                    ++__error_count;
                    continue;
                }
                if (it->second.required) {
                    if (i + 1 < argc && !__has_prefix(argv[i + 1], __option_prefix) && !__has_prefix(argv[i + 1], __option_short_prefix)) {
                        it->second.value = argv[++i];
                    } else {
                        std::cerr << "Error: value missing for option '" << arg << "'\n";
                        ++__error_count;
                    }
                } else {
                    it->second.value = "true";
                }
            } else if (__has_prefix(arg, __option_short_prefix)) {
                std::string short_name = __get_option_name_no_prefix(arg, __option_short_prefix);
                auto _it = __short_option_names.find(short_name);
                if (_it == __short_option_names.end()) {
                    std::cerr << "Error: unknown option '" << arg << "'\n";
                    ++__error_count;
                    continue;
                }
                auto it = __options.find(_it->second);
                if (it == __options.end()) {
                    std::cerr << "Error: unknown option '" << arg << "'\n";
                    ++__error_count;
                    continue;
                }
                if (it->second.required) {
                    if (i + 1 < argc && !__has_prefix(argv[i + 1], __option_prefix) && !__has_prefix(argv[i + 1], __option_short_prefix)) {
                        it->second.value = argv[++i];
                    } else {
                        std::cerr << "Error: value missing for option '" << arg << "'\n";
                        ++__error_count;
                    }
                } else {
                    it->second.value = "true";
                }
            } else {
                __positional_args.emplace_back(arg);
            }
        }
    }

    // Set program's name.
    void set_program_name(const std::string& name) {
        __program_name = name;
    }

    std::string get_program_name() const {
        return __program_name;
    }

    // Set option's prefix.
    void set_option_prefix(const std::string& prefix) {
        __option_prefix = prefix;
    }

    // Set option's short prefix.
    void set_option_short_prefix(const std::string& prefix) {
        __option_short_prefix = prefix;
    }

    void add_option(const std::string& name, const std::string& description, bool required = false) {
        __options[name] = { name, "", description, "", required };
    }

    void add_option(const Option& option) {
        __options[option.name] = option;
        if (!option.short_name.empty()) {
            __short_option_names[option.short_name] = option.name;
        }
    }

    Option get_option(const std::string& name) const {
        if (__options.find(name) != __options.end()) {
            return __options.at(name);
        }
        return {};
    }

    std::string get_option_value(const std::string& name) const {
        if (__options.find(name) != __options.end()) {
            return __options.at(name).value;
        }
        return "";
    }

    std::string get_option_short_name(const std::string& name) const {
        if (__options.find(name) != __options.end()) {
            return __options.at(name).short_name;
        }
        return "";
    }

    std::string get_option_description(const std::string& name) const {
        if (__options.find(name) != __options.end()) {
            return __options.at(name).description;
        }
        return "";
    }

    std::vector<std::string>& get_positional_args() {
        return __positional_args;
    }

    unsigned int get_error_count() {
        return __error_count;
    }

private:
    static bool __has_prefix(const std::string& __arg, const std::string& __prefix) {
        return __arg.substr(0, __prefix.length()) == __prefix;
    }

    static std::string __get_option_name_no_prefix(const std::string& __arg_with_prefix, const std::string& __prefix) {
        return __arg_with_prefix.substr(__prefix.length());
    }

private:
    std::string __program_name;
    std::string __option_prefix;
    std::string __option_short_prefix;
    std::unordered_map<std::string, Option> __options;
    std::unordered_map<std::string, std::string> __short_option_names;
    std::vector<std::string> __positional_args;
    unsigned int __error_count = 0;
};

void print_help(const CmdLineParser& parser) {
    std::cout << "\nUsage: " << parser.get_program_name() << " [options]\n";
    std::cout << "\nOptions:\n";
    std::cout << "  -h, --help                  " << parser.get_option_description("help") << "\n";
    std::cout << "      --name <arg>            " << parser.get_option_description("name") << "\n";
    std::cout << "      --age <arg>             " << parser.get_option_description("age") << "\n";
    std::cout << "  -A, --address <arg>         " << parser.get_option_description("address") << "\n";
}

// test for CmdLineParser
int main(int argc, char* argv[]) {
    CmdLineParser parser;

    parser.set_program_name(argv[0]);
    parser.set_option_prefix("--");
    parser.set_option_short_prefix("-");

    parser.add_option({ "help", "h", "display this infomation", "", false });
    parser.add_option("name", "display your name", true);
    parser.add_option("age", "display your age", true);
    parser.add_option({ "address", "A", "display your address", "", true });

    parser.parse(argc, argv);

    if (argc == 1) {
        print_help(parser);
        exit(1);
    }

    const auto& positional_args = parser.get_positional_args();
    if (!positional_args.empty()) {
        std::cerr << "Error: invalid argument '" << *positional_args.begin() << "'\n";
        exit(1);
    }

    if (parser.get_error_count() > 0) {
        print_help(parser);
        exit(1);
    }

    if (parser.get_option_value("help") == "true") {
        print_help(parser);
        return 0;
    }

    std::string name = parser.get_option_value("name");
    if (!name.empty()) {
        std::cout << "Your name is " << name << ".\n";
    }

    std::string age = parser.get_option_value("age");
    if (!age.empty()) {
        std::cout << "You are " << age << " years old.\n";
    }

    std::string address = parser.get_option_value("address");
    if (!address.empty()) {
        std::cout << "You reside in " << address << ".\n";
    }

    return 0;
}
