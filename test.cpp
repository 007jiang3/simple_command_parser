#include "cli.hpp"

// test for CmdLineParser
int main(int argc, char* argv[]) {
    CmdLineParser parser;

    parser.set_program_name(argv[0]);       // 设置程序名
    parser.set_ostream(std::cout);          // 设置输出流，默认为 std::cout
    parser.set_error_ostream(std::cerr);    // 设置错误输出流，默认为 std::cerr
    parser.set_option_prefix("--");         // 设置选项前缀符，默认为 "--"
    parser.set_option_short_prefix("-");    // 设置短选项前缀符，默认为 "-"

    // 添加选项
    parser.add_option({ "help", "h", "display this infomation", "", false });
    parser.add_option("name", "display your name", true);
    parser.add_option("age", "display your age", true);
    parser.add_option({ "address", "A", "display your address", "", true });

    // 参数解析
    parser.parse(argc, argv);

    // 参数处理
    if (argc == 1) {
        parser.print_help();
        exit(1);
    }

    const auto& positional_args = parser.get_positional_args(); // 获取位置参数，返回 vector 数组
    if (!positional_args.empty()) {
        std::cerr << "Error: invalid argument '" << *positional_args.begin() << "'\n";
        exit(1);
    }

    // 获取参数解析过程中的错误数量
    if (parser.get_error_count() > 0) {
        parser.print_help();
        exit(1);
    }

    // 获取选项值
    if (parser.get_option_value("help") == "true") {
        parser.print_help();
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
