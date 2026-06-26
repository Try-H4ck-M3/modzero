g++ -O2 -Wl,-z,relro,-z,now -fPIE -pie -o bin/modzero \
    src/main.cpp    \
    src/logger.cpp  \
    src/read.cpp    \
    src/write.cpp   \
    src/utils.cpp   \
    src/recon.cpp   \
    -std=c++20 -lmodbus