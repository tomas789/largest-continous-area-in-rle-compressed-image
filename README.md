ADS II - Largest continous area in RLE compressed image

PCXArea.pdf        Dokumentace
PCXArea.tex        Dokumentace
pcxarea.cpp.bkp    Delší a více komentovaná verze 
pcxarea.min.cpp    Verze okleštěná o zbytečnosti a zúžená 
                   (výstup je pouze číslo nebo návrat hodnoty 1)
data/<a>_<b>.pcx   Vstupni testovaci data
                   <a> je velikost strany obrazku (vetsinou ctverec)
                   <b> je spravna odpoved

Pouzity prekladac:
MacBook-Air:pcx tomas789$ llvm-g++ -v
Using built-in specs.
Target: i686-apple-darwin11
Configured with: /private/var/tmp/llvmgcc42/llvmgcc42-2336.11~28/src/configure --disable-checking --enable-werror --prefix=/Applications/Xcode.app/Contents/Developer/usr/llvm-gcc-4.2 --mandir=/share/man --enable-languages=c,objc,c++,obj-c++ --program-prefix=llvm- --program-transform-name=/^[cg][^.-]*$/s/$/-4.2/ --with-slibdir=/usr/lib --build=i686-apple-darwin11 --enable-llvm=/private/var/tmp/llvmgcc42/llvmgcc42-2336.11~28/dst-llvmCore/Developer/usr/local --program-prefix=i686-apple-darwin11- --host=x86_64-apple-darwin11 --target=i686-apple-darwin11 --with-gxx-include-dir=/usr/include/c++/4.2.1
Thread model: posix
gcc version 4.2.1 (Based on Apple Inc. build 5658) (LLVM build 2336.11.00)
