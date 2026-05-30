# FJE
a json file visualized command line interface tool

# Installation
```bash
git clone --recurse-submodules https://github.com/hzspyy/FJE.git
cd FJE
cmake -S . -B build
cmake --build build
```

# Usage
```bash
./build/FJE -f test/test.json -s tree -i star -c icon_config.json
./build/FJE -f test/test.json -s rectangle -i pokerface -c icon_config.json
cat test/test.json | ./build/FJE -f - --no-icons
./build/FJE --list-icons
./build/FJE -f test/test.json -o output.txt
```

如果省略 `-c/--icon-config`，FJE 会依次从当前工作目录、可执行文件目录、可执行文件父目录查找 `icon_config.json`。

# 命令行参数

Usage: FJE [--help] [--version] [--file VAR] [--style VAR] [--icon VAR] [--icon-config VAR] [--output VAR] [--list-icons] [--no-icons]

Optional arguments:
  -h, --help         shows help message and exits
  -v, --version      prints version information and exits
  -f, --file         json file to be parsed, or '-' to read stdin
  -s, --style        Choose a display style: rectangle or tree [nargs=0..1] [default: "tree"]
  -i, --icon         Choose an icon style from the icon config [nargs=0..1] [default: "star"]
  -c, --icon-config  Path to icon config json [nargs=0..1] [default: "icon_config.json"]
  -o, --output       Output to a file [nargs=0..1] [default: ""]
  --list-icons       List icons from the resolved icon config and exit
  --no-icons         Render without configured icon prefixes

# Tests
```bash
ctest --test-dir build --output-on-failure
```
