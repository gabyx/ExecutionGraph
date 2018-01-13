#!/bin/bash
cat << EOF > main.cpp
#include <iostream>
#include <string>

struct A{
    int a;
    int b;
};

int main()
{
    A s;
    std::string a="hello world";
    std::string b=u8"hello world";
    std::cout << a << std::endl;
}
EOF

export LDFLAGS="$LDFLAGS -L/usr/local/opt/llvm/lib -Wl,-rpath,/usr/local/opt/llvm/lib"
export CPPFLAGS="$CPPFLAGS -I/usr/local/opt/llvm/include -I/usr/local/opt/llvm/include/c++/v1/"

clang++ -g -arch x86_64 -mmacosx-version-min=10.9 \
        -std=c++17 -lc++experimental \
        -fsanitize=leak -fsanitize=address \
        $LDFLAGS \
        $CPPFLAGS \
        main.cpp -o main
echo -ne "breakpoint set --file main.cpp --line 14\nrun\nfr v a\nfr v b" > runFile
lldb ./main -s runFile