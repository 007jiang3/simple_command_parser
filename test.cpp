#include "cli.hpp"

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
