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

#ifndef __AESL_AP_SIM_H__
#define __AESL_AP_SIM_H__

#ifndef __cplusplus
#error C++ is required to include this header file
#else

#ifdef __HLS_SYN__
#error ap_int simulation header file is not applicable for synthesis
#else

#include "etc/ap_int_sim.h"
#include "etc/ap_fixed_sim.h"

//Forward declaration
template<int _AP_W, int _AP_I, ap_q_mode _AP_Q, 
         ap_o_mode _AP_O, int _AP_N> class ap_fixed;
template<int _AP_W, int _AP_I, ap_q_mode _AP_Q, 
         ap_o_mode _AP_O, int _AP_N> class ap_ufixed;
template<int _AP_W> class ap_int;
template<int _AP_W> class ap_uint;
//AP_INT
//--------------------------------------------------------
template<int _AP_W>
class ap_int: public ap_private<_AP_W, true> {
#ifdef _MSC_VER
#pragma warning( disable : 4521 4522 )
#endif
public:
    typedef ap_private<_AP_W, true> Base;
    //Constructor
    INLINE ap_int(): Base() {}
    template<int _AP_W2>
    INLINE ap_int(const volatile ap_int<_AP_W2> &op):Base((const ap_private<_AP_W2,true> &)(op)) {}
    
    template<int _AP_W2>
    INLINE ap_int(const ap_int<_AP_W2> &op):Base((const ap_private<_AP_W2,true> &)(op)) {}

    template<int _AP_W2>
    INLINE ap_int(const ap_uint<_AP_W2> &op):Base((const ap_private<_AP_W2,false> &)(op)) {}

    template<int _AP_W2>
    INLINE ap_int(const volatile ap_uint<_AP_W2> &op):Base((const ap_private<_AP_W2,false> &)(op)) {}
    
    template<int _AP_W2, bool _AP_S2>
    INLINE ap_int(const ap_range_ref<_AP_W2, _AP_S2>& ref):Base(ref) {}

    template<int _AP_W2, bool _AP_S2>
    INLINE ap_int(const ap_bit_ref<_AP_W2, _AP_S2>& ref):Base(ref) {}
  
    template<int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
    INLINE ap_int(const ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3>& ref):Base(ref) {}

    template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_int(const ap_fixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
                 :Base(op.to_ap_private()) {}
 
    template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_int(const ap_ufixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
                  :Base(op.to_ap_private()) {}

    template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_int(const volatile ap_fixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
                 :Base(op.to_ap_private()) {}
 
    template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_int(const volatile ap_ufixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
                  :Base(op.to_ap_private()) {}

    template<int _AP_W2, bool _AP_S2>
    INLINE ap_int(const ap_private<_AP_W2, _AP_S2>& op):Base(op) {}

    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_int(const af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, 
                  _AP_N2>& op):Base(op) {}

    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_int(const af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, 
                  _AP_N2>& op):Base(op) {}

    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_int(const ap_fixed_base<_AP_W2, _AP_I2, _AP_S2, 
                    _AP_Q2, _AP_O2, _AP_N2>& op):Base(op.to_ap_private()) {}

#define CTOR(TYPE) \
    INLINE ap_int(TYPE v):Base(v) {}
    CTOR(bool)
    CTOR(signed char)
    CTOR(unsigned char)
    CTOR(short)
    CTOR(unsigned short)
    CTOR(int)
    CTOR(unsigned int)
    CTOR(long)
    CTOR(unsigned long)
    CTOR(unsigned long long)
    CTOR(long long)
    CTOR(float)
    CTOR(double)
    CTOR(const char*)
    // CTOR(const std::string&)
#undef CTOR
    INLINE ap_int(const char* str, signed char rd):Base(str, rd) {}    
     //Assignment
     //Another form of "write"
    INLINE void operator = (const ap_int<_AP_W>& op2) volatile {
      const_cast<ap_int*>(this)->operator = (op2);
    }
 
    INLINE void operator = (const volatile ap_int<_AP_W>& op2) volatile {
      const_cast<Base*>(this)->operator = (op2);
    }

    INLINE ap_int<_AP_W>& operator = (const volatile ap_int<_AP_W>& op2) {
      Base::operator = (const_cast<ap_int<_AP_W>& >(op2));
        return *this;
    }

    INLINE ap_int<_AP_W>& operator = (const ap_int<_AP_W>& op2) {
      Base::operator = ((const ap_private<_AP_W, true>&)op2);
        return *this;
    }

};

//AP_UINT
//---------------------------------------------------------------
template<int _AP_W>
class ap_uint: public ap_private<_AP_W, false> {
#ifdef _MSC_VER
#pragma warning( disable : 4521 4522 )
#endif
public:
    typedef ap_private<_AP_W, false> Base;
    //Constructor
    INLINE ap_uint(): Base() {}
  INLINE ap_uint(const ap_uint<_AP_W>& op) :Base(dynamic_cast<const ap_private<_AP_W, false>&>(op)) {}
  INLINE ap_uint(const volatile ap_uint<_AP_W>& op):Base(dynamic_cast<const volatile ap_private<_AP_W, false>&>(op)){}
    template<int _AP_W2>
    INLINE ap_uint(const volatile ap_uint<_AP_W2> &op):Base((const ap_private<_AP_W2, false>&)(op)) {}

    template<int _AP_W2>
    INLINE ap_uint(const ap_uint<_AP_W2> &op) : Base((const ap_private<_AP_W2, false>&)(op)){}

    template<int _AP_W2>
    INLINE ap_uint(const ap_int<_AP_W2> &op) : Base((const ap_private<_AP_W2, true>&)(op)) {}

    template<int _AP_W2>
    INLINE ap_uint(const volatile ap_int<_AP_W2> &op) : Base((const ap_private<_AP_W2, false>&)(op)) {}

    template<int _AP_W2, bool _AP_S2>
    INLINE ap_uint(const ap_range_ref<_AP_W2, _AP_S2>& ref):Base(ref) {}

    template<int _AP_W2, bool _AP_S2>
    INLINE ap_uint(const ap_bit_ref<_AP_W2, _AP_S2>& ref):Base(ref) {}
  
    template<int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
    INLINE ap_uint(const ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3>& ref):Base(ref) {}

    template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_uint(const ap_fixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
                   :Base(op.to_ap_private()) {}
 
    template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_uint(const ap_ufixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
                   :Base(op.to_ap_private()) {}

    template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_uint(const volatile ap_fixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
                   :Base(op.to_ap_private()) {}
 
    template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_uint(const volatile ap_ufixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
                   :Base(op) {}

    template<int _AP_W2, bool _AP_S2>
    INLINE ap_uint(const ap_private<_AP_W2, _AP_S2>& op):Base(op) {}

    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_uint(const af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, 
                  _AP_N2>& op):Base(op) {}

    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_uint(const af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, 
                  _AP_N2>& op):Base(op) {}

    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_uint(const ap_fixed_base<_AP_W2, _AP_I2, _AP_S2, 
                    _AP_Q2, _AP_O2, _AP_N2>& op):Base(op.to_ap_private()) {}

#define CTOR(TYPE) \
    INLINE ap_uint(TYPE v):Base(v) {}
    CTOR(bool)
    CTOR(signed char)
    CTOR(unsigned char)
    CTOR(short)
    CTOR(unsigned short)
    CTOR(int)
    CTOR(unsigned int)
    CTOR(long)
    CTOR(unsigned long)
    CTOR(unsigned long long)
    CTOR(long long)
    CTOR(float)
    CTOR(double)
    CTOR(const char*)
    // CTOR(const std::string&)
#undef CTOR    
    INLINE ap_uint(const char* str, signed char rd):Base(str, rd) {}    
     //Assignment
     //Another form of "write"
    INLINE void operator = (const ap_uint<_AP_W>& op2) volatile {
      Base::operator = (op2);
    }
 
    INLINE void operator = (const volatile ap_uint<_AP_W>& op2) volatile {
      Base::operator = (op2);
    }

    INLINE ap_uint<_AP_W>& operator = (const volatile ap_uint<_AP_W>& op2) {
      Base::operator = (op2);
        return *this;
    }

    INLINE ap_uint<_AP_W>& operator = (const ap_uint<_AP_W>& op2) {
      Base::operator = ((const ap_private<_AP_W, false>&)(op2));
        return *this;
    }

};

#define ap_bigint ap_int
#define ap_biguint ap_uint

//AP_FIXED
//---------------------------------------------------------------------  
template<int _AP_W, int _AP_I, ap_q_mode _AP_Q = AP_TRN, 
         ap_o_mode _AP_O = AP_WRAP, int _AP_N = 0>
class ap_fixed: public ap_fixed_base<_AP_W, _AP_I, true, _AP_Q, _AP_O, _AP_N> {
#ifdef _MSC_VER
#pragma warning( disable : 4521 4522 )
#endif
public:
    typedef ap_fixed_base<_AP_W, _AP_I, true, _AP_Q, _AP_O, _AP_N> Base;
     //Constructor
    INLINE ap_fixed():Base() {}

    template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
            ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_fixed(const ap_fixed<_AP_W2, _AP_I2,  _AP_Q2, _AP_O2, 
                    _AP_N2>& op): Base(op) {}

    
    template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_fixed(const ap_ufixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2,
                    _AP_N2>& op): Base(ap_fixed_base<_AP_W2, _AP_I2, 
                   false, _AP_Q2, _AP_O2, _AP_N2>(op)) {}

    template<int _AP_W2>
    INLINE ap_fixed(const ap_int<_AP_W2>& op):
                   Base(ap_private<_AP_W2, true>(op)) {}

    template<int _AP_W2>
    INLINE ap_fixed(const ap_uint<_AP_W2>& op):Base(ap_private<_AP_W2, false>(op)) {}

    template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
            ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_fixed(const volatile ap_fixed<_AP_W2, _AP_I2,  _AP_Q2, _AP_O2, 
                    _AP_N2>& op): Base(ap_fixed_base<_AP_W2, _AP_I2, 
                    true, _AP_Q2, _AP_O2, _AP_N2>(op)) {}

    
    template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_fixed(const volatile ap_ufixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2,
                    _AP_N2>& op): Base(ap_fixed_base<_AP_W2, _AP_I2, 
                   false, _AP_Q2, _AP_O2, _AP_N2>(op)) {}

    template<int _AP_W2>
    INLINE ap_fixed(const volatile ap_int<_AP_W2>& op):
                   Base(ap_private<_AP_W2, true>(op)) {}

    template<int _AP_W2>
    INLINE ap_fixed(const volatile ap_uint<_AP_W2>& op):Base(op) {}

    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_fixed(const ap_fixed_base<_AP_W2, _AP_I2, _AP_S2, 
                    _AP_Q2, _AP_O2, _AP_N2>& op):Base(op) {}

    template<int _AP_W2, bool _AP_S2>
    INLINE ap_fixed(const ap_bit_ref<_AP_W2, _AP_S2>& op):
                   Base(op) {}

    template<int _AP_W2, bool _AP_S2>
    INLINE ap_fixed(const ap_range_ref<_AP_W2, _AP_S2>& op):
                   Base(op) {}

    template<int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
    INLINE ap_fixed(const ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3>& op):
                   Base(op) {}

    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_fixed(const af_bit_ref<_AP_W2, _AP_I2, _AP_S2, 
                    _AP_Q2, _AP_O2, _AP_N2>& op): Base(op) {}

    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_fixed(const af_range_ref<_AP_W2, _AP_I2, _AP_S2, 
                    _AP_Q2, _AP_O2, _AP_N2>& op): Base(op) {}

    template<int _AP_W2, bool _AP_S2>
    INLINE ap_fixed(const ap_private<_AP_W2, _AP_S2>& op):Base(op) {}

 #define CTOR(TYPE) \
    INLINE ap_fixed(TYPE v):Base(v) {}
    CTOR(bool)
    CTOR(signed char)
    CTOR(unsigned char)
    CTOR(short)
    CTOR(unsigned short)
    CTOR(int)
    CTOR(unsigned int)
    CTOR(long)
    CTOR(unsigned long)
    CTOR(unsigned long long)
    CTOR(long long)
    CTOR(float)
    CTOR(double)
    CTOR(const char*)
    // CTOR(const std::string&)
#undef CTOR    
    INLINE ap_fixed(const char* str, signed char rd):Base(str, rd) {}    
     
    //Assignment
    INLINE ap_fixed& operator = (const ap_fixed<_AP_W, _AP_I, 
                    _AP_Q, _AP_O, _AP_N>& op) {
      Base::operator = (op);
        return *this;
    }

    INLINE ap_fixed& operator = (const volatile ap_fixed<_AP_W, _AP_I,
                    _AP_Q, _AP_O, _AP_N>& op) {
        Base::operator = (op);
        return *this;
    }

    INLINE void operator = (const ap_fixed<_AP_W, _AP_I, 
                    _AP_Q, _AP_O, _AP_N>& op) volatile {
      Base::operator = (op);
    }

    INLINE void operator = (const volatile ap_fixed<_AP_W, _AP_I,
                    _AP_Q, _AP_O, _AP_N>& op) volatile {
        Base::operator = (op);
    }

};    
//AP_ UFIXED
//--- ----------------------------------------------------------------
template<int _AP_W, int _AP_I, ap_q_mode _AP_Q = AP_TRN,
         ap_o_mode _AP_O = AP_WRAP, int _AP_N = 0>
class ap_ufixed: public ap_fixed_base<_AP_W, _AP_I, false, _AP_Q, _AP_O, _AP_N> {
#ifdef _MSC_VER
#pragma warning( disable : 4521 4522 )
#endif
public:
    typedef ap_fixed_base<_AP_W, _AP_I, false, _AP_Q, _AP_O, _AP_N> Base;
     //Constructor
    INLINE ap_ufixed():Base() {}
     
    template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_ufixed(const ap_fixed<_AP_W2, _AP_I2, _AP_Q2, 
                     _AP_O2, _AP_N2>& op) : Base(ap_fixed_base<_AP_W2, 
                     _AP_I2, true, _AP_Q2, _AP_O2, _AP_N2>(op)) {}
     
     
    template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_ufixed(const ap_ufixed<_AP_W2, _AP_I2,  _AP_Q2, 
                     _AP_O2, _AP_N2>& op): Base(ap_fixed_base<_AP_W2, _AP_I2,
                      false, _AP_Q2, _AP_O2, _AP_N2>(op)) {}
     
    template<int _AP_W2>
    INLINE ap_ufixed(const ap_int<_AP_W2>& op):
      Base((const ap_private<_AP_W2, true>&)(op)) {}

    template<int _AP_W2>
    INLINE ap_ufixed(const ap_uint<_AP_W2>& op):
      Base((const ap_private<_AP_W2, false>&)(op)) {}

    template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_ufixed(const volatile ap_fixed<_AP_W2, _AP_I2, _AP_Q2, 
                     _AP_O2, _AP_N2>& op) : Base(ap_fixed_base<_AP_W2, 
                     _AP_I2, true, _AP_Q2, _AP_O2, _AP_N2>(op)) {}
     
     
    template<int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_ufixed(const volatile ap_ufixed<_AP_W2, _AP_I2,  _AP_Q2, 
                     _AP_O2, _AP_N2>& op): Base(ap_fixed_base<_AP_W2, _AP_I2,
                      false, _AP_Q2, _AP_O2, _AP_N2>(op)) {}
     
    template<int _AP_W2>
    INLINE ap_ufixed(const volatile ap_int<_AP_W2>& op):
                     Base(ap_private<_AP_W2, true>(op)) {}

    template<int _AP_W2>
    INLINE ap_ufixed(const volatile ap_uint<_AP_W2>& op):
                     Base(ap_private<_AP_W2, false>(op)) {}

    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
             ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_ufixed(const ap_fixed_base<_AP_W2, _AP_I2, _AP_S2, _AP_Q2,
                     _AP_O2, _AP_N2>& op):Base(op) {}

    template<int _AP_W2, bool _AP_S2>
    INLINE ap_ufixed(const ap_bit_ref<_AP_W2, _AP_S2>& op):
                   Base(op) {}

    template<int _AP_W2, bool _AP_S2>
    INLINE ap_ufixed(const ap_range_ref<_AP_W2, _AP_S2>& op):
                   Base(op) {}

    template<int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
    INLINE ap_ufixed(const ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3>& op):
                   Base(op) {}

    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_ufixed(const af_bit_ref<_AP_W2, _AP_I2, _AP_S2, 
                    _AP_Q2, _AP_O2, _AP_N2>& op): Base(op) {}

    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_ufixed(const af_range_ref<_AP_W2, _AP_I2, _AP_S2, 
                    _AP_Q2, _AP_O2, _AP_N2>& op): Base(op) {}

    template<int _AP_W2, bool _AP_S2>
    INLINE ap_ufixed(const ap_private<_AP_W2, _AP_S2>& op):Base(op) {}


 #define CTOR(TYPE) \
    INLINE ap_ufixed(TYPE v):Base(v) {}
    CTOR(bool)
    CTOR(signed char)
    CTOR(unsigned char)
    CTOR(short)
    CTOR(unsigned short)
    CTOR(int)
    CTOR(unsigned int)
    CTOR(long)
    CTOR(unsigned long)
    CTOR(unsigned long long)
    CTOR(long long)
    CTOR(float)
    CTOR(double)
    CTOR(const char*)
    // CTOR(const std::string&)
#undef CTOR    
    INLINE ap_ufixed(const char* str, signed char rd):Base(str, rd) {}    
     
    //Assignment

    INLINE ap_ufixed& operator = (const ap_ufixed<_AP_W, _AP_I, 
                    _AP_Q, _AP_O, _AP_N>& op) {
      Base::operator = (op);
        return *this;
    }

    INLINE ap_ufixed& operator = (const volatile ap_ufixed<_AP_W, _AP_I, 
                    _AP_Q, _AP_O, _AP_N>& op) {
        Base::V = const_cast<ap_ufixed&>(op);
        return *this;
    }

    INLINE void operator = (const ap_ufixed<_AP_W, _AP_I, 
                    _AP_Q, _AP_O, _AP_N>& op) volatile {
      Base::operator = (op);
    }

    INLINE void operator = (const volatile ap_ufixed<_AP_W, _AP_I, 
                    _AP_Q, _AP_O, _AP_N>& op) volatile {
        Base::V = const_cast<ap_ufixed&>(op);
    }

};

#if defined(SYSTEMC_H) || defined(SYSTEMC_INCLUDED) 
template<int _AP_W>
INLINE void sc_trace(sc_core::sc_trace_file *tf, const ap_int<_AP_W> &op,
                     const std::string &name) {
    if (tf)
        tf->trace(sc_dt::sc_lv<_AP_W>(op.to_string(2).c_str()), name);
}

template<int _AP_W>
INLINE void sc_trace(sc_core::sc_trace_file *tf, const ap_uint<_AP_W> &op,
                     const std::string &name) {
    if (tf)
        tf->trace(sc_dt::sc_lv<_AP_W>(op.to_string(2).c_str()), name);
}

template<int _AP_W, int _AP_I, ap_q_mode _AP_Q,
         ap_o_mode _AP_O, int _AP_N>
INLINE void sc_trace(sc_core::sc_trace_file *tf, const ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N >&op, const std::string &name) {
        tf->trace(sc_dt::sc_lv<_AP_W>(op.to_string(2).c_str()), name);
}

template<int _AP_W, int _AP_I, ap_q_mode _AP_Q,
         ap_o_mode _AP_O, int _AP_N>
INLINE void sc_trace(sc_core::sc_trace_file *tf, const ap_ufixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N >&op, const std::string &name) {
        tf->trace(sc_dt::sc_lv<_AP_W>(op.to_string(2).c_str()), name);
}
#endif

#endif // __SYNTHESIS__

#endif // __cplusplus

#endif

