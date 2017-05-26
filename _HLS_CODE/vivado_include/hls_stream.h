/*
   __VIVADO_HLS_COPYRIGHT-INFO__

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef X_HLS_STREAM_SIM_H
#define X_HLS_STREAM_SIM_H

/*
 * This file contains a C++ model of hls::stream.
 * It defines C simulation model.
 */
#ifndef __cplusplus

#error C++ is required to include this header file

#else

//////////////////////////////////////////////
// C level simulation models for hls::stream
//////////////////////////////////////////////
#include <queue>
#include <iostream>
#include <typeinfo>
#include <string>
#include <sstream>

#ifdef HLS_STREAM_THREAD_SAFE
#include <mutex>
#include <condition_variable>
#endif

#ifndef _MSC_VER
#include <cxxabi.h>
#include <stdlib.h>
#endif

namespace hls {

template<typename __STREAM_T__>
class stream
{
  protected:
    std::string _name;
    std::deque<__STREAM_T__> _data; // container for the elements
#ifdef HLS_STREAM_THREAD_SAFE
    std::mutex _mutex;
    std::condition_variable _condition_var;
#endif    

  public:
    /// Constructors
    // Keep consistent with the synthesis model's constructors
    stream() {
        static unsigned _counter = 1;
        std::stringstream ss;
#ifndef _MSC_VER
        char* _demangle_name = abi::__cxa_demangle(typeid(*this).name(), 0, 0, 0);
        if (_demangle_name) {
            _name = _demangle_name;
            free(_demangle_name);
        }
        else {
            _name = "hls_stream";
        }
#else
        _name = typeid(*this).name();
#endif

        ss << _counter++;
        _name += "." + ss.str();
    }

    stream(const std::string name) {
    // default constructor,
    // capacity set to predefined maximum
        _name = name;
    }

  /// Make copy constructor and assignment operator private
  private:
    stream(const stream< __STREAM_T__ >& chn):
        _name(chn._name), _data(chn._data) {
    }

    stream& operator = (const stream< __STREAM_T__ >& chn) {
        _name = chn._name;
        _data = chn._data;
        return *this;
    }

  public:
    /// Overload >> and << operators to implement read() and write()
    void operator >> (__STREAM_T__& rdata) {
        read(rdata);
    }

    void operator << (const __STREAM_T__& wdata) {
        write(wdata);
    }


  public:
    /// Destructor
    /// Check status of the queue
    virtual ~stream() {
        if (!_data.empty())
        {
            std::cout << "WARNING: Hls::stream '" 
                      << _name 
                      << "' contains leftover data,"
                      << " which may result in RTL simulation hanging."
                      << std::endl;
        }
    }

    /// Status of the queue
    bool empty() {
#ifdef HLS_STREAM_THREAD_SAFE
        std::lock_guard<std::mutex> lg(_mutex);
#endif
        return _data.empty();
    }    

    bool full() const { return false; }

    /// Blocking read
    void read(__STREAM_T__& head) {
        head = read();
    }

#ifdef HLS_STREAM_THREAD_SAFE
    __STREAM_T__ read() {
        std::unique_lock<std::mutex> ul(_mutex);
        while (_data.empty()) {
            _condition_var.wait(ul);
        }

        __STREAM_T__ elem;
        elem = _data.front();
        _data.pop_front();
        return elem;
    }
#else
    __STREAM_T__ read() {
        __STREAM_T__ elem;
        if (_data.empty()) {
            std::cout << "WARNING: Hls::stream '"
                      << _name 
                      << "' is read while empty,"
                      << " which may result in RTL simulation hanging."
                      << std::endl;
            elem = __STREAM_T__();
        } else {
            elem = _data.front();
            _data.pop_front();
        }
        return elem;
    }
#endif

    /// Blocking write
    void write(const __STREAM_T__& tail) { 
#ifdef HLS_STREAM_THREAD_SAFE
        std::unique_lock<std::mutex> ul(_mutex);
#endif
        _data.push_back(tail);
#ifdef HLS_STREAM_THREAD_SAFE
        _condition_var.notify_one();
#endif
    }

    /// Nonblocking read
    bool read_nb(__STREAM_T__& head) {
#ifdef HLS_STREAM_THREAD_SAFE
        std::lock_guard<std::mutex> lg(_mutex);
#endif    
        bool is_empty = _data.empty();
        if (is_empty) {
            head = __STREAM_T__();
        } else {
            __STREAM_T__ elem(_data.front());
            _data.pop_front();
            head = elem;
        }
        return !is_empty;
    }

    /// Nonblocking write
    bool write_nb(const __STREAM_T__& tail) {
        bool is_full = full();
        write(tail);
        return !is_full;
    }

    /// Fifo size
    size_t size() {
        return _data.size();
    }
};

} // namespace hls

#endif // __cplusplus
#endif  // X_HLS_STREAM_SIM_H
