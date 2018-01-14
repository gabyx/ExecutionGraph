#!/bin/bash
set -e
cat << EOF > main.cpp
#include <iostream>
#include <string>
#include <codecvt>
struct A{
    int a;
    int b;
};

int main()
{
    A s;
    std::string a="hello world/\x85";
    std::string b=u8"hello world/lkajsdlkajsdlkjalsdj/lasjdlkajsdlkjasd/laksjdlkajsdlkjasd/lkajsdlkajsdlkajsldkjad";
    std::cout << a << std::endl;

    std::wstring wstring = L"xyz";
    std::u16string u16s = u"xyz/asd/";
    std::string u8_conv = std::wstring_convert<
        std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(u16s);
    std::cout << u8_conv.data() << std::endl;

    const char* p = reinterpret_cast<const char*>(static_cast<char16_t*>(u16s.data()));
    std::string f = p;
    std::cout << f << std::endl;  
    const char* r = "\x85\xac\a\0p\0\0\x10\x85\xac\a\0p\0\0";
    std::cout << r << std::endl;
}
EOF

export LDFLAGS="$LDFLAGS -L/usr/local/opt/llvm/lib -Wl,-rpath,/usr/local/opt/llvm/lib"
export CPPFLAGS="$CPPFLAGS -I/usr/local/opt/llvm/include -I/usr/local/opt/llvm/include/c++/v1/"

clang++ -g -g3 -arch x86_64 -mmacosx-version-min=10.9 \
        -std=c++17 -lc++experimental \
        -fsanitize=leak -fsanitize=address \
        $LDFLAGS \
        $CPPFLAGS \
        main.cpp -o main
echo -ne "breakpoint set --file main.cpp --line 14\nrun\nfr v a\nfr v b" > runFile
lldb ./main -s runFile