#ifndef LZW_HPP
#define LZW_HPP

#include <string>
#include <map>
#include <cmath>
#include <iostream>
#include "buffer.hpp"

class LZWCompress {
private:
  std::map<std::string, unsigned int> dictionary;
  int dictionary_size, dictionary_bits;
  bool variable_dictionary;

public:
  LZWCompress(int dictionary_base, bool variable_dictionary, int dictionary_bits) {
    this->variable_dictionary = variable_dictionary;
    this->dictionary_bits = dictionary_bits;
    for (int i = 0; i < dictionary_base; ++i) dictionary[std::string(1, i)] = i;
    this->dictionary_size = dictionary_base;
  }

  void compress(InputBuffer &in, OutputBuffer &out) {
    unsigned int dictionary_limit = std::pow(2, out.get_word_length());
    std::string word;
    for (;;) {
      try {
        int value = in.read();
        std::string prefix = word + std::string(1, value);
        if (dictionary.count(prefix)) {
          word = prefix;
        } else {
          out.write(dictionary[word]);
          if (dictionary_size < dictionary_limit || (variable_dictionary && out.get_word_length() <= dictionary_bits)) {
            if (variable_dictionary && dictionary_size >= dictionary_limit) {
              out.set_word_length(out.get_word_length() + 1);
              dictionary_limit *= 2;
            }
            dictionary[prefix] = dictionary_size++;
          }
          word = std::string(1, value);
        }
      } catch (const std::exception &e) {
        if (strcmp(e.what(), "buffer underflow")) std::cout << e.what() << std::endl;
        break;
      }
    }
    if (!word.empty()) out.write(dictionary[word]);
  }
};

class LZWInflate {
private:
  std::map<unsigned int, std::string> dictionary;
  unsigned int dictionary_size, dictionary_bits;
  bool variable_dictionary;

public:
  LZWInflate(int dictionary_base, bool variable_dictionary, int dictionary_bits) {
    this->variable_dictionary = variable_dictionary;
    this->dictionary_bits = dictionary_bits;
    for (int i = 0; i < dictionary_base; ++i) dictionary[i] = std::string(1, i);
    this->dictionary_size = dictionary_base;
  }

  void decompress(InputBuffer &in, OutputBuffer &out) {
    unsigned int dictionary_limit = std::pow(2, in.get_word_length());
    std::string entry;
    std::string word(1, in.read());
    out.write(word[0]);
    for (;;) {
      try {
        unsigned int value = in.read();
        if (dictionary.count(value)) {
          entry = dictionary[value];
        } else if (value == dictionary_size) {
          entry = word + word[0];
        } else {
          std::string msg("bad compression ... value encountered: ");
          msg += std::to_string(value);
          msg += ", dict size: ";
          msg += std::to_string(dictionary_size);
          throw std::runtime_error(msg);
        }
        for (unsigned char c : entry) out.write(c);
        if (dictionary_size < dictionary_limit || (variable_dictionary && in.get_word_length() <= dictionary_bits)) {
          if (variable_dictionary && dictionary_size >= dictionary_limit - 1) {
            in.set_word_length(in.get_word_length() + 1);
            dictionary_limit *= 2;
          }
          dictionary[dictionary_size++] = word + std::string(1, entry[0]);
        }
        word = entry;
      } catch (const std::exception &e) {
        if (strcmp(e.what(), "buffer underflow")) std::cout << e.what() << std::endl;
        break;
      }
    }
  }
};

#endif
