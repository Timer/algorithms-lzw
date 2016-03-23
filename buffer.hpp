#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <string>
#include <climits>
#include <cassert>
#include <cmath>
#include <bitset>

class Buffer {
protected:
  std::string buffer;
  unsigned int pos, word_length, word_mask;

public:
  unsigned int get_word_length() const { return word_length; }

  void set_word_length(const unsigned int &word_length) {
    assert(word_length > 0);
    this->word_length = word_length;
    this->word_mask = 0;
    for (unsigned int i = 0; i < word_length - 1; ++i) {
      this->word_mask |= 0b1;
      this->word_mask <<= 1;
    }
    this->word_mask |= 0b1;
  }
};

class OutputBuffer : public Buffer {
public:
  OutputBuffer() : OutputBuffer(CHAR_BIT) { }

  OutputBuffer(unsigned int word_length) {
    this->buffer = buffer;
    this->pos = 0;
    set_word_length(word_length);
  }

  void write(const unsigned int _value) {
    unsigned int value = _value & this->word_mask;
    assert(value == _value);
    buffer.resize(std::ceil((pos + word_length) / (double) CHAR_BIT), '\0');
    unsigned int start = pos / CHAR_BIT, offset = pos % CHAR_BIT;
    if (offset + word_length <= CHAR_BIT) {
      value <<= CHAR_BIT - offset - word_length;
      buffer[start] |= value;
      pos += word_length;
    } else {
      unsigned int remain = CHAR_BIT - offset;
      buffer[start] |= value >> (word_length - remain);
      pos += remain;
      unsigned int ows = get_word_length();
      set_word_length(ows - remain);
      unsigned int extract = 0;
      for (unsigned int i = 0; i < (ows - remain - 1); ++i) { extract |= 0b1; extract <<= 1; }
      extract |= 0b1;
      write(value & extract);
      set_word_length(ows);
    }
  }

  std::string & get_buffer() { return buffer; }
};

class InputBuffer : public Buffer {
public:
  InputBuffer(std::string buffer) : InputBuffer(buffer, CHAR_BIT) { }

  InputBuffer(std::string buffer, unsigned int word_length) {
    this->buffer = buffer;
    this->pos = 0;
    set_word_length(word_length);
  }

  unsigned int read() {
    if (pos + word_length > buffer.length() * CHAR_BIT) throw std::runtime_error("buffer underflow");
    unsigned int start = pos / CHAR_BIT, offset = pos % CHAR_BIT;
    if (offset + word_length <= CHAR_BIT) {
      unsigned int a = buffer[start] >> (CHAR_BIT - offset - word_length);
      pos += word_length;
      return a & word_mask;
    } else {
      unsigned int remain = CHAR_BIT - offset, extract = 0;
      for (unsigned int i = 0; i < remain - 1; ++i) { extract |= 0b1; extract <<= 1; }
      extract |= 0b1;
      unsigned int a = buffer[start], b = buffer[start + 1];
      unsigned int first = a & extract;
      pos += remain;
      unsigned int ows = get_word_length();
      set_word_length(ows - remain);
      unsigned int second = read();
      set_word_length(ows);
      return (first << (word_length - remain)) | second;
    }
  }
};

#endif
