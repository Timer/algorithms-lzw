#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstddef>
#include "lzw.hpp"
#include "buffer.hpp"

#define DICTIONARY_MIN_BITS 9
#define DICTIONARY_MAX_BITS 16

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cout << "You must provide this program with two arguments, [c/e] [filename]." << std::endl;
    return 1;
  }
  if (!strcmp(argv[1], "c")) {
    std::cout << "Compressing " << argv[2] << " ..." << std::endl;
    std::ifstream f(argv[2]);
    std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    f.close();
    LZWCompress lzw(256, true, DICTIONARY_MAX_BITS);
    InputBuffer in(content, 8);
    OutputBuffer out(DICTIONARY_MIN_BITS);
    lzw.compress(in, out);
    std::ofstream f2((std::string(argv[2]) + ".lzw2").c_str());
    f2 << out.get_buffer();
    f2.close();
  } else if(!strcmp(argv[1], "e")) {
    std::cout << "Expanding " << argv[2] << " ..." << std::endl;
    std::ifstream f(argv[2]);
    std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    f.close();
    LZWInflate lzw(256, true, DICTIONARY_MAX_BITS);
    InputBuffer in(content, DICTIONARY_MIN_BITS);
    OutputBuffer out(8);
    lzw.decompress(in, out);
    std::string output_name(argv[2]);
    std::size_t pos = output_name.find_last_of(".lzw2");
    if (pos != std::string::npos) output_name = output_name.substr(0, pos - 4) + "2M";
    std::ofstream f2(output_name.c_str());
    f2 << out.get_buffer();
    f2.close();
  } else {
    std::cout << "Unknown mode `" << argv[1] <<  '`' << std::endl;
    return 1;
  }
  return 0;
}
