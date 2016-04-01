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
    //Create a bitmask to extract bytes with the given word length
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
    //Ensure we're not losing data
    assert(value == _value);
    //Ensure there's enough room for value about to be wrote
    buffer.resize(std::ceil((pos + word_length) / (double) CHAR_BIT), '\0');
    //Compute where we need to put the data in the string
    unsigned int start = pos / CHAR_BIT, offset = pos % CHAR_BIT;
    //The data fits within one byte ->
    if (offset + word_length <= CHAR_BIT) {
      //Shift value and append it to the current byte with an bitwise OR operation
      value <<= CHAR_BIT - offset - word_length;
      buffer[start] |= value;
      pos += word_length;
    } else
    //The data does not fit within one byte, we have to split it ->
    {
      unsigned int remain = CHAR_BIT - offset;
      //Append what we can of the value to the remaining byte
      buffer[start] |= value >> (word_length - remain);
      pos += remain;
      //Recursively write the next byte and reduce the word size since we took some off
      unsigned int ows = get_word_length();
      set_word_length(ows - remain);
      unsigned int extract = 0;
      for (unsigned int i = 0; i < (ows - remain - 1); ++i) { extract |= 0b1; extract <<= 1; }
      extract |= 0b1;
      //Write the value minus what we already wrote (bitwise AND)
      write(value & extract);
      //Restore functionality
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
    //Compute where we need to read from next
    unsigned int start = pos / CHAR_BIT, offset = pos % CHAR_BIT;
    //If we can read from the current byte without viewing two ->
    if (offset + word_length <= CHAR_BIT) {
      unsigned int a = buffer[start] >> (CHAR_BIT - offset - word_length);
      pos += word_length;
      return a & word_mask;
    } else
    //We need to look across two (or more) bytes, split the view ->
    {
      //Compute a mask to extract what we need from the current byte
      unsigned int remain = CHAR_BIT - offset, extract = 0;
      for (unsigned int i = 0; i < remain - 1; ++i) { extract |= 0b1; extract <<= 1; }
      extract |= 0b1;
      unsigned int a = buffer[start], b = buffer[start + 1];
      unsigned int first = a & extract;
      pos += remain;
      //Recursively read the next part
      unsigned int ows = get_word_length();
      set_word_length(ows - remain);
      unsigned int second = read();
      set_word_length(ows);
      //Combine the data we retrieved
      return (first << (word_length - remain)) | second;
    }
  }
};

#endif
