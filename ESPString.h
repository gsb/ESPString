
//-- ESPString ------------------------------------------------------------------
// This is a modified PString helper class w/ websocket client send builtin.
//    PString.h - Lightweight printable string class
// Copyright (c) 2009-2012 Mikal Hart.  All right reserved.
// Copyright (c) 2020 Abhijit Bose (@boseji) 
//    - v3.0.1 - Modifications to advance Hex printing functions

#define  ESPSTRING_H

#include "Print.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#ifdef ARDUINO_ARCH_ESP32
  #include <AsyncTCP.h>
#elif defined(ARDUINO_ARCH_ESP8266)
  #include <ESPAsyncTCP.h>
#else
  // System not supported.
#endif

class ESPString : public Print {

  private:
    char *_buf, *_cur;
    size_t _size;
    AsyncClient* _client;

  public:
    ESPString(char *buf, size_t size) : _buf(buf), _size(size) { begin(); } // Basic constructor requires a preallocated buffer
    template<class T> ESPString(char *buf, size_t size, T arg, AsyncClient* target) : _buf(buf), _size(size), _client(target) {begin();print(arg);send();}
    template<class T> ESPString(char *buf, size_t size, T arg) : _buf(buf), _size(size) {begin(); print(arg);}
    template<class T> ESPString(char *buf, size_t size, T arg, int modifier) : _buf(buf), _size(size) { begin(); print(arg, modifier); }
    inline const size_t length() { return _cur - _buf; } // returns the length of the current string, not counting the 0 terminator
    inline const size_t capacity() { return _size; } // returns the capacity of the string
    inline operator const char *() { return _buf; } // gives access to the internal string
    bool operator==(const char *str) { return _size > 0 && !strcmp(_buf, str); } // compare to another string
    void begin(){ _cur = _buf; if (_size > 0) _buf[0] = '\0'; } // call this to re-use an existing string
    void client(AsyncClient* client) {_client = client;};
    void send() {_client->add(_buf,length());_client->send();}; // call this to send the buffered string to a websocket client.
    template<class T> inline ESPString &operator =(T arg) { begin(); print(arg); return *this; } // This function allows assignment to an arbitrary scalar value like str = myfloat;
    template<class T> inline ESPString &operator +=(T arg) { print(arg); return *this; } // Concatenation of any type data: str += myfloat;
    char* str(void){return _buf;}
    int format(char *str, ...) {
      va_list argptr;
      va_start(argptr, str);
      int ret = vsnprintf(_cur, _size - (_cur - _buf), str, argptr);
      if (_size) while (*_cur) ++_cur;
      return ret;
    }
    void printHexByte(uint8_t data) {
      static const char map[]={"0123456789ABCDEF"};
      print(map[(uint8_t)((data&0xF0)>>4)]);
      print(map[(uint8_t)(data&0x0F)]);
    }
    void printHexBuffer(char* buf, size_t size) {
      for(size_t i = 0; i < size ; i++) {
        printHexByte((uint8_t)buf[i]);
      }
    }
    void printHexBufferArr(char* buf, size_t size) {
      for(size_t i = 0; i < size ; i++) {
        print(F(" 0x"));
        printHexByte((uint8_t)buf[i]);
        print(',');
      }
    }
    #if defined(ARDUINO) && ARDUINO >= 100
    size_t write(uint8_t b) {
      if (_cur + 1 < _buf + _size) {
        *_cur++ = (char)b;
        *_cur = '\0';
        return 1;
      }
      return 0;
    }
    #else
    void write(uint8_t b) {
      if (_cur + 1 < _buf + _size) {
        *_cur++ = (char)b;
        *_cur = '\0';
      }
    }
    #endif
}; ///ESPString
//-------------------------------------------------------
