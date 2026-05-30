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
```

如果省略 `-c/--icon-config`，FJE 默认从当前工作目录读取 `icon_config.json`。

# 命令行参数

Usage: FJE [--help] [--version] --file VAR [--style VAR] [--icon VAR] [--icon-config VAR] [--output VAR]

Optional arguments:
  -h, --help         shows help message and exits
  -v, --version      prints version information and exits
  -f, --file         json file to be parsed [required]
  -s, --style        Choose a display style: rectangle or tree [nargs=0..1] [default: "tree"]
  -i, --icon         Choose an icon style from the icon config [nargs=0..1] [default: "star"]
  -c, --icon-config  Path to icon config json [nargs=0..1] [default: "icon_config.json"]
  -o, --output       Output to a file [nargs=0..1] [default: ""]
