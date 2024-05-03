#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <iomanip>

class CmdLineParser {
public:
    struct Option {
        std::string name;
        std::string short_name;
        std::string description;
        std::string value;
        bool required;
    };

    CmdLineParser() {
        __ostream_ptr = &std::cout;
        __error_ostream_ptr = &std::cerr;
        __option_prefix = "--";
        __option_short_prefix = "-";
    }

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
                    *__error_ostream_ptr << "Error: unknown option '" << arg << "'\n";
                    ++__error_count;
                    continue;
                }
                if (it->second.required) {
                    if (i + 1 < argc && !__has_prefix(argv[i + 1], __option_prefix) && !__has_prefix(argv[i + 1], __option_short_prefix)) {
                        it->second.value = argv[++i];
                    } else {
                        *__error_ostream_ptr << "Error: value missing for option '" << arg << "'\n";
                        ++__error_count;
                    }
                } else {
                    it->second.value = "true";
                }
            } else if (__has_prefix(arg, __option_short_prefix)) {
                std::string short_name = __get_option_name_no_prefix(arg, __option_short_prefix);
                auto _it = __short_option_names.find(short_name);
                if (_it == __short_option_names.end()) {
                    *__error_ostream_ptr << "Error: unknown option '" << arg << "'\n";
                    ++__error_count;
                    continue;
                }
                auto it = __options.find(_it->second);
                if (it == __options.end()) {
                    *__error_ostream_ptr << "Error: unknown option '" << arg << "'\n";
                    ++__error_count;
                    continue;
                }
                if (it->second.required) {
                    if (i + 1 < argc && !__has_prefix(argv[i + 1], __option_prefix) && !__has_prefix(argv[i + 1], __option_short_prefix)) {
                        it->second.value = argv[++i];
                    } else {
                        *__error_ostream_ptr << "Error: value missing for option '" << arg << "'\n";
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

    void set_ostream(std::ostream& os) {
        __ostream_ptr = &os;
    }

    void set_error_ostream(std::ostream& err_os) {
        __error_ostream_ptr = &err_os;
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

    void print_help() {
        *__ostream_ptr << "\nUsage: " << __program_name << " [options]\n";
        *__ostream_ptr << "\nOptions:\n";
        std::size_t short_prefix_size = __option_short_prefix.length();
        std::size_t max_short_name_size = 0;
        for (const auto& name_pair : __short_option_names) {
            max_short_name_size = std::max(max_short_name_size, name_pair.first.length());
        }
        std::size_t size = short_prefix_size + max_short_name_size;
        for (const auto& option_pair : __options) {
            const auto& option = option_pair.second;
            *__ostream_ptr << "  ";
            if (option.short_name.empty()) {
                std::string blank_str(size, ' ');
                *__ostream_ptr << blank_str << "  ";
            } else {
                *__ostream_ptr << __option_short_prefix << std::left << std::setw(max_short_name_size) << option.short_name << ", ";
            }
            *__ostream_ptr << __option_prefix << std::left << std::setw(30);
            if (option.required) {
                *__ostream_ptr << option.name + " <arg>";
            } else {
                *__ostream_ptr << option.name + "      ";
            }
            *__ostream_ptr << option.description << '\n';
        }
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
    std::ostream* __ostream_ptr, * __error_ostream_ptr;
};
