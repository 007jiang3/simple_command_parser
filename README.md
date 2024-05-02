# Simple Command Parser
一个简单的命令行参数解析库，使用现代C++编写。

## 用法

初始化，包括设置程序名、选项前缀、添加选项。
```cpp
CmdLineParser parser;

parser.set_program_name(argv[0]);       // 设置程序名
parser.set_option_prefix("--");         // 设置选项前缀符
parser.set_option_short_prefix("-");    // 设置选型缩写前缀符

// 添加选项
parser.add_option({ "help", "h", "display this infomation", "", false });
parser.add_option("name", "display your name", true);
parser.add_option("age", "display your age", true);
parser.add_option({ "address", "A", "display your address", "", true });

```
参数解析，需传入`main(int argc, char* argv[])`中的`argc`和`argv`。
```cpp
parser.parse(argc, argv);
```
获取位置参数。
```cpp
const auto& positional_args = parser.get_positional_args();
```

## 示例
示例程序代码`test.cpp`。
```cpp
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
```
执行解析程序，`test`为程序名，需在终端运行。
```sh
./test --name test --age 20 -A home
```
