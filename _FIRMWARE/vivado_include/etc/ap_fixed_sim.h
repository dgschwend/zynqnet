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

#ifndef __AESL_GCC_AP_FIXED_H__
#define __AESL_GCC_AP_FIXED_H__

#ifndef __cplusplus
#error C++ is required to include this header file
#else

#ifdef __HLS_SYN__
#error ap_fixed simulation header file is not applicable for synthesis
#else

#include <math.h>
#include "etc/ap_int_sim.h"
#include <sstream>


#define FLOAT_MAN 23
#define FLOAT_EXP 8
#define FLOAT_MAN_MASK 0x7fffff
#define DOUBLE_MAN 52
#define DOUBLE_EXP 11
// #define DOUBLE_MAN_MASK (~0ULL >> (64-DOUBLE_MAN-2))
#define DOUBLE_MAN_MASK 0x3fffffffffffffULL
#define BIAS(e) ((1ULL<<(e-1))-1)
#define FLOAT_BIAS BIAS(FLOAT_EXP)
#define DOUBLE_BIAS BIAS(DOUBLE_EXP)
#define APFX_IEEE_DOUBLE_E_MAX DOUBLE_BIAS
#define APFX_IEEE_DOUBLE_E_MIN (-DOUBLE_BIAS + 1)
 
/// Forward declaration.
template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N> struct ap_fixed_base;

///Proxy class, which allows bit selection  to be used as both rvalue(for reading) and
//lvalue(for writing)
template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
struct af_bit_ref {
#ifdef _MSC_VER
#pragma warning( disable : 4521 4522 )
#endif
    ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& d_bv;
    int d_index;
public:
    INLINE af_bit_ref(const af_bit_ref<_AP_W, _AP_I, _AP_S, 
                                        _AP_Q, _AP_O, _AP_N>& ref): 
           d_bv(ref.d_bv), d_index(ref.d_index) {
        if (d_index  < 0 ) 
	    fprintf(stderr, "Warning! Index of bit vector  (%d) cannot be negative.\n", d_index);
        if (d_index >= _AP_W)
	    fprintf(stderr, "Warning! Index of bit vector (%d) out of range (%d).\n", d_index, _AP_W);
        //assert(d_index<_AP_W&&"Index of bit vector out of bound");
        //assert(d_index>=0&&"index of bit vector cannot be negative");
    }

    INLINE af_bit_ref(ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>* bv, int index=0):
    d_bv(*bv),d_index(index) {}

    INLINE operator bool() const {
        return d_bv.V[d_index];
    }

    INLINE af_bit_ref& operator=(unsigned long long val) {
        if(val)
            d_bv.V.set(d_index);
        else
            d_bv.V.clear(d_index);
        return *this;
    }

    template<int _AP_W2, bool _AP_S2>
    INLINE af_bit_ref& operator =(const ap_private<_AP_W2,_AP_S2>& val) {
        return operator=(val!=0);
    }
    
    INLINE af_bit_ref& operator =(const af_bit_ref<_AP_W, _AP_I, _AP_S, 
                                        _AP_Q, _AP_O, _AP_N>& val) {
        return operator=((unsigned long long)(bool)val);
    }

    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2>
    INLINE af_bit_ref operator=(const af_bit_ref<_AP_W2,_AP_I2,_AP_S2,_AP_Q2,_AP_O2, _AP_N2>& val) {
        return operator=((unsigned long long)(bool)val);
    }

    template<int _AP_W2, bool _AP_S2>
    INLINE af_bit_ref& operator = ( const ap_bit_ref<_AP_W2, _AP_S2> &val) {
        return operator =((unsigned long long) (bool) val);
    }

    template<int _AP_W2, bool _AP_S2>
    INLINE af_bit_ref& operator = ( const ap_range_ref<_AP_W2,_AP_S2>& val) {
        return operator =((const ap_private<_AP_W2, false>) val);
    }

    template<int _AP_W2, int _AP_I2, bool _AP_S2, 
             ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2> 
    INLINE af_bit_ref& operator= (const af_range_ref<_AP_W2, _AP_I2, _AP_S2,
                                    _AP_Q2, _AP_O2, _AP_N2>& val) {
        return operator=((const ap_private<_AP_W2, false>)(val));
    }
 
    template<int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
    INLINE af_bit_ref& operator= (const ap_concat_ref<_AP_W2, _AP_T3, _AP_W3, _AP_T3>& val) {
        return operator=((const ap_private<_AP_W2 + _AP_W3, false>)(val));
    }

    template<int _AP_W2, int _AP_S2>
    INLINE ap_concat_ref<1, af_bit_ref, _AP_W2, ap_private<_AP_W2, _AP_S2> >
    operator, (ap_private<_AP_W2, _AP_S2>& op) {
        return ap_concat_ref<1, af_bit_ref, _AP_W2, 
               ap_private<_AP_W2, _AP_S2> >(*this, op);
    }

    template<int _AP_W2, int _AP_S2>
    INLINE ap_concat_ref<1, af_bit_ref, 1, ap_bit_ref<_AP_W2, _AP_S2> >
    operator, (const ap_bit_ref<_AP_W2, _AP_S2> &op) {
        return ap_concat_ref<1, af_bit_ref, 1, 
               ap_bit_ref<_AP_W2, _AP_S2> >(*this, 
               const_cast<ap_bit_ref<_AP_W2, _AP_S2>& >(op));
    }

    template<int _AP_W2, int _AP_S2>
    INLINE ap_concat_ref<1, af_bit_ref, _AP_W2, ap_range_ref<_AP_W2, _AP_S2> >
    operator, (const ap_range_ref<_AP_W2, _AP_S2> &op) {
        return ap_concat_ref<1, af_bit_ref, _AP_W2, 
               ap_range_ref<_AP_W2, _AP_S2> >(*this, 
               const_cast<ap_range_ref<_AP_W2, _AP_S2>& >(op));
    }

    template<int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
    INLINE ap_concat_ref<1, af_bit_ref, _AP_W2 + _AP_W3, 
                        ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3> > 
    operator, (const ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3> &op) {
        return ap_concat_ref<1, af_bit_ref, _AP_W2 + _AP_W3, 
                 ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3> >(*this, 
                 const_cast<ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3>& >(op));
    }

    template<int _AP_W2, int _AP_I2, bool _AP_S2, 
             ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_concat_ref<1, af_bit_ref, _AP_W2, 
             af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >
    operator, (const af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, 
               _AP_O2, _AP_N2> &op) {
       return ap_concat_ref<1, af_bit_ref,  _AP_W2,
             af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, 
             _AP_N2> >(*this, const_cast<af_range_ref<_AP_W2, _AP_I2, 
             _AP_S2, _AP_Q2, _AP_O2,_AP_N2>& >(op));
    } 
              
    template<int _AP_W2, int _AP_I2, bool _AP_S2, 
             ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_concat_ref<1, af_bit_ref, 1, 
             af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >
    operator, (const af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, 
               _AP_O2, _AP_N2> &op) {
       return ap_concat_ref<1, af_bit_ref, 1,
             af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >(*this, 
             const_cast<af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2,
              _AP_O2, _AP_N2>& >(op));
    }

    template<int _AP_W2, int _AP_I2, bool _AP_S2, 
             ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2>
    INLINE bool operator == (const af_bit_ref<_AP_W2, _AP_I2, 
                             _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op) {
        return get() == op.get();
    }

    template<int _AP_W2, int _AP_I2, bool _AP_S2, 
             ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2>
    INLINE bool operator != (const af_bit_ref<_AP_W2, _AP_I2, 
                             _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op) {
        return get() != op.get();
    }

    INLINE bool operator ~ () const {
        bool bit = (d_bv.V)[d_index];
        return bit ? false : true;
    }

    INLINE int length() const {
        return 1;
    }

    INLINE bool get() {
        return d_bv.V[d_index];
    }
    
    INLINE bool get() const {
        return d_bv.V[d_index];
    }

    INLINE std::string to_string() const {
        return d_bv.V[d_index] ? "1" : "0";
    }
};

///Range(slice)  reference
//------------------------------------------------------------
template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
struct af_range_ref {
#ifdef _MSC_VER
#pragma warning( disable : 4521 4522 )
#endif
    ap_fixed_base<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> &d_bv;
    int l_index;
    int h_index;

public:
    INLINE af_range_ref(const af_range_ref<_AP_W, _AP_I, _AP_S, 
                                        _AP_Q, _AP_O, _AP_N>& ref):
           d_bv(ref.d_bv), l_index(ref.l_index), h_index(ref.h_index) {}

    INLINE af_range_ref(ap_fixed_base<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>* bv, int h, int l):
        d_bv(*bv),l_index(l),h_index(h) {
        if (h < 0 || l < 0) 
	    fprintf(stderr, "Warning! Higher bound (%d) and lower bound (%d) cannot be negative.\n", h, l);
        if (h >= _AP_W || l >= _AP_W)
	    fprintf(stderr, "Warning! Higher bound (%d) or lower bound (%d) out of range (%d).\n", h, l, _AP_W);
        //assert((h>=0 && l>=0) && "Higher bound and lower bound cannot be negative.");
        //assert((h<_AP_W && l<_AP_W) && "Higher bound or lower bound out of range.");
        //if (h < l) 
          // fprintf(stderr, 
           //"Warning! The bits selected will be returned in reverse order\n");
    }

    INLINE operator ap_private<_AP_W, false> () const {
        if(h_index >= l_index) {
          ap_private<_AP_W, false> val(d_bv.V);
          ap_private<_AP_W,false> mask(-1);
          mask>>=_AP_W-(h_index-l_index+1);
          val>>=l_index;
          return val&=mask;
        } else {
          ap_private<_AP_W, false> val = 0;
          for(int i=0, j=l_index;j>=0&&j>=h_index;j--,i++)
            if((d_bv.V)[j]) val.set(i);
          return val;
        }
    }

    INLINE operator unsigned long long() const {
        return get().to_uint64();
    }

    template<int _AP_W2,bool _AP_S2>
    INLINE af_range_ref& operator =(const ap_private<_AP_W2,_AP_S2>& val) {
      ap_private<_AP_W, false> vval= ap_private<_AP_W, false>(val);
        if(l_index > h_index) {
            for(int i=0, j=l_index;j>=0&&j>=h_index;j--,i++)
                    vval[i]? d_bv.V.set(j):d_bv.V.clear(j);
        } else {
          ap_private<_AP_W,false> mask(-1);
            if(l_index>0) {
                mask<<=l_index;
                vval<<=l_index;
            }
            if(h_index<_AP_W-1) {
              ap_private<_AP_W,false> mask2(-1);
              mask2>>=_AP_W-h_index-1;
              mask&=mask2;
              vval&=mask2;
            }
            mask.flip();
            d_bv.V &= mask;
            d_bv.V |= vval;
        }
        return *this;
    }
#define ASSIGN_INT_TO_AF_RANGE(DATA_TYPE) \
    INLINE af_range_ref& operator = (const DATA_TYPE val) { \
        const ap_private<_AP_W, false> tmpVal(val); \
        return operator = (tmpVal); \
    }
ASSIGN_INT_TO_AF_RANGE(char)
ASSIGN_INT_TO_AF_RANGE(signed char)
ASSIGN_INT_TO_AF_RANGE(short)
ASSIGN_INT_TO_AF_RANGE(unsigned short)
ASSIGN_INT_TO_AF_RANGE(int)
ASSIGN_INT_TO_AF_RANGE(unsigned int)
ASSIGN_INT_TO_AF_RANGE(long)
ASSIGN_INT_TO_AF_RANGE(unsigned long)
ASSIGN_INT_TO_AF_RANGE(long long)
ASSIGN_INT_TO_AF_RANGE(unsigned long long)
#undef ASSIGN_INT_TO_AF_RANGE

    template<int _AP_W3, typename _AP_T3, int _AP_W4, typename _AP_T4>
    INLINE af_range_ref& operator = 
        (const ap_concat_ref <_AP_W3, _AP_T3, _AP_W4, _AP_T4>& val) {
        const ap_private<_AP_W, false> tmpVal(val);
        return operator = (tmpVal);
    }

    template <int _AP_W3, bool _AP_S3>
    INLINE af_range_ref& operator =(const ap_bit_ref<_AP_W3, _AP_S3>& val) {
        const ap_private<_AP_W, false> tmpVal(val);
        return operator = (tmpVal);
    }

    template <int _AP_W3, bool _AP_S3>
    INLINE af_range_ref& operator =(const ap_range_ref<_AP_W3,_AP_S3>& val) {
        const ap_private<_AP_W, false> tmpVal(val);
        return operator =(tmpVal);
    }

    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, 
             ap_o_mode _AP_O2, int _AP_N2>
    INLINE af_range_ref& operator= (const af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& val) {
        const ap_private<_AP_W2, false> tmp= val.get();
        return operator = (tmp);
    }

    INLINE af_range_ref& operator= (const char* val) {
        const ap_private<_AP_W, false> tmp(val);
        return operator = (tmp);
    }

    INLINE af_range_ref& operator= (const af_range_ref<_AP_W, _AP_I, _AP_S, 
                                        _AP_Q, _AP_O, _AP_N>& val) {
        const ap_private<_AP_W, false> tmp= val.get();
        return operator = (tmp);
    }

    template<int _AP_W2, int _AP_I2, bool _AP_S2, 
             ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2>
    INLINE af_range_ref& operator= (const ap_fixed_base<_AP_W2, 
                          _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& val) {
        return operator=(val.to_ap_private());
    }

    template<int _AP_W2, bool _AP_S2>
    INLINE bool operator == (const ap_range_ref<_AP_W2, _AP_S2>& op2) {
        ap_private<_AP_W,false> lhs=get();
        ap_private<_AP_W2,false> rhs=op2.get();
        return lhs==rhs;
    }

    template<int _AP_W2, bool _AP_S2>
    INLINE bool operator != (const ap_range_ref<_AP_W2, _AP_S2>& op2) {
        ap_private<_AP_W,false> lhs=get();
        ap_private<_AP_W2,false> rhs=op2.get();
        return lhs!=rhs;
    }

    template<int _AP_W2, bool _AP_S2>
    INLINE bool operator > (const ap_range_ref<_AP_W2, _AP_S2>& op2) {
        ap_private<_AP_W,false> lhs=get();
        ap_private<_AP_W2,false> rhs=op2.get();
        return lhs>rhs;
    }

    template<int _AP_W2, bool _AP_S2>
    INLINE bool operator >= (const ap_range_ref<_AP_W2, _AP_S2>& op2) {
        ap_private<_AP_W,false> lhs=get();
        ap_private<_AP_W2,false> rhs=op2.get();
        return lhs>=rhs;
    }

    template<int _AP_W2, bool _AP_S2>
    INLINE bool operator < (const ap_range_ref<_AP_W2, _AP_S2>& op2) {
        ap_private<_AP_W,false> lhs=get();
        ap_private<_AP_W2,false> rhs=op2.get();
        return lhs<rhs;
    }

    template<int _AP_W2, bool _AP_S2>
    INLINE bool operator <= (const ap_range_ref<_AP_W2, _AP_S2>& op2) {
        ap_private<_AP_W,false> lhs=get();
        ap_private<_AP_W2,false> rhs=op2.get();
        return lhs<=rhs;
    }

    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2>
    INLINE bool operator == (const af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op2) {
        ap_private<_AP_W,false> lhs=get();
        ap_private<_AP_W2,false> rhs=op2.get();
        return lhs==rhs;
    }

    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2>
    INLINE bool operator != (const af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op2) {
        ap_private<_AP_W,false> lhs=get();
        ap_private<_AP_W2,false> rhs=op2.get();
        return lhs!=rhs;
    }

    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2>
    INLINE bool operator > (const af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op2) {
        ap_private<_AP_W,false> lhs=get();
        ap_private<_AP_W2,false> rhs=op2.get();
        return lhs>rhs;
    }

    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2>
    INLINE bool operator >= (const af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op2) {
        ap_private<_AP_W,false> lhs=get();
        ap_private<_AP_W2,false> rhs=op2.get();
        return lhs>=rhs;
    }

    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2>
    INLINE bool operator < (const af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op2) {
        ap_private<_AP_W,false> lhs=get();
        ap_private<_AP_W2,false> rhs=op2.get();
        return lhs<rhs;
    }

    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2>
    INLINE bool operator <= (const af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op2) {
        ap_private<_AP_W,false> lhs=get();
        ap_private<_AP_W2,false> rhs=op2.get();
        return lhs<=rhs;
    }

    template<int _AP_W2>
    INLINE void set(const ap_private<_AP_W2,false>& val) {
        ap_private<_AP_W,_AP_S> vval=val;
        if(l_index>h_index) {
            for(int i=0, j=l_index;j>=0&&j>=h_index;j--,i++)
                    vval[i]? d_bv.V.set(j):d_bv.V.clear(j);
        } else {
          ap_private<_AP_W,_AP_S> mask(-1);
            if(l_index>0) {
                ap_private<_AP_W,false> mask1(-1);
                mask1>>=_AP_W-l_index;
                mask1.flip();
                mask=mask1;
                //vval&=mask1;
                vval<<=l_index;
            }
            if(h_index<_AP_W-1) {
                ap_private<_AP_W,false> mask2(-1);
                mask2<<=h_index+1;
                mask2.flip();
                mask&=mask2;
                vval&=mask2;
            }
            mask.flip();
            d_bv&=mask;
            d_bv|=vval;
        }

    }

    INLINE ap_private<_AP_W,false> get() const {
        if(h_index<l_index) {
          ap_private<_AP_W, false> val(0);
          for(int i=0, j=l_index;j>=0&&j>=h_index;j--,i++)
            if((d_bv.V)[j]) val.set(i);
          return val;
        } else {
          ap_private<_AP_W, false> val = ap_private<_AP_W,false>(d_bv.V);
          val>>= l_index;
          if(h_index<_AP_W-1)
            {
              ap_private<_AP_W,false> mask(-1);
              mask>>=_AP_W-(h_index-l_index+1);
              val&=mask;
            }
          return val;
        }
    }

    template<int _AP_W2, int _AP_S2>
    INLINE ap_concat_ref<_AP_W, af_range_ref, _AP_W2, ap_private<_AP_W2, _AP_S2> >
    operator, (ap_private<_AP_W2, _AP_S2>& op) {
        return ap_concat_ref<_AP_W, af_range_ref, _AP_W2, 
               ap_private<_AP_W2, _AP_S2> >(*this, op);
    }

    template<int _AP_W2, int _AP_S2>
    INLINE ap_concat_ref<_AP_W, af_range_ref, 1, ap_bit_ref<_AP_W2, _AP_S2> >
    operator, (const ap_bit_ref<_AP_W2, _AP_S2> &op) {
        return ap_concat_ref<_AP_W, af_range_ref, 1, 
               ap_bit_ref<_AP_W2, _AP_S2> >(*this, 
               const_cast<ap_bit_ref<_AP_W2, _AP_S2>& >(op));
    }

    template<int _AP_W2, int _AP_S2>
    INLINE ap_concat_ref<_AP_W, af_range_ref, _AP_W2, ap_range_ref<_AP_W2, _AP_S2> >
    operator, (const ap_range_ref<_AP_W2, _AP_S2> &op) {
        return ap_concat_ref<_AP_W, af_range_ref, _AP_W2, 
               ap_range_ref<_AP_W2, _AP_S2> >(*this, 
               const_cast<ap_range_ref<_AP_W2, _AP_S2>& >(op));
    }

    template<int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
    INLINE ap_concat_ref<_AP_W, af_range_ref, _AP_W2 + _AP_W3, 
                        ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3> > 
    operator, (const ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3> &op) {
        return ap_concat_ref<_AP_W, af_range_ref, _AP_W2 + _AP_W3, 
                 ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3> >(*this,
                 const_cast<ap_concat_ref<_AP_W2, _AP_T2, _AP_W3,
                  _AP_T3>& >(op));
    }

    template<int _AP_W2, int _AP_I2, bool _AP_S2, 
             ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_concat_ref<_AP_W, af_range_ref, _AP_W2, 
             af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >
    operator, (const af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, 
               _AP_O2, _AP_N2> &op) {
       return ap_concat_ref<_AP_W, af_range_ref,  _AP_W2,
             af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >(*this,
             const_cast<af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, 
             _AP_N2>& > (op));
    } 
              
    template<int _AP_W2, int _AP_I2, bool _AP_S2, 
             ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_concat_ref<_AP_W, af_range_ref, 1, 
             af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >
    operator, (const af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, 
               _AP_O2, _AP_N2> &op) {
       return ap_concat_ref<_AP_W, af_range_ref, 1,
             af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >(*this,
             const_cast<af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, 
             _AP_O2, _AP_N2>& >(op));
    } 
 
    INLINE int length() const {
        return h_index>=l_index?h_index-l_index+1:l_index-h_index+1;
    }

    INLINE int to_int() const {
        ap_private<_AP_W,false> val=get();
        return val.to_int();
    }

    INLINE unsigned int to_uint() const {
        ap_private<_AP_W,false> val=get();
        return val.to_uint();
    }

    INLINE long to_long() const {
        ap_private<_AP_W,false> val=get();
        return val.to_long();
    }

    INLINE unsigned long to_ulong() const {
        ap_private<_AP_W,false> val=get();
        return val.to_ulong();
    }

    INLINE ap_slong to_int64() const {
        ap_private<_AP_W,false> val=get();
        return val.to_int64();
    }

    INLINE ap_ulong to_uint64() const {
        ap_private<_AP_W,false> val=get();
        return val.to_uint64();
    }

    INLINE std::string to_string(uint8_t radix) const {
        return get().to_string(radix);
    }

};
//-----------------------------------------------------------------------------
///ap_fixed_base: AutoPilot fixed point
//-----------------------------------------------------------------------------
template<int _AP_W, int _AP_I, bool _AP_S=true, ap_q_mode _AP_Q=AP_TRN, 
         ap_o_mode _AP_O=AP_WRAP, int _AP_N=0>
struct ap_fixed_base {
#ifdef _MSC_VER
#pragma warning( disable : 4521 4522 )
#endif
private:

    INLINE ap_fixed_base(const std::string& val) {
#ifndef NON_C99STRING
//#ifdef C99STRING
        fromString(val);
#else
        ap_private<_AP_W, _AP_S> Tmp(val, 2);
        V = Tmp;
#endif
            //report();
    }

    void fromString(const std::string& val) {
        int radix = 10;
        // std::cout << "val = " << val << "\n";
        std::string s = ap_private_ops::parseString(val, radix);
        // std::cout << "s = " << s << " " << radix << "\n";
        fromString(s, radix);
    }

    void fromString(const std::string& val, unsigned char radix) {
        assert(radix == 2 || radix == 8 || radix == 10 || radix == 16);
        V = 0;
        int startPos = 0;
        int endPos = val.length();
        int decPos = val.find(".");
        if (decPos == -1)
            decPos = endPos;
        bool isNegative = false;
        if (val[0] == '-') {
            isNegative = true;
            ++startPos;
        } else if (val[0] == '+')
            ++startPos;
//         // Skip over leading zeros until we get to the decimal point
//         while (startPos < endPos) {
//             if (val[startPos] != '.' && val[startPos] != '0')
//                 break;
//             ++startPos;
//         }
//         if (startPos == endPos)
//             return;
//         // Skip over trailing zeros until we get to the decimal point
//         while (1) {
//             if (val[endPos-1] != '.' && val[endPos-1] != '0')
//                 break;
//             --endPos;
//         }

        // If there are no integer bits, e.g.:
        // .0000XXXX, then keep at least one bit.
        // If the width is greater than the number of integer bits, e.g.:
        // XXXX.XXXX, then we keep the integer bits
        // if the number of integer bits is greater than the width, e.g.:
        // XXX000 then we keep the integer bits.
        // Always keep one bit.
        ap_fixed_base<AP_MAX(_AP_I,4)+4, AP_MAX(_AP_I,4)+4, false> integer_bits = 0;

        // Figure out if we can shift instead of multiply
        uint32_t shift = (radix == 16 ? 4 : radix == 8 ? 3 : radix == 2 ? 1 : 0);

        // std::cout << val << "\n";
        // std::cout << startPos << " " << decPos << " " << endPos << "\n";

        bool sticky_int = false;

        // Traverse the integer digits from the MSD, multiplying by radix as we go.
        for (int i = startPos; i < decPos; i++) {
            // Get a digit
            char cdigit = val[i];
            if (cdigit == '\0') continue;
            uint32_t digit = ap_private_ops::decode_digit(cdigit, radix);

            sticky_int |= integer_bits[AP_MAX(_AP_I,4)+4 - 1] | integer_bits[AP_MAX(_AP_I,4)+4 - 2] | integer_bits[AP_MAX(_AP_I,4)+4 - 3] | integer_bits[AP_MAX(_AP_I,4)+4 - 4];
            // Shift or multiply the value by the radix
            if (shift)
                integer_bits <<= shift;
            else
                integer_bits *= radix;

            // Add in the digit we just interpreted
            integer_bits += digit;
            // std::cout << "idigit = " << digit << " " << integer_bits.to_string() << " " << sticky_int <<  "\n";
        }
        integer_bits[AP_MAX(_AP_I,4)+4 - 3] = integer_bits[AP_MAX(_AP_I,4)+4 - 3] | sticky_int;

        ap_fixed_base<AP_MAX(_AP_W-_AP_I,0)+4+4, 4, false> fractional_bits = 0;
        bool sticky = false;

        // Traverse the fractional digits from the LSD, dividing by radix as we go.
        for (int i = endPos-1; i >= decPos+1; i--) {
            // Get a digit
            char cdigit = val[i];
            if (cdigit == '\0') continue;
            uint32_t digit = ap_private_ops::decode_digit(cdigit, radix);
            // Add in the digit we just interpreted
            fractional_bits += digit;

            sticky |= fractional_bits[0] | fractional_bits[1] | fractional_bits[2] | fractional_bits[3];
            // Shift or divide the value by the radix
            if (shift)
                fractional_bits >>= shift;
            else
                fractional_bits /= radix;

            // std::cout << "fdigit = " << digit << " " << fractional_bits.to_string() << " " << sticky << "\n";
        }

        // std::cout << "Int =" << integer_bits.to_string() << " " << fractional_bits.to_string() << "\n";

        fractional_bits[0] = fractional_bits[0] | sticky;

        if(isNegative)
            *this = -(integer_bits + fractional_bits);
        else
            *this = integer_bits + fractional_bits;

        // std::cout << "end = " << this->to_string(16) << "\n";
    }

    INLINE void report() {
#if 0
        if (_AP_W > 1024 && _AP_W <= 4096) {
            fprintf(stderr, "[W] W=%d is out of bound (1<=W<=1024):"
                            " for synthesis, please define macro AP_INT_TYPE_EXT(N) to"
                            " extend the valid range.\n", _AP_W);
        } else
#endif
        if (!_AP_S  && _AP_O == AP_WRAP_SM) {
            fprintf(stderr, "ap_ufxied<...> cannot support AP_WRAP_SM.\n");
            exit(1);
        } 
        if (_AP_W > MAX_MODE(AP_INT_MAX_W) * 1024) {
            fprintf(stderr, "[E] ap_%sfixed<%d, ...>: Bitwidth exceeds the "
                   "default max value %d. Please use macro "
                   "AP_INT_MAX_W to set a larger max value.\n", 
                            _AP_S?"":"u", _AP_W, 
                            MAX_MODE(AP_INT_MAX_W) * 1024);
            exit(1);
        } 
    }

    //helper function
    INLINE unsigned long long doubleToRawBits(double pf)const {
        union {
            unsigned long long __L;
            double __D;
        }LD;
        LD.__D=pf;
        return LD.__L;    
    }


    INLINE double rawBitsToDouble(unsigned long long pi) const {
        union {
            unsigned long long __L;
            double __D;
        }LD;
        LD.__L=pi;
        return LD.__D;
    }

    INLINE float rawBitsToFloat(uint32_t pi) const {
        union {
            uint32_t __L;
            float __D;
        }LD;
        LD.__L = pi;
        return LD.__D;
    }


public:
    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2> friend struct
ap_fixed_base;
    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2> friend struct
af_bit_ref;
    
    INLINE void overflow_adjust(bool underflow, bool overflow,
                                bool lD, bool sign) {
        if (!overflow && !underflow) return;
        switch (_AP_O) {
            case AP_WRAP:
                if (_AP_N == 0) 
                    return;
                if (_AP_S) {
                    //signed SC_WRAP 
                    //n_bits == 1;
                    if (_AP_N > 1) {
                        ap_private<_AP_W, _AP_S>  mask(-1);
                        if (_AP_N >= _AP_W) mask = 0;
                        else mask.lshr(_AP_N);
                        if (sign) 
                            V &= mask;
                        else
                            V |= ~mask;
                    }
                    sign ? V.set(_AP_W - 1) : V.clear(_AP_W - 1);
                } else {
                    //unsigned SC_WRAP
                    ap_private<_AP_W, _AP_S> mask(-1);
                    if (_AP_N >= _AP_W) mask = 0;
                    else mask.lshr(_AP_N);
                    mask.flip();
                    V |= mask;  
                }
                break;
            case AP_SAT_ZERO:
                V.clear();
                break;
            case AP_WRAP_SM:
                {
                bool Ro = ap_private_ops::get<_AP_W, _AP_S, _AP_W -1>(V); // V[_AP_W -1]; 
                if (_AP_N == 0) {
                    if (lD != Ro) { 
                        V.flip();
                        lD ? ap_private_ops::set<_AP_W, _AP_S, _AP_W - 1>(V) :
                            ap_private_ops::clear<_AP_W, _AP_S, _AP_W - 1>(V);
                    }
                } else {
                    if (_AP_N == 1 && sign != Ro) {
                        V.flip();
                    } else if (_AP_N > 1) {
                        bool lNo = ap_private_ops::get<_AP_W, _AP_S, _AP_W - _AP_N> (V); // V[_AP_W - _AP_N];
                        if (lNo == sign) 
                            V.flip();
                        ap_private<_AP_W, false> mask(-1);
                        if (_AP_N >= _AP_W) mask = 0;
                        else mask.lshr(_AP_N);
                        if (sign) 
                            V &= mask;
                        else 
                            V |= mask.flip();
                        sign ? ap_private_ops::set<_AP_W, _AP_S, _AP_W - 1>(V) : ap_private_ops::clear<_AP_W, _AP_S, _AP_W - 1>(V);
                    }   
                }
                }
                break;
            default:
                if (_AP_S) {
                    if (overflow) {
                        V.set(); ap_private_ops::clear<_AP_W, _AP_S, _AP_W-1>(V);
                    } else if (underflow) {
                        V.clear();
                        ap_private_ops::set<_AP_W, _AP_S, _AP_W-1>(V);
                        if(_AP_O == AP_SAT_SYM)
                            ap_private_ops::set<_AP_W, _AP_S, 0>(V);
                    }
                } else {
                    if (overflow)
                        V.set();
                    else if (underflow)
                        V.clear();
                }
        }
    }

    INLINE bool quantization_adjust(bool qb, bool r, bool s) {
        bool carry=ap_private_ops::get<_AP_W, _AP_S, _AP_W-1>(V);
        switch (_AP_Q) {
            case AP_TRN:
                return false;
            case AP_RND_ZERO:
                qb &= s || r; 
                break;
            case AP_RND_MIN_INF:
                qb &= r;
                break;
            case AP_RND_INF:
                qb &= !s || r;
                break;
            case AP_RND_CONV:
                qb &= ap_private_ops::get<_AP_W, _AP_S, 0>(V) || r;
                break;
            case AP_TRN_ZERO:
                qb = s && ( qb || r );
                break;
            default:;

        }
        if(qb) ++V;
        //only when old V[_AP_W-1]==1 && new V[_AP_W-1]==0 
        return carry && !(ap_private_ops::get<_AP_W, _AP_S, _AP_W-1>(V)); //(!V[_AP_W-1]); 
    }
    
    template<int _AP_W2, int _AP_I2, bool _AP_S2>
    struct RType {
        enum {
            _AP_F=_AP_W-_AP_I, 
            F2=_AP_W2-_AP_I2,
            mult_w = _AP_W+_AP_W2,
            mult_i = _AP_I+_AP_I2,
            mult_s = _AP_S||_AP_S2,
            plus_w = AP_MAX(_AP_I+(_AP_S2&&!_AP_S),_AP_I2+(_AP_S&&!_AP_S2))+1+AP_MAX(_AP_F,F2),
            plus_i = AP_MAX(_AP_I+(_AP_S2&&!_AP_S),_AP_I2+(_AP_S&&!_AP_S2))+1,
            plus_s = _AP_S||_AP_S2,
            minus_w = AP_MAX(_AP_I+(_AP_S2&&!_AP_S),_AP_I2+(_AP_S&&!_AP_S2))+1+AP_MAX(_AP_F,F2),
            minus_i = AP_MAX(_AP_I+(_AP_S2&&!_AP_S),_AP_I2+(_AP_S&&!_AP_S2))+1,
            minus_s = true,
#ifndef __SC_COMPATIBLE__
            div_w = _AP_W + AP_MAX(_AP_W2 - _AP_I2, 0) + _AP_S2,
#else
            div_w = _AP_W + AP_MAX(_AP_W2 - _AP_I2, 0) + _AP_S2 + AP_MAX(_AP_I2, 0),
#endif
            div_i = _AP_I + (_AP_W2-_AP_I2) + _AP_S2,
            div_s = _AP_S||_AP_S2,
            logic_w = AP_MAX(_AP_I+(_AP_S2&&!_AP_S),_AP_I2+(_AP_S&&!_AP_S2))+AP_MAX(_AP_F,F2),
            logic_i = AP_MAX(_AP_I+(_AP_S2&&!_AP_S),_AP_I2+(_AP_S&&!_AP_S2)),
            logic_s = _AP_S||_AP_S2
        };

        typedef ap_fixed_base<mult_w, mult_i, mult_s> mult;
        typedef ap_fixed_base<plus_w, plus_i, plus_s> plus;
        typedef ap_fixed_base<minus_w, minus_i, minus_s> minus;
        typedef ap_fixed_base<logic_w, logic_i, logic_s> logic;
        typedef ap_fixed_base<div_w, div_i, div_s> div;
        typedef ap_fixed_base<_AP_W, _AP_I, _AP_S> arg1;
    };

 
    /// Constructors.
    // -------------------------------------------------------------------------
#if 0
    #ifdef __SC_COMPATIBLE__
    INLINE ap_fixed_base():V(uint32_t(_AP_W), uint64_t(0)) {}
    #else 
    INLINE ap_fixed_base():V(uint32_t(_AP_W)) {} 
    #endif
#else
    INLINE ap_fixed_base() {} 
#endif
    //  INLINE ap_fixed_base():V() {} 
    //  INLINE  explicit ap_fixed_base(const ap_private<_AP_W+_AP_I, _AP_S>& _V):V(_V) {}
    INLINE ap_fixed_base(const ap_fixed_base& op):V(op.V) {}
    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_fixed_base(const ap_fixed_base<_AP_W2,_AP_I2,_AP_S2,_AP_Q2,_AP_O2, _AP_N2>& op):V(0) {
        enum {N2=_AP_W2,_AP_F=_AP_W-_AP_I,F2=_AP_W2-_AP_I2,QUAN_INC=F2>_AP_F && !(_AP_Q==AP_TRN ||
                (_AP_Q==AP_TRN_ZERO && !_AP_S2))};
        if (!op) return;
        bool carry=false;
        //handle quantization
        enum { sh_amt =(F2>_AP_F)?F2-_AP_F:_AP_F-F2};
        const ap_private<_AP_W2, _AP_S2>& val = op.V;
        bool neg_src=val.isNegative();
        if (F2==_AP_F)
            V=val;

        else if (F2>_AP_F) {
            if (sh_amt >= _AP_W2)
                V = neg_src ? -1 : 0;
            else
                V = _AP_S2?val.ashr(sh_amt):val.lshr(sh_amt);
            if (_AP_Q!=AP_TRN && !(_AP_Q==AP_TRN_ZERO && !_AP_S2)) {
                bool qb = false;
                if (F2-_AP_F>_AP_W2)
                    qb = neg_src;
                else
                    qb = ap_private_ops::get<_AP_W2, _AP_S2, F2-_AP_F-1>(val); 

                bool r=false;
                enum { pos3 = F2-_AP_F-2};
                if(pos3>=_AP_W2-1)
                    r=val!=0;
                else if (pos3>=0)
                r = (val<<(_AP_W2-1-pos3))!=0;
                carry = quantization_adjust(qb,r,neg_src);
            }
        } else { //no quantization
            if (sh_amt < _AP_W) {
                V=val; 
                V <<= sh_amt;
            }
        }
        //hanle overflow/underflow
        if ((_AP_O!=AP_WRAP || _AP_N != 0) && 
                ((!_AP_S && _AP_S2) || _AP_I-_AP_S < 
                 _AP_I2 - _AP_S2 + (QUAN_INC|| (_AP_S2 &&
                     _AP_O==AP_SAT_SYM)))) {//saturation
            bool deleted_zeros = _AP_S2?true:!carry, 
                 deleted_ones = true;
            bool lD=(_AP_I2>_AP_I) && (_AP_W2-_AP_I2+_AP_I>=0) && 
                ap_private_ops::get<_AP_W2, _AP_S2, _AP_W2-_AP_I2+_AP_I>(val);
            enum { pos1=F2-_AP_F+_AP_W, pos2=F2-_AP_F+_AP_W+1};
            if (pos1 < _AP_W2) {
                bool Range1_all_ones= true;
                bool Range1_all_zeros= true;
                if (pos1 >= 0) {
                    enum { __W = (_AP_W2-pos1) > 0 ? (_AP_W2-pos1) : 1 };
                    const ap_private<__W, _AP_S2> Range1=ap_private<__W, _AP_S2>(val.lshr(pos1));
                    Range1_all_ones=Range1.isAllOnesValue();
                    Range1_all_zeros=Range1.isMinValue();
                } else {
                    Range1_all_ones=false;
                    Range1_all_zeros=val.isMinValue();
                }
                bool Range2_all_ones=true;
                if (pos2<_AP_W2 && pos2>=0) {
                    enum { __W = (_AP_W2-pos2)>0 ? (_AP_W2-pos2) : 1};
                    ap_private<__W, true> Range2=ap_private<__W, true>(val.lshr(pos2));                      
                    Range2_all_ones=Range2.isAllOnesValue();
                } else if(pos2<0)
                    Range2_all_ones=false;

                deleted_zeros=deleted_zeros && (carry?Range1_all_ones:Range1_all_zeros);
                deleted_ones=carry?Range2_all_ones&&(F2-_AP_F+_AP_W<0||!lD)
                    :Range1_all_ones;
                neg_src= neg_src&&!(carry && Range1_all_ones);
            } else
                neg_src = neg_src && V[_AP_W-1];

            bool neg_trg= V.isNegative();
            bool overflow=(neg_trg||!deleted_zeros) && !val.isNegative();
            bool underflow=(!neg_trg||!deleted_ones)&&neg_src;
            //printf("neg_src = %d, neg_trg = %d, deleted_zeros = %d, deleted_ones = %d, overflow = %d, underflow = %d\n",
            //        neg_src, neg_trg, deleted_zeros, deleted_ones,
            //        overflow, underflow);
            if(_AP_O==AP_SAT_SYM && _AP_S2 && _AP_S)
                underflow |= neg_src && (_AP_W>1?V.isMinSignedValue():true);
            overflow_adjust(underflow, overflow, lD, neg_src);
        }
        report();
    }

    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_fixed_base(const volatile ap_fixed_base<_AP_W2,_AP_I2,
                _AP_S2,_AP_Q2,_AP_O2, _AP_N2> &op) : V(op.V) {
        *this = const_cast<ap_fixed_base<_AP_W2,_AP_I2,
        _AP_S2,_AP_Q2,_AP_O2, _AP_N2>&>(op);
    }

    template<int _AP_W2, bool _AP_S2>
    INLINE ap_fixed_base(const ap_private<_AP_W2,_AP_S2>& op) {
        ap_fixed_base<_AP_W2,_AP_W2,_AP_S2> f_op;
        f_op.V=op;
        *this = f_op;
    }  

    INLINE ap_fixed_base(bool b) {
        *this=(ap_private<1,false>)b;
        report();
    }

    INLINE ap_fixed_base(char b) {
        *this=(ap_private<8,false>)b;
        report();
    }

    INLINE ap_fixed_base(signed char b) {
        *this=(ap_private<8,true>)b;
        report();
    }

    INLINE ap_fixed_base(unsigned char b) {
        *this=(ap_private<8,false>)b;
        report();
    }

    INLINE ap_fixed_base(signed short b) {
        *this=(ap_private<16,true>)b;
        report();
    }

    INLINE ap_fixed_base(unsigned short b) {
        *this=(ap_private<16,false>)b;
        report();
    }

    INLINE ap_fixed_base(signed int b) {
        *this=(ap_private<32,true>)b;
        report();
    }

    INLINE ap_fixed_base(unsigned int b) {
        *this=(ap_private<32,false>)b;
        report();
    }
# if defined __x86_64__
    INLINE ap_fixed_base(signed long b) {
        *this=(ap_private<64,true>)b;
        report();
    }

    INLINE ap_fixed_base(unsigned long  b) {
        *this=(ap_private<64,false>)b;
        report();
    }
# else 
    INLINE ap_fixed_base(signed long  b) {
        *this=(ap_private<32,true>)b;
        report();
    }

    INLINE ap_fixed_base(unsigned long  b) {
        *this=(ap_private<32,false>)b;
        report();
    }
# endif

    INLINE ap_fixed_base(ap_slong b) {
        *this=(ap_private<64,true>)b;
        report();
    }

    INLINE ap_fixed_base(ap_ulong b) {
        *this=(ap_private<64,false>)b;
        report();
    }



    //#define NON_C99STRING

    INLINE ap_fixed_base(const char* val):V(0) {
#ifndef NON_C99STRING
//#ifdef C99STRING
        fromString(val);
#else
        ap_private<_AP_W, _AP_S> Tmp(val);
        V = Tmp;
#endif
    }

    INLINE  ap_fixed_base(const char* val, signed char radix): V(0) {
//#ifndef NON_C99STRING
#ifdef C99STRING
        fromString(val, radix);
#else
        ap_private<_AP_W, _AP_S> Tmp(val, radix);
        V = Tmp;
#endif
    }

    template<int _AP_W2, bool _AP_S2>
    INLINE ap_fixed_base(const ap_bit_ref<_AP_W2, _AP_S2>& op) {
        *this = ((bool)op);
        report();
    }

    template<int _AP_W2, bool _AP_S2>
    INLINE ap_fixed_base(const ap_range_ref<_AP_W2, _AP_S2>& op) {
        *this = ap_private<_AP_W2, _AP_S2>(op);
        report();
    }

    template<int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
    INLINE ap_fixed_base(const ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3>& op) {
        *this = ((const ap_private<_AP_W2 + _AP_W3, false>&)(op));
        report();
    }

    template<int _AP_W2, int _AP_I2, bool _AP_S2, 
             ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_fixed_base(const af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op) {
         *this = (bool(op));
        report();
    }

    template<int _AP_W2, int _AP_I2, bool _AP_S2, 
             ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2>
    INLINE ap_fixed_base(const af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op) {
        *this = (ap_private<_AP_W2, false>(op));
        report();
    } 
 
    INLINE ap_fixed_base(double d):V(0) {
        if(!d) return;  
        const bool isneg=d<0;

        const uint64_t ireg=doubleToRawBits(isneg?-d:d);
        if((ireg&0x7fffffffffffffffULL)!=0) {          
            const int32_t exp=(((ireg)>>DOUBLE_MAN)&0x07ff)-DOUBLE_BIAS;
            ap_private<DOUBLE_MAN+2, true> man = ireg & DOUBLE_MAN_MASK;
           
            if (exp == APFX_IEEE_DOUBLE_E_MAX + 1 &&
                man.range(DOUBLE_MAN - 1, 0) != 0) {
                //this is NaN
                fprintf(stderr, "[E] ap_%sfixed<%d, ...>: trying to "
                   "assign NaN to fixed point value.\n",
                            _AP_S?"":"u", _AP_W);
                exit(1);
            }
            man.clear(DOUBLE_MAN+1);
            man.set(DOUBLE_MAN);
            if(isneg) {
                man.flip();
                man++;
            }

            enum {_AP_S2=true,  _AP_W2=DOUBLE_MAN+2,_AP_F=_AP_W -_AP_I };
            const int _AP_I2=exp+2;
            const int F2=_AP_W2-_AP_I2;
            const bool QUAN_INC=F2>_AP_F && !(_AP_Q==AP_TRN || (_AP_Q==AP_TRN_ZERO && 
                    !_AP_S2));
            bool carry=false;
            //handle quantization
            const unsigned sh_amt=abs(F2-_AP_F); // sh_amt = F2>_AP_F ? F2 -_AP_F : _AP_F-F2; 
            if (F2==_AP_F )
                V=man;
            else if (F2>_AP_F) {
                if(sh_amt >= DOUBLE_MAN+2)
                    V=isneg?-1:0;
                else
                    V= ap_private<DOUBLE_MAN+2, true>((man>>sh_amt) | 
                       ((man & 1ULL<<(DOUBLE_MAN+1))? 
                        (DOUBLE_MAN_MASK>>(DOUBLE_MAN+2-sh_amt) 
                        <<(DOUBLE_MAN+2-sh_amt)):0));

                if (_AP_Q!=AP_TRN && !(_AP_Q==AP_TRN_ZERO && !_AP_S2)) {
                    const bool qb=((F2-_AP_F > DOUBLE_MAN+2) ? isneg : (man & (1ULL<<(F2-_AP_F-1))) != 0);
                    const int pos3=F2-_AP_F-2;
                    const bool r = (pos3>= 0) ? (man << AP_MAX(0, _AP_W2-pos3-1)& DOUBLE_MAN_MASK)!=0  : false;
                    carry = quantization_adjust(qb,r,isneg);
                }
            }
            else { //no quantization
                //                V=man;
                if (sh_amt < _AP_W) {
                    V = man; 
                    V <<= sh_amt;
                }
            }
            //handle overflow/underflow
            if((_AP_O != AP_WRAP || _AP_N != 0) && 
                    ((!_AP_S && _AP_S2) || _AP_I-_AP_S <
                     _AP_I2-_AP_S2+(QUAN_INC|| (_AP_S2 &&
                         _AP_O==AP_SAT_SYM)) )) {// saturation
                bool deleted_zeros = _AP_S2?true:!carry, 
                     deleted_ones = true;
                bool neg_src;
                const bool lD=(_AP_I2>_AP_I) && (_AP_W2-_AP_I2+_AP_I>=0) &&
                    (man & (1ULL <<(DOUBLE_MAN+2-_AP_I2+_AP_I)));
                int pos1=F2+_AP_W-_AP_F;
                if (pos1 < _AP_W2) {
                    int pos2=pos1+1;
                    bool Range1_all_ones=true;
                    bool Range1_all_zeros=true;
                    if (pos1>=0) {
                        ap_private<DOUBLE_MAN+2,_AP_S> Range1=
                            ap_private<DOUBLE_MAN+2,_AP_S>((man >> pos1) | ((1ULL<<(DOUBLE_MAN+1)&man) ? (DOUBLE_MAN_MASK >> (DOUBLE_MAN+2-pos1) <<(DOUBLE_MAN+2-pos1)):0));
                        Range1_all_ones = Range1.isAllOnesValue(); // Range1.isAllOnesValue();
                        Range1_all_zeros = Range1.isMinValue(); // Range1.isMinValue();
                    } else {
                        Range1_all_ones=false;
                        Range1_all_zeros = man==0; // man.isMinValue();
                    }
                    bool Range2_all_ones=true;
                    if (pos2<_AP_W2 && pos2>=0) {
                        ap_private<DOUBLE_MAN+2, _AP_S> Range2=
                            ap_private<DOUBLE_MAN+2, _AP_S>((man >> pos2) | ((1ULL<<(DOUBLE_MAN+1)&man) ? (DOUBLE_MAN_MASK >> (DOUBLE_MAN+2-pos2) <<(DOUBLE_MAN+2-pos2)):0));
                        Range2_all_ones=Range2.isAllOnesValue(); // Range2.isAllOnesValue();
                    } else if (pos2<0)
                        Range2_all_ones=false;
                    deleted_zeros=deleted_zeros && (carry?Range1_all_ones:Range1_all_zeros);
                    deleted_ones=carry?Range2_all_ones&&(F2-_AP_F+_AP_W<0||!lD) : Range1_all_ones;
                    neg_src=isneg&&!(carry&Range1_all_ones);
                } else
                    neg_src = isneg &&  V[_AP_W -1];

                const bool neg_trg=V.isNegative();
                const bool overflow=(neg_trg||!deleted_zeros) && !isneg;
                bool underflow=(!neg_trg||!deleted_ones)&&neg_src;
                //printf("neg_src = %d, neg_trg = %d, deleted_zeros = %d,
                //          deleted_ones = %d, overflow = %d, underflow = %d\n",
                //          neg_src, neg_trg, deleted_zeros, deleted_ones,
                //          overflow, underflow);
                if(_AP_O==AP_SAT_SYM && _AP_S2 && _AP_S)
                    underflow |= neg_src && (_AP_W>1?V.isMinSignedValue():true);
                overflow_adjust(underflow,overflow,lD, neg_src);
            }    
        }
        report();
    }
        

        ///assign operators
    //-------------------------------------------------------------------------

    INLINE void operator=(const ap_fixed_base<_AP_W, _AP_I, _AP_S,
                                    _AP_Q, _AP_O, _AP_N>& op) volatile {
        V = op.V;
    }

    INLINE ap_fixed_base& operator=(const ap_fixed_base<_AP_W, _AP_I, _AP_S,
                                    _AP_Q, _AP_O, _AP_N>& op) {
        V = op.V;
        return *this;
    }
  
    INLINE void operator=(const volatile ap_fixed_base<_AP_W, _AP_I, _AP_S,
                                    _AP_Q, _AP_O, _AP_N>& op) volatile {
        V = op.V;
    }

    INLINE ap_fixed_base& operator=(const volatile ap_fixed_base<_AP_W, _AP_I, _AP_S,
                                    _AP_Q, _AP_O, _AP_N>& op) {
        V = op.V;
        return *this;
    }

    // Set this ap_fixed_base with a bits string. That means the ssdm_int::V
    // inside this ap_fixed_base is assigned by bv.
    // Note the input parameter should be a fixed-point formatted bit string.
    INLINE ap_fixed_base& setBits(unsigned long long bv) {
        V=bv;
        return *this;
    }

    // Return a ap_fixed_base object whose ssdm_int::V is assigned by bv.
    // Note the input parameter should be a fixed-point formatted bit string.
    static INLINE ap_fixed_base bitsToFixed(unsigned long long bv) {
        ap_fixed_base Tmp=bv;
        return Tmp;
    }

    // Explicit conversion functions to ap_private that captures 
    // all integer bits (bits are truncated)
    INLINE ap_private<AP_MAX(_AP_I,1),_AP_S> 
    to_ap_private(bool Cnative = true) const {
        ap_private<AP_MAX(_AP_I,1),_AP_S> ret = ap_private<AP_MAX(_AP_I,1),_AP_S> ((_AP_I >= 1) ? (_AP_S==true ? V.ashr(AP_MAX(0,_AP_W - _AP_I)) : V.lshr(AP_MAX(0,_AP_W - _AP_I))) : ap_private<_AP_W, _AP_S>(0)); 

        if (Cnative) {
            bool r = false;
            if (_AP_I < _AP_W) {
                if (_AP_I > 0) r = !(V.getLoBits(_AP_W - _AP_I).isMinValue());
                else r = !(V.isMinValue());
            } 
            if (r && V.isNegative()) { // if this is negative integer
                ++ret;//ap_private<AP_MAX(_AP_I,1),_AP_S>(1,_AP_S);
            }
        } else {
            //Follow OSCI library, conversion from sc_fixed to sc_int
        }
        return ret;
    }

    template<int _AP_W2, bool _AP_S2>
    INLINE operator ap_private<_AP_W2,_AP_S2> () const {
        return (ap_private<_AP_W2,_AP_S2>)to_ap_private();
    }

    template<int _AP_W2, bool _AP_S2, int _AP_N2>
    INLINE operator ap_private<_AP_W2,_AP_S2,_AP_N2> () const {
        return (ap_private<_AP_W2,_AP_S2,_AP_N2>)to_ap_private();
    }

    //Explict conversion function to C built-in integral type
    INLINE int to_int() const {
        return to_ap_private().to_int();
    }

    INLINE int to_uint() const {
        return to_ap_private().to_uint();
    }

    INLINE ap_slong to_int64() const {
        return to_ap_private().to_int64();
    }

    INLINE ap_ulong to_uint64() const {
        return to_ap_private().to_uint64();
    }

    INLINE double to_double() const {
        if(!V)
            return 0;
        bool isneg = _AP_S && V[_AP_W-1];
        uint64_t res = isneg ? 0x8000000000000000ULL : 0;
        ap_private<_AP_W, false>  tmp(V);
        if (isneg) tmp = ap_private<_AP_W, false>(-V);
        int i = _AP_W -1 - tmp.countLeadingZeros();
        int exp = _AP_I-(_AP_W-i);
        res|=((uint64_t)(exp+DOUBLE_BIAS))<<DOUBLE_MAN;
        if(i!=0) {
            tmp.clear(i);
            uint64_t man = ((i>DOUBLE_MAN)?tmp.lshr(i-DOUBLE_MAN):tmp).to_uint64() & DOUBLE_MAN_MASK;
            res |= i<DOUBLE_MAN ? (man)<<(DOUBLE_MAN-i)& DOUBLE_MAN_MASK  : man;
        }
        double dp=rawBitsToDouble(res);
        return dp;
    }

    INLINE float to_float() const {
        if(!V)
            return 0;
        bool isneg = _AP_S && V[_AP_W-1];
        uint64_t res = isneg ? 0x80000000ULL : 0;
        ap_private<_AP_W, false>  tmp = V;
        if (isneg) tmp = -tmp;
        int i = _AP_W -1 - tmp.countLeadingZeros();
        int exp = _AP_I-(_AP_W-i);
        res|=((uint64_t)(exp+FLOAT_BIAS))<<FLOAT_MAN;
        if(i!=0) {
            tmp.clear(i);
            uint32_t man = ((i>FLOAT_MAN) ? tmp.lshr(i-FLOAT_MAN) : tmp).to_uint() & FLOAT_MAN_MASK;
            res |= i<FLOAT_MAN ? (man)<<(FLOAT_MAN-i)& FLOAT_MAN_MASK : man;
        }
        float dp=rawBitsToFloat(res);
        return dp;
    }

    INLINE operator long double () const {
        return to_double();
    }

    INLINE operator double () const {
        return to_double();
    }
#ifndef __SC_COMPATIBLE__ 
    INLINE operator float () const {
           return to_float();
    }
    
    INLINE operator char () const {
        return (char) to_int();
    }

    INLINE operator signed char () const {
        return (signed char) to_int();
    }

    INLINE operator unsigned char () const {
        return (unsigned char) to_uint();
    }

    INLINE operator short () const {
        return (short) to_int();
    }

    INLINE operator unsigned short () const {
        return (unsigned short) to_uint();
    }

    INLINE operator int () const {
        return to_int();
    } 
 
    INLINE operator unsigned int () const {
        return to_uint();
    }
#if 1
#ifdef __x86_64__
    INLINE operator long () const {
        return (long)to_int64();
    }

    INLINE operator unsigned long () const {
        return (unsigned long) to_uint64();
    }
#else
    INLINE operator long () const {
        return to_int64();
    }

    INLINE operator unsigned long () const {
        return to_uint64();
    }
    
#endif
#endif
    INLINE operator unsigned long long () const {
        return to_uint64();
    }

    INLINE operator long long () const {
        return to_int64();
    }
#endif
  
    INLINE std::string to_string(uint8_t radix=2, bool sign=_AP_S) const;
  
    INLINE ap_slong bits_to_int64() const  {
      ap_private<AP_MIN(_AP_W, 64), _AP_S> res(V);
        return (ap_slong) res;
    }
    
    INLINE ap_ulong bits_to_uint64() const {
      ap_private<AP_MIN(64,_AP_W), _AP_S> res(V);
      return (ap_ulong) res;
    }
    
    INLINE int length() const {return _AP_W;}

    // Count the number of zeros from the most significant bit 
    // to the first one bit. Note this is only for ap_fixed_base whose 
    // _AP_W <= 64, otherwise will incur assertion.
    INLINE int countLeadingZeros() {
        return V.countLeadingZeros();
    }

    ///Arithmetic:Binary
    //-------------------------------------------------------------------------
    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2>
    INLINE typename RType<_AP_W2,_AP_I2,_AP_S2>::mult
    operator * (const ap_fixed_base<_AP_W2,_AP_I2,_AP_S2,_AP_Q2,_AP_O2, _AP_N2>& op2) const {
        typename RType<_AP_W2,_AP_I2,_AP_S2>::mult r;
        r.V = V * op2.V;
        return r;
    }
    
    template<int _AP_W1, int _AP_I1, bool _AP_S1, int _AP_W2, int _AP_I2, bool _AP_S2>
    static INLINE ap_fixed_base multiply(const ap_fixed_base<_AP_W1,_AP_I1,_AP_S1>& op1, const
         ap_fixed_base<_AP_W2,_AP_I2,_AP_S2>& op2) {
        ap_private<_AP_W+_AP_W2, _AP_S> OP1=op1.V;
        ap_private<_AP_W2,_AP_S2> OP2=op2.V;
        return OP1*OP2;
    }

    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2>
    INLINE typename RType<_AP_W2,_AP_I2,_AP_S2>::div
    operator / (const ap_fixed_base<_AP_W2,_AP_I2,_AP_S2,_AP_Q2,_AP_O2, _AP_N2>& op2) const {
        enum {F2 = _AP_W2-_AP_I2,
              _W1=AP_MAX(_AP_W + AP_MAX(F2, 0) + ((_AP_S2 && !_AP_S) ? 1 : 0), _AP_W2 + ((_AP_S && !_AP_S2) ? 1 : 0))};
        ap_private<_W1, _AP_S||_AP_S2> dividend = (ap_private<_W1, _AP_S>(V)) << AP_MAX(F2, 0);
        ap_private<_W1, _AP_S||_AP_S2> divisior = ap_private<_W1, _AP_S2>(op2.V);
        typename RType<_AP_W2, _AP_I2, _AP_S2>::div r;
        r.V = ((_AP_S||_AP_S2) ? dividend.sdiv(divisior): dividend.udiv(divisior));
        return r;
    }
#define OP_BIN_AF(Sym, Rty, Width, Sign, Fun)                                \
    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2> \
    INLINE typename RType<_AP_W2,_AP_I2,_AP_S2>::Rty  \
    operator Sym (const ap_fixed_base<_AP_W2,_AP_I2,_AP_S2,_AP_Q2,_AP_O2, _AP_N2>& op2) const \
    {                                                                        \
        enum {_AP_F=_AP_W-_AP_I, F2=_AP_W2-_AP_I2};                                \
        typename RType<_AP_W2,_AP_I2,_AP_S2>::Rty  r, lhs(*this), rhs(op2);        \
        r.V = lhs.V.Fun(rhs.V);                                                \
        return r;                                                                \
    }                                                                        \
    INLINE typename RType<_AP_W,_AP_I,_AP_S>::Rty                        \
    operator Sym (const ap_fixed_base& op2) const                        \
    {                                                                        \
        typename RType<_AP_W,_AP_I,_AP_S>::Rty  r;                                \
        r.V = V Sym op2.V;                                                        \
        return r;                                                                \
    }                                                                        \

    OP_BIN_AF(+, plus, plus_w, plus_s, Add)
    OP_BIN_AF(-, minus, minus_w, minus_s, Sub)

#define OP_LOGIC_BIN_AF(Sym, Rty, Width, Sign)                                \
        template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2> \
        INLINE typename RType<_AP_W2,_AP_I2,_AP_S2>::Rty                \
        operator Sym (const ap_fixed_base<_AP_W2,_AP_I2,_AP_S2,_AP_Q2,_AP_O2, _AP_N2>& op2) const \
    {                                                                        \
        typename RType<_AP_W2,_AP_I2,_AP_S2>::Rty r, lhs(*this), rhs(op2);  \
        r.V=lhs.V Sym rhs.V;                                                \
        return r;                                                                \
    }                                                                        \
    INLINE typename RType<_AP_W,_AP_I,_AP_S>::Rty                        \
    operator Sym (const ap_fixed_base& op2) const                        \
    {                                                                        \
        typename RType<_AP_W,_AP_I,_AP_S>::Rty  r; \
        r.V = V Sym op2.V;                                                        \
        return r;                                                                \
    }                                                                        \
    INLINE typename RType<_AP_W,_AP_I,_AP_S>::Rty  operator Sym(int op2) const \
    {                                                                        \
        return V Sym (op2<<(_AP_W - _AP_I));                                \
    }
    OP_LOGIC_BIN_AF(&, logic, logic_w, logic_s)
    OP_LOGIC_BIN_AF(|, logic, logic_w, logic_s)
    OP_LOGIC_BIN_AF(^, logic, logic_w, logic_s)

    ///Arithmic : assign
    //-------------------------------------------------------------------------
#define OP_ASSIGN_AF(Sym) \
    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2> \
    INLINE ap_fixed_base& operator Sym##= (const ap_fixed_base<_AP_W2,_AP_I2,_AP_S2,_AP_Q2,_AP_O2, _AP_N2>& op2) \
    { \
        *this=operator Sym (op2) ; \
        return *this; \
    }

    OP_ASSIGN_AF(+)
    OP_ASSIGN_AF(-)
    OP_ASSIGN_AF(&)
    OP_ASSIGN_AF(|)
    OP_ASSIGN_AF(^)
    OP_ASSIGN_AF(*)
    OP_ASSIGN_AF(/)
    
    ///Prefix increment, decrement
    //-------------------------------------------------------------------------
    INLINE ap_fixed_base& operator ++() {
        operator+=(ap_fixed_base<1,1,false>(1)); //SystemC's semantics
        return *this;
    }

    INLINE ap_fixed_base& operator --() {
        operator-=(ap_fixed_base<1,1,false>(1)); //SystemC's semantics
        return *this;
    }

    //Postfix increment, decrement
    //-------------------------------------------------------------------------
    INLINE const ap_fixed_base operator ++(int) {
        ap_fixed_base t(*this);
        operator++();
        return t;
    }

    INLINE const ap_fixed_base operator --(int) {
        ap_fixed_base t = *this;
        operator--();
        return t;
    }

    ///Unary arithmetic
    //-------------------------------------------------------------------------
    INLINE ap_fixed_base operator +() {return *this;}

    INLINE ap_fixed_base<_AP_W + 1, _AP_I + 1, true> operator -() const {
        ap_fixed_base<_AP_W + 1, _AP_I + 1, true> Tmp(*this);
        Tmp.V = - Tmp.V;
        return Tmp;
    }

    INLINE ap_fixed_base<_AP_W,_AP_I,true,_AP_Q,_AP_O, _AP_N> getNeg() {
        ap_fixed_base<_AP_W,_AP_I,true,_AP_Q,_AP_O, _AP_N> Tmp(*this);
        Tmp.V=-Tmp.V;
        return Tmp;
    }

    ///Not (!)
    //-------------------------------------------------------------------------
    INLINE bool operator !() const {
        return !V;
    }

    ///Bitwise complement
    //-------------------------------------------------------------------------
    INLINE ap_fixed_base<_AP_W, _AP_I, _AP_S> 
         operator ~() const {
        ap_fixed_base<_AP_W, _AP_I, _AP_S> res(*this);
        res.V.flip();
        return res;
    }

    ///Shift
    ///template argument as shift value
    template<int _AP_SHIFT>
    INLINE ap_fixed_base<_AP_W, _AP_I + _AP_SHIFT, _AP_S> lshift () const {
        ap_fixed_base<_AP_W, _AP_I + _AP_SHIFT, _AP_S> r;
        r.V = V;
        return r;
    }
    
    template<int _AP_SHIFT>
    INLINE ap_fixed_base<_AP_W, _AP_I - _AP_SHIFT, _AP_S> rshift () const {
        ap_fixed_base<_AP_W, _AP_I - _AP_SHIFT, _AP_S> r;
        r.V = V;
        return r;
    }
 
    //Because the return type is the type of the the first operand, shift assign
    //operators do not carry out any quantization or overflow
    //While systemc, shift assigns for sc_fixed/sc_ufixed will result in
    //quantization or overflow (depending on the mode of the first operand)
    //-------------------------------------------------------------------------
    INLINE ap_fixed_base operator << (int sh) const {
        ap_fixed_base r;
        bool isNeg=(sh&0x80000000) != 0;
        sh=isNeg?-sh:sh;
        bool shiftoverflow = sh >= _AP_W;
        bool NegSrc = V.isNegative();
        if(isNeg) {
            if(shiftoverflow)
                NegSrc?r.V.set():r.V.clear(); 
            else
                r.V=_AP_S?V.ashr(sh):V.lshr(sh);
        } else { 
            if(shiftoverflow)
                r.V.clear();
            else
                r.V=V<<sh;
        }
#ifdef __SC_COMPATIBLE__
        if (sh == 0) return r;
        if (isNeg == true && _AP_Q != AP_TRN) {
           bool qb = false;
           if (sh <= _AP_W) qb = V[sh - 1];
           bool rb  =  false;
           if (sh > 1 && sh <= _AP_W) 
                rb = (V << (_AP_W - sh + 1 )) != 0;
           else if (sh > _AP_W)
                rb = V != 0;
           r.quantization_adjust(qb, rb, NegSrc);
        } else if (isNeg == false && _AP_O != AP_WRAP) {
             bool allones, allzeros;
             if (sh < _AP_W ) {
                 ap_private<_AP_W, _AP_S > range1 = V.lshr(_AP_W - sh - 1);
                 allones = range1.isAllOnesValue();
                 allzeros = range1.isMinValue();            
             } else {
                 allones = false;
                 allzeros = V.isMinValue();
             }
             bool overflow = !allzeros && !NegSrc;
             bool underflow = !allones && NegSrc;
             if (_AP_O == AP_SAT_SYM && _AP_S)   
                  underflow |= NegSrc && (_AP_W > 1 ? r.V.isMinSignedValue():true);
             bool lD = false;
             if ( sh < _AP_W ) lD = V[_AP_W - sh - 1];
             r.overflow_adjust(underflow, overflow, lD, NegSrc);       
        }
#endif        
        return r;
    }

    template<int _AP_W2>
    INLINE ap_fixed_base operator<<(const ap_private<_AP_W2,true>& op2) const {
        int sh = op2.to_int();
        return operator << (sh);
    }

    INLINE ap_fixed_base operator << (unsigned int sh ) const {
        ap_fixed_base r;
        bool shiftoverflow = sh >= _AP_W;
        r.V = shiftoverflow ? ap_private<_AP_W, _AP_S >(0) : V << sh;
        if (sh == 0) return r;
#ifdef __SC_COMPATIBLE__
        bool NegSrc = V.isNegative();
        if (_AP_O != AP_WRAP) {
             bool allones, allzeros;
             if (sh < _AP_W ) {
                 ap_private<_AP_W, _AP_S > range1 = V.lshr(_AP_W - sh -1);
                 allones = range1.isAllOnesValue();
                 allzeros = range1.isMinValue();            
             } else {
                 allones = false;
                 allzeros = V.isMinValue();
             }
             bool overflow = !allzeros && !NegSrc;
             bool underflow = !allones && NegSrc;
             if (_AP_O == AP_SAT_SYM && _AP_S)   
                  underflow |= NegSrc && (_AP_W > 1 ? r.V.isMinSignedValue():true);
             bool lD = false;
             if ( sh < _AP_W ) lD = V[_AP_W - sh - 1];
             r.overflow_adjust(underflow, overflow, lD, NegSrc);       
        }
#endif
        return r;
    }

    template<int _AP_W2>
    INLINE ap_fixed_base operator << (const ap_private<_AP_W2,false>& op2) const {
        unsigned int sh = op2.to_uint();
        return operator << (sh);
    }

    INLINE ap_fixed_base operator >> (int sh) const {
        ap_fixed_base r;
        bool isNeg=(sh&0x80000000) != 0;
        bool NegSrc = V.isNegative();
        sh=isNeg?-sh:sh;
        bool shiftoverflow = sh >= _AP_W;
        if(isNeg && !shiftoverflow) r.V=V<<sh;
        else {
            if(shiftoverflow)
                NegSrc?r.V.set():r.V.clear();
            else
                r.V=_AP_S?V.ashr(sh):V.lshr(sh);
        }
#ifdef __SC_COMPATIBLE__
        if (sh == 0) return r;
        if (isNeg == false && _AP_Q != AP_TRN) {
           bool qb = false;
           if (sh <= _AP_W) qb = V[sh - 1];
           bool rb  =  false;
           if (sh > 1 && sh <= _AP_W) 
                rb = (V << (_AP_W - sh + 1 )) != 0;
           else if (sh > _AP_W)
                rb = V != 0;
           r.quantization_adjust(qb, rb, NegSrc);
        } else if (isNeg == true && _AP_O != AP_WRAP) {
             bool allones, allzeros;
             if (sh < _AP_W ) {
                 ap_private<_AP_W, _AP_S > range1 = V.lshr(_AP_W - sh - 1);
                 allones = range1.isAllOnesValue();
                 allzeros = range1.isMinValue();            
             } else {
                 allones = false;
                 allzeros = V.isMinValue();
             }
             bool overflow = !allzeros && !NegSrc;
             bool underflow = !allones && NegSrc;
             if (_AP_O == AP_SAT_SYM && _AP_S)   
                  underflow |= NegSrc && (_AP_W > 1 ? r.V.isMinSignedValue():true);
             bool lD = false;
             if ( sh < _AP_W ) lD = V[_AP_W - sh - 1];
             r.overflow_adjust(underflow, overflow, lD, NegSrc); 
        }      
#endif        
        return r;
    }

    template<int _AP_W2>
    INLINE ap_fixed_base operator >> (const ap_private<_AP_W2,true>& op2) const {
        int sh = op2.to_int();
        return operator >> (sh);
    }

    INLINE ap_fixed_base operator >> (unsigned int sh) const {
        ap_fixed_base r;
        bool NegSrc = V.isNegative();
        bool shiftoverflow = sh >= _AP_W;
        if(shiftoverflow)
            NegSrc?r.V.set():r.V.clear();
        else
            r.V=_AP_S?V.ashr(sh):V.lshr(sh);
#ifdef __SC_COMPATIBLE__
        if (sh == 0) return r;
        if (_AP_Q != AP_TRN) {
           bool qb = false;
           if (sh <= _AP_W) qb = V[sh - 1];
           bool rb  =  false;
           if (sh > 1 && sh <= _AP_W) 
                rb = (V << (_AP_W - sh + 1 )) != 0;
           else if (sh > _AP_W)
                rb = V != 0;
           r.quantization_adjust(qb, rb, NegSrc);
        }
#endif        
        return r;
    }

    template<int _AP_W2>
    INLINE ap_fixed_base operator >> (const ap_private<_AP_W2,false>& op2) const {
        unsigned int sh = op2.to_uint();
        return operator >> (sh);
    }

    ///shift assign
    //-------------------------------------------------------------------------
#define OP_AP_SHIFT_AP_ASSIGN_AF(Sym) \
    template<int _AP_W2, bool _AP_S2> \
    INLINE ap_fixed_base& operator Sym##=(const ap_private<_AP_W2,_AP_S2>& op2) \
    { \
        *this=operator Sym (op2); \
        return *this; \
    }

    OP_AP_SHIFT_AP_ASSIGN_AF(<<)
    OP_AP_SHIFT_AP_ASSIGN_AF(>>)

    ///Support shift(ap_fixed_base)
#define OP_AP_SHIFT_AF(Sym) \
    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2> \
    INLINE ap_fixed_base operator Sym (const ap_fixed_base<_AP_W2,_AP_I2,_AP_S2,_AP_Q2,_AP_O2, _AP_N2>& op2) const \
    { \
        return operator Sym (op2.to_ap_private()); \
    } \
    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2> \
    INLINE ap_fixed_base& operator Sym##= (const ap_fixed_base<_AP_W2,_AP_I2,_AP_S2,_AP_Q2,_AP_O2, _AP_N2>& op2) \
    { \
        *this=operator Sym (op2); \
        return *this; \
    }

    OP_AP_SHIFT_AF(<<)
    OP_AP_SHIFT_AF(>>)
        
    INLINE ap_fixed_base& operator >>= (unsigned int sh) {
        *this = operator >> (sh);
        return *this;
    }

    INLINE ap_fixed_base& operator <<= (unsigned int sh) {
        *this = operator << (sh);
        return *this;
    }

    INLINE ap_fixed_base& operator >>= (int sh) {
        *this = operator >> (sh);
        return *this;
    }

    INLINE ap_fixed_base& operator <<= (int sh) {
        *this = operator << (sh);
        return *this;
    }

    ///Comparisons
    //-------------------------------------------------------------------------
    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2> 
    INLINE bool operator == (const ap_fixed_base<_AP_W2,_AP_I2,_AP_S2,_AP_Q2,_AP_O2, _AP_N2>& op2) const {
        enum {_AP_F=_AP_W-_AP_I,F2=_AP_W2-_AP_I2, shAmt1 = AP_MAX(F2-_AP_F, 0), shAmt2 = AP_MAX(_AP_F-F2,0), _AP_W3 = (_AP_F==F2) ? AP_MAX(_AP_W,_AP_W2) : AP_MAX(_AP_W+shAmt1, _AP_W2+shAmt2)};
        ap_private<_AP_W3, _AP_S > OP1= ap_private<_AP_W3, _AP_S >(V)<<shAmt1;
        ap_private<_AP_W3,_AP_S2 > OP2=ap_private<_AP_W3,_AP_S2 >(op2.V)<<shAmt2;
        return OP1 == OP2;
    }

    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2> 
    INLINE bool operator != (const ap_fixed_base<_AP_W2,_AP_I2,_AP_S2,_AP_Q2,_AP_O2, _AP_N2>& op2) const {
        return !(*this==op2);
    }

    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2> 
    INLINE bool operator > (const ap_fixed_base<_AP_W2,_AP_I2,_AP_S2,_AP_Q2,_AP_O2, _AP_N2>& op2) const {
        enum {_AP_F=_AP_W-_AP_I,F2=_AP_W2-_AP_I2, shAmt1 = AP_MAX(F2-_AP_F, 0), shAmt2 = AP_MAX(_AP_F-F2,0), _AP_W3 = (_AP_F==F2) ? AP_MAX(_AP_W,_AP_W2) : AP_MAX(_AP_W+shAmt1, _AP_W2+shAmt2)};
        ap_private<_AP_W3, _AP_S > OP1= ap_private<_AP_W3, _AP_S >(V)<<shAmt1;
        ap_private<_AP_W3,_AP_S2 > OP2=ap_private<_AP_W3,_AP_S2 >(op2.V)<<shAmt2;
        if(_AP_S||_AP_S2)
            return OP1.sgt(OP2);
        else
            return OP1.ugt(OP2); 
    }

    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2> 
    INLINE bool operator <= (const ap_fixed_base<_AP_W2,_AP_I2,_AP_S2,_AP_Q2,_AP_O2, _AP_N2>& op2) const {
        return !(*this>op2);
    }

    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2> 
    INLINE bool operator < (const ap_fixed_base<_AP_W2,_AP_I2,_AP_S2,_AP_Q2,_AP_O2, _AP_N2>& op2) const {
        enum {_AP_F=_AP_W-_AP_I,F2=_AP_W2-_AP_I2, shAmt1 = AP_MAX(F2-_AP_F, 0), shAmt2 = AP_MAX(_AP_F-F2,0), _AP_W3 = (_AP_F==F2) ? AP_MAX(_AP_W,_AP_W2) : AP_MAX(_AP_W+shAmt1, _AP_W2+shAmt2)};
        ap_private<_AP_W3, _AP_S > OP1= ap_private<_AP_W3, _AP_S >(V)<<shAmt1;
        ap_private<_AP_W3,_AP_S2 > OP2=ap_private<_AP_W3,_AP_S2 >(op2.V)<<shAmt2;
        if(_AP_S||_AP_S2) 
            return OP1.slt(OP2);
        else
            return OP1.ult(OP2);   
    }

    template<int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2> 
    INLINE bool operator >= (const ap_fixed_base<_AP_W2,_AP_I2,_AP_S2,_AP_Q2,_AP_O2, _AP_N2>& op2) const {
        return !(*this<op2);
    }

#define DOUBLE_CMP_AF(Sym) \
    INLINE bool operator Sym (double d) const { \
        return to_double() Sym d; \
    }

    DOUBLE_CMP_AF(==)
    DOUBLE_CMP_AF(!=)
    DOUBLE_CMP_AF(>)
    DOUBLE_CMP_AF(>=)
    DOUBLE_CMP_AF(<)
    DOUBLE_CMP_AF(<=)

    // Bit and Slice Select
    INLINE af_bit_ref<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N> operator [] (unsigned int index) {
        return af_bit_ref<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>(this, index);
    }

    INLINE af_bit_ref<_AP_W, _AP_I,_AP_S,_AP_Q,_AP_O, _AP_N> bit(unsigned int index) {
        return af_bit_ref<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>(this, index);
    }

    template<int _AP_W2, bool _AP_S2>
    INLINE af_bit_ref<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N> bit (const ap_private<_AP_W2,_AP_S2>& index) {
        return af_bit_ref<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>(this, index.to_int());
    }

    INLINE bool bit (unsigned int index) const {
        if (index >= _AP_W)
	    fprintf(stderr, "Warning! Index of bit vector (%d) out of range (%d).\n", index, _AP_W);
        //assert(index < _AP_W && "Attempting to read bit beyond MSB");
        return V[index];
    }

    INLINE bool operator [] (unsigned int index) const {
        if (index >= _AP_W)
	    fprintf(stderr, "Warning! Index of bit vector (%d) out of range (%d).\n", index, _AP_W);
        //assert(index < _AP_W && "Attempting to read bit beyond MSB");
        return V[index];
    }

    template<int _AP_W2, bool _AP_S2>
    INLINE bool bit (const ap_private<_AP_W2, _AP_S2>& index) const {
        if (index >= _AP_W)
	    fprintf(stderr, "Warning! Index of bit vector (%d) out of range (%d).\n", index.to_int(), _AP_W);
        //assert(index < _AP_W && "Attempting to read bit beyond MSB");
        return V[index.to_uint()];
    }

    template<int _AP_W2, bool _AP_S2>
    INLINE bool operator [] (const ap_private<_AP_W2, _AP_S2>& index) const {
        if (index >= _AP_W)
	    fprintf(stderr, "Warning! Index of bit vector (%d) out of range (%d).\n", index.to_int(), _AP_W);
        //assert(index < _AP_W && "Attempting to read bit beyond MSB");
        return V[index.to_uint()];
    }

    INLINE af_bit_ref<_AP_W, _AP_I,_AP_S,_AP_Q,_AP_O, _AP_N> get_bit(int index) {
        return af_bit_ref<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>(this, index + _AP_W - _AP_I);
    }

    template<int _AP_W2>
    INLINE af_bit_ref<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N> get_bit (const ap_private<_AP_W2, true>& index) {
        return af_bit_ref<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>(this, index.to_int() + _AP_W - _AP_I);
    }

    INLINE bool get_bit (int index) const {
        if (index  < _AP_I - _AP_W ) 
	    fprintf(stderr, "Warning! Index of bit vector  (%d) cannot be negative.\n", index);
        if (index >= _AP_I)
	    fprintf(stderr, "Warning! Index of bit vector (%d) out of range (%d).\n", index, _AP_W);
        //assert(index >= _AP_I - _AP_W && "Attempting to read bit with negative index");
       // assert(index < _AP_I && "Attempting to read bit beyond MSB");
        return V[index + _AP_W - _AP_I];
    }

    template<int _AP_W2>
    INLINE bool get_bit (const ap_private<_AP_W2, true>& index) const {
        if (index  < _AP_I - _AP_W ) 
	    fprintf(stderr, "Warning! Index of bit vector  (%d) cannot be negative.\n", index.to_int());
        if (index >= _AP_I)
	    fprintf(stderr, "Warning! Index of bit vector (%d) out of range (%d).\n", index.to_int(), _AP_W);
        //assert(index >= _AP_I - _AP_W && "Attempting to read bit with negative index");
        //assert(index < _AP_I && "Attempting to read bit beyond MSB");
        return V[index.to_int() + _AP_W - _AP_I];
    }

    INLINE af_range_ref<_AP_W,_AP_I,_AP_S, _AP_Q, _AP_O, _AP_N>
    range(int Hi, int Lo) {
        return af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>(this, Hi, Lo);
    }

    INLINE af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>
    operator () (int Hi, int Lo) {
        return af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>(this, Hi, Lo);
    }

    INLINE af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>
    range(int Hi, int Lo) const {
        return af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>(const_cast<ap_fixed_base*>(this), Hi, Lo);
    }

    INLINE af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>
    operator () (int Hi, int Lo) const {
        return this->range(Hi, Lo);
    }

    template<int _AP_W2, bool _AP_S2, int _AP_W3, bool _AP_S3>
    INLINE af_range_ref<_AP_W,_AP_I,_AP_S, _AP_Q, _AP_O, _AP_N>
    range(const ap_private<_AP_W2, _AP_S2> &HiIdx, 
          const ap_private<_AP_W3, _AP_S3> &LoIdx) {
        int Hi = HiIdx.to_int();
        int Lo = LoIdx.to_int();
        return af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>(this, Hi, Lo);
    }

    template<int _AP_W2, bool _AP_S2, int _AP_W3, bool _AP_S3>
    INLINE af_range_ref<_AP_W,_AP_I,_AP_S, _AP_Q, _AP_O, _AP_N>
    operator () (const ap_private<_AP_W2, _AP_S2> &HiIdx, 
                 const ap_private<_AP_W3, _AP_S3> &LoIdx) {
        int Hi = HiIdx.to_int();
        int Lo = LoIdx.to_int();
        return af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>(this, Hi, Lo);
    }

    template<int _AP_W2, bool _AP_S2, int _AP_W3, bool _AP_S3>
    INLINE af_range_ref<_AP_W,_AP_I,_AP_S, _AP_Q, _AP_O, _AP_N>
    range(const ap_private<_AP_W2, _AP_S2> &HiIdx, 
          const ap_private<_AP_W3, _AP_S3> &LoIdx) const {
        int Hi = HiIdx.to_int();
        int Lo = LoIdx.to_int();
        return af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>(const_cast<
               ap_fixed_base<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>*>(this), 
               Hi, Lo);
    }

    template<int _AP_W2, bool _AP_S2, int _AP_W3, bool _AP_S3>
    INLINE af_range_ref<_AP_W,_AP_I,_AP_S, _AP_Q, _AP_O, _AP_N>
    operator () (const ap_private<_AP_W2, _AP_S2> &HiIdx, 
                 const ap_private<_AP_W3, _AP_S3> &LoIdx) const {
        int Hi = HiIdx.to_int();
        int Lo = LoIdx.to_int();
        return this->range(Hi, Lo);
    }

    INLINE af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>
    range() {
        return this->range(_AP_W - 1, 0);
    }

    INLINE af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>
    range() const {
        return this->range(_AP_W - 1, 0);
    }
   
    INLINE bool is_zero () const {
        return V.isMinValue();
    }

    INLINE bool is_neg () const {
        if (V.isNegative())
            return true;
        return false;
    }

    INLINE int wl () const {
        return _AP_W;
    }

    INLINE int iwl () const {
        return _AP_I;
    } 

    INLINE ap_q_mode q_mode () const {
        return _AP_Q;
    }

    INLINE ap_o_mode o_mode () const {
        return _AP_O;
    }

    INLINE int n_bits () const {
        return 0;
    } 
   
  //private:
public:
    ap_private<_AP_W, _AP_S> V;
    static const int width = _AP_W;
    static const int iwidth = _AP_I;
    static const ap_q_mode qmode = _AP_Q;
    static const ap_o_mode omode = _AP_O;

};

// print a string representation of this number in the given radix.
// Radix support is 2, 8, 10, or 16.
// The result will include a prefix indicating the radix, except for decimal, where no prefix is needed
// The default is to output a signed representation of signed numbers, or an unsigned representation
// of unsigned numbers.  For non-decimal formats, this can be changed by the 'sign' argument.
template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
std::string ap_fixed_base<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>::to_string(
                                            uint8_t radix, bool sign) const {
    std::string str;
    str.clear();
    char step = 0;
    bool isNeg = sign && V.isNegative();

    // Extend to take care of the -MAX case.
    ap_fixed_base<_AP_W+1, _AP_I+1> tmp(*this);
    if (isNeg) {
        tmp = -tmp;
        str += '-';
    }
    std::string prefix;
    switch (radix) {
        case 2  : prefix = "0b"; step = 1; break;
        case 8  : prefix = "0o"; step = 3; break;
        case 16 : prefix = "0x"; step = 4; break;
        default : break;
    }

    if (_AP_I > 0) {
        // Note we drop the quantization and rounding flags here.  The
        // integer part is always in range, and the fractional part we
        // want to drop.  Also, the number is always positive, because
        // of the absolute value above.
        ap_fixed_base<AP_MAX(_AP_I+1, 1), AP_MAX(_AP_I+1, 1), false> int_part = tmp;
        str += int_part.to_ap_private().to_string(radix, false);
    } else {
        str += prefix;
        str += '0';
    }
    ap_fixed_base<AP_MAX(_AP_W - _AP_I, 1), 0, false> frac_part = tmp;

    if (radix == 10) {
        if (frac_part != 0) {
            str += ".";
            while (frac_part != 0) {
                char digit = (char)(frac_part * radix).to_ap_private();
                str += static_cast<char>(digit + '0');
                frac_part *= radix;
            }
        }
    } else {
        if (frac_part != 0) {
            str += ".";
            for (signed i = _AP_W - _AP_I - 1; i >= 0; i -= step) {
                char digit = (char)(frac_part.range(i, AP_MAX(0, i - step + 1)));
                // If we have a partial bit pattern at the end, then we need
                // to put it in the high-order bits of 'digit'.
                int offset = AP_MIN(0, i - step + 1);
                digit <<= -offset;
                str += digit < 10 ? static_cast<char>(digit + '0') :
                    static_cast<char>(digit - 10 + 'a');
            }
            str += "p0"; // C99 Hex constants are required to have an exponent.
        }
    }
    return str;
}

template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
INLINE void b_not(ap_fixed_base<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>& ret, 
            const ap_fixed_base<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>& op) {
    ret.V = op.V;
    ret.V.flip();
}

template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
INLINE void b_and(ap_fixed_base<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>& ret, 
            const ap_fixed_base<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>& op1,
            const ap_fixed_base<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>& op2) {
    ret.V =  op1.V & op2.V;
}

template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
INLINE void b_or(ap_fixed_base<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>& ret, 
            const ap_fixed_base<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>& op1,
            const ap_fixed_base<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>& op2) {
    ret.V =  op1.V | op2.V;
}

template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
INLINE void b_xor(ap_fixed_base<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>& ret, 
            const ap_fixed_base<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>& op1,
            const ap_fixed_base<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>& op2) {
    ret.V =  op1.V ^ op2.V;
}

template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O,
         int _AP_N, int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
          ap_o_mode _AP_O2, int _AP_N2>
INLINE void neg(ap_fixed_base<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>& ret, 
            const ap_fixed_base<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op) {
    ap_fixed_base<_AP_W2+!_AP_S2, _AP_I2+!_AP_S2, true, _AP_Q2, _AP_O2, _AP_N2> Tmp;
    Tmp.V = - op.V;
    ret = Tmp;
}

template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O,
         int _AP_N>
INLINE void neg(ap_fixed_base<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>& ret, 
            const ap_fixed_base<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>& op) {
    ret.V = -op.V;
}

template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O,
         int _AP_N, int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, 
         ap_o_mode _AP_O2, int _AP_N2>
INLINE void lshift(ap_fixed_base<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>& ret, 
            const ap_fixed_base<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op,
            int i) {
    ap_fixed_base<_AP_W2 - _AP_I2 + AP_MAX(_AP_I, _AP_I2), AP_MAX(_AP_I, _AP_I2), _AP_S2, _AP_Q2, _AP_O2, _AP_N2> Tmp;
    Tmp = op;
    Tmp.V <<= i;
    ret = Tmp; 
} 

template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O,
         int _AP_N>
INLINE void lshift(ap_fixed_base<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>& ret, 
            const ap_fixed_base<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>& op,
            int i) {
    ret.V = op.V << i;
} 

template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O,
         int _AP_N, int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, 
         ap_o_mode _AP_O2, int _AP_N2>
INLINE void rshift(ap_fixed_base<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>& ret, 
            const ap_fixed_base<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op,
            int i) {
    ap_fixed_base<_AP_I2 + AP_MAX(_AP_W - _AP_I, _AP_W2 - _AP_I2), _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> Tmp;
    Tmp = op;
    Tmp.V = _AP_S2 ? Tmp.V.ashr(i): Tmp.V.lshr(i);
    ret = Tmp; 
}

template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O,
         int _AP_N>
INLINE void rshift(ap_fixed_base<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>& ret, 
            const ap_fixed_base<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N>& op,
            int i) {
    ret.V = _AP_S ? op.V.ashr(i): op.V.lshr(i);
}

#define AF_CTOR_SPEC_BASE(_AP_W,_AP_S,C_TYPE) \
  template<> INLINE ap_fixed_base<_AP_W,_AP_W,_AP_S,AP_TRN,AP_WRAP>::ap_fixed_base(C_TYPE i_op):V(i_op) \
    { \
  }

#define AF_CTOR_SPEC(__W,C_TYPE) \
    AF_CTOR_SPEC_BASE(__W,true,C_TYPE) \
    AF_CTOR_SPEC_BASE(__W,false,C_TYPE) 

AF_CTOR_SPEC(1,bool)
AF_CTOR_SPEC(8, signed char)
AF_CTOR_SPEC(8, unsigned char)
AF_CTOR_SPEC(16, signed short)
AF_CTOR_SPEC(16, unsigned short)
AF_CTOR_SPEC(32, signed int)
AF_CTOR_SPEC(32, unsigned int)
AF_CTOR_SPEC(64, ap_slong)
AF_CTOR_SPEC(64, ap_ulong)
#undef AF_CTOR_SPEC_BASE
#undef AF_CTOR_SPEC

INLINE std::string scientificFormat(std::string& input) {
    if (input.length() == 0)
        return input;

    size_t decPosition = input.find('.');
    if (decPosition == std::string::npos)
        decPosition = input.length();

    size_t firstNonZeroPos = 0;
    for (; input[firstNonZeroPos] > '9' ||
           input[firstNonZeroPos] < '1';
         firstNonZeroPos++);

    int exp;
    if (firstNonZeroPos > decPosition)
        exp = decPosition - firstNonZeroPos;
    else
        exp = decPosition - firstNonZeroPos - 1;
    std::string expString = "";
    if (exp == 0);
    else if (exp < 0) {
        expString += "e-";
        exp = -exp;
    } else
        expString += "e+";

    if (exp < 10 && exp > 0) {
        expString += '0';
        expString += (char)('0' + exp);
    } else if (exp != 0) {
        std::string tmp;

        std::ostringstream oss;
        oss<<exp;

        tmp=oss.str();
        expString += tmp;
    }

    int lastNonZeroPos = (int) (input.length() - 1);
    for (; lastNonZeroPos >= 0; --lastNonZeroPos)
        if (input[lastNonZeroPos] <= '9' && input[lastNonZeroPos] > '0')
            break;
        
    std::string ans = "";
    ans += input[firstNonZeroPos];
    if (firstNonZeroPos != (size_t)lastNonZeroPos) {
        ans += '.';
        for (int i=firstNonZeroPos+1; i <= lastNonZeroPos; i++)
            if (input[i] != '.')
                ans += input[i];
    }

    ans += expString;
    return ans;
}

INLINE std::string reduceToPrecision(std::string& input, int precision) {

    bool isZero = true;
    size_t inputLen = input.length();
    for (size_t i=0; i<inputLen && isZero; i++)
        if (input[i] != '.' && input[i] != '0')
            isZero = false;
    if (isZero)
        return "0";

    //Find the first valid number, skip '-'
    int FirstNonZeroPos = 0;
    int LastNonZeroPos = (int) inputLen - 1;
    int truncBitPosition = 0;
    size_t decPosition = input.find('.');
    for (; input[FirstNonZeroPos] < '1' || input[FirstNonZeroPos] > '9';
         FirstNonZeroPos++);

    for (; input[LastNonZeroPos] < '1' || input[LastNonZeroPos] > '9';
         LastNonZeroPos--);

    if (decPosition == std::string::npos)
        decPosition = inputLen;
    //Count the valid number, to decide whether we need to truncate
    if ((int) decPosition > LastNonZeroPos) {
        if (LastNonZeroPos - FirstNonZeroPos + 1 <= precision)
            return input;
        truncBitPosition = FirstNonZeroPos + precision;
    } else if ((int) decPosition < FirstNonZeroPos) { //This is pure decimal
        if (LastNonZeroPos - FirstNonZeroPos + 1 <= precision) {
            if (FirstNonZeroPos - decPosition - 1 < 4) {
                return input;
            } else {
                if (input[0] == '-') {
                    std::string tmp = input.substr(1, inputLen-1);
                    return std::string("-") +
                           scientificFormat(tmp);
                } else
                    return scientificFormat(input);
            }
        }
        truncBitPosition = FirstNonZeroPos + precision;
    } else {
        if (LastNonZeroPos - FirstNonZeroPos <= precision)
            return input;
        truncBitPosition = FirstNonZeroPos + precision + 1;
    }

    //duplicate the input string, we want to add "0" before the valid numbers
    // This is easy for quantization, since we may change 9999 to 10000
    std::string ans = "";
    std::string dupInput = "0";
    if (input[0] == '-') {
        ans += '-';
        dupInput += input.substr(1, inputLen-1);
    } else {
        dupInput += input.substr(0, inputLen);
        ++truncBitPosition;
    }

    //Add 'carry' after truncation, if necessary
    bool carry = dupInput[truncBitPosition] > '4';
    for (int i = truncBitPosition-1; i >=0 && carry; i--) {
        if (dupInput[i] == '.')
            continue;
        if (dupInput[i] == '9')
            dupInput[i] = '0';
        else {
            ++dupInput[i];
            carry = false;
        }
    }

    //bits outside precision range should be set to 0
    if (dupInput[0] == '1')
        FirstNonZeroPos = 0;
    else {
        FirstNonZeroPos = 0;
        while (dupInput[FirstNonZeroPos] < '1' ||
               dupInput[FirstNonZeroPos] > '9')
            ++FirstNonZeroPos;
    }

    unsigned it = FirstNonZeroPos;
    int NValidNumber = 0;
    while (it < dupInput.length()) {
        if (dupInput[it] == '.') {
            ++it;
            continue;
        }
        ++NValidNumber;
        if (NValidNumber > precision)
            dupInput[it] = '0';
        ++it;
    }

    //Here we wanted to adjust the truncate position and the value
    decPosition = dupInput.find('.');
    if (decPosition == std::string::npos) //When this is integer
        truncBitPosition = (int) dupInput.length();
    else 
        for (truncBitPosition = (int) (dupInput.length()-1);
             truncBitPosition >=0;
             --truncBitPosition) {
            if (dupInput[truncBitPosition] == '.')
                break;
            if (dupInput[truncBitPosition] != '0') {
                truncBitPosition++;
                break;
            }
        }
    
    if (dupInput[0] == '1')
        dupInput = dupInput.substr(0,
                                   truncBitPosition);
    else
        dupInput = dupInput.substr(1, truncBitPosition-1);

    decPosition = dupInput.find('.');
    if (decPosition != std::string::npos) {
        size_t it = 0;
        for (it = decPosition+1; dupInput[it]=='0'; it++);
        if (it - decPosition - 1 < 4) {
            ans += dupInput;
            return ans;
        } else {
            ans += scientificFormat(dupInput);
            return ans;
        }
    } else if ((int)(dupInput.length()) <= precision) {
        ans += dupInput;
        return ans;
    }

    ans += scientificFormat(dupInput);
    return ans;
}

///Output streaming
//-----------------------------------------------------------------------------
template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
INLINE std::ostream&
operator <<(std::ostream& out, const ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& x) {
    unsigned width = out.width();
    unsigned precision = out.precision();
    char fill = out.fill();
    std::string str=x.to_string(10,_AP_S);
    str = reduceToPrecision(str, precision);
    if (width > str.length()) {
        char *padding = (char*)malloc((width - str.length() + 1)*sizeof(char));
        for (unsigned i=0; i<width - str.length(); ++i)
            padding[i] = fill;
        padding[width - str.length()] = 0;
        str = std::string(padding) + str;
        free(padding);
    }
    out<<str;
    return out;
}

///Input streaming
//-----------------------------------------------------------------------------
template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
INLINE std::istream&
operator >> (std::istream& os, ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& x) {
     double d;
     os >> d;
     x = ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>(d);
     return os;
}

template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
INLINE void print(const ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& x) {
    ap_private<_AP_W,_AP_S> data=x.V;
    if(_AP_I>0) {
        const ap_private<_AP_I,_AP_S> p1=data>>(_AP_W-_AP_I);
        print(p1);
        
    } else
        printf("0");
    printf(".");
    if(_AP_I<_AP_W) {
        const ap_private<_AP_W-_AP_I,false> p2=data;
        print(p2,false);
    }
}

///Operators mixing Integers with ap_fixed_base
//-----------------------------------------------------------------------------
#if 1
#define AF_BIN_OP_WITH_INT_SF(BIN_OP,C_TYPE,_AP_W2,_AP_S2,RTYPE)        \
  template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N> \
  INLINE typename ap_fixed_base<_AP_W,_AP_I,_AP_S>::template RType<_AP_W2,_AP_W2,_AP_S2>::RTYPE \
  operator BIN_OP (const ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& op, C_TYPE i_op) \
  {                                                                        \
    return op.operator BIN_OP(ap_private<_AP_W2,_AP_S2>(i_op));        \
  }
#define AF_BIN_OP_WITH_INT(BIN_OP, C_TYPE, _AP_W2,_AP_S2,RTYPE)                \
  template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N> \
  INLINE typename ap_fixed_base<_AP_W,_AP_I,_AP_S>::template RType<_AP_W2,_AP_W2,_AP_S2>::RTYPE \
  operator BIN_OP (const ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& op, C_TYPE i_op) \
  {                                                                        \
    return op.operator BIN_OP (ap_fixed_base<_AP_W2,_AP_W2,_AP_S2>(i_op)); \
  }                                                                        \
                                                                        \
  template<int _AP_W, int _AP_I, bool _AP_S,ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N > \
  INLINE typename ap_fixed_base<_AP_W,_AP_I,_AP_S>::template RType<_AP_W2,_AP_W2,_AP_S2>::RTYPE \
  operator BIN_OP (C_TYPE i_op, const ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& op) \
  {                                                                        \
    return ap_fixed_base<_AP_W2,_AP_W2,_AP_S2>(i_op).operator BIN_OP (op); \
  }

#else
#define AF_BIN_OP_WITH_INT_SF(BIN_OP,C_TYPE,_AP_W2,_AP_S2,RTYPE)        \
  template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N> \
  INLINE typename ap_fixed_base<_AP_W,_AP_I,_AP_S>::template RType<_AP_W2,_AP_W2,_AP_S2>::RTYPE \
  operator BIN_OP (const ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& op, C_TYPE i_op) \
  {                                                                        \
    return op BIN_OP (i_op);                                        \
  }
#define AF_BIN_OP_WITH_INT(BIN_OP, C_TYPE, _AP_W2,_AP_S2,RTYPE)                \
  template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N> \
  INLINE typename ap_fixed_base<_AP_W,_AP_I,_AP_S>::template RType<_AP_W2,_AP_W2,_AP_S2>::RTYPE \
  operator BIN_OP (const ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& op, C_TYPE i_op) \
  {                                                                        \
    return op.V BIN_OP (i_op<<(_AP_W-_AP_I));                        \
  }                                                                        \
                                                                          \
                                                                          \
  template<int _AP_W, int _AP_I, bool _AP_S,ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N > \
  INLINE typename ap_fixed_base<_AP_W,_AP_I,_AP_S>::template RType<_AP_W2,_AP_W2,_AP_S2>::RTYPE \
  operator BIN_OP (C_TYPE i_op, const ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& op) \
  {                                                                        \
    return ap_fixed_base<_AP_W2,_AP_W2,_AP_S2>(i_op).operator BIN_OP (op);                        \
  }

#endif
#if 1
#define AF_REL_OP_WITH_INT(REL_OP, C_TYPE, _AP_W2,_AP_S2)                \
  template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N> \
    INLINE bool operator REL_OP (const ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& op, C_TYPE i_op) \
    { \
        return op.operator REL_OP (ap_fixed_base<_AP_W2,_AP_W2,_AP_S2>(i_op)); \
    } \
    \
    \
    template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N> \
    INLINE bool operator REL_OP (C_TYPE i_op, const ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& op) \
    { \
        return ap_fixed_base<_AP_W2,_AP_W2,_AP_S2>(i_op).operator REL_OP (op); \
    }
#else
#define AF_REL_OP_WITH_INT(REL_OP, C_TYPE, _AP_W2,_AP_S2) \
    template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N> \
    INLINE bool operator REL_OP (const ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& op, C_TYPE i_op) \
    { \
      return op.V.operator REL_OP (i_op<<(_AP_W-_AP_I));        \
    } \
    \
    \
    template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N> \
    INLINE bool operator REL_OP (C_TYPE i_op, const ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& op) \
    { \
      return (i_op<<(_AP_W-_AP_I)) REL_OP (op.V.get_VAL());        \
    }
#endif
#if 1
#define AF_ASSIGN_OP_WITH_INT(ASSIGN_OP, C_TYPE, _AP_W2, _AP_S2) \
  template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N> \
  INLINE ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& operator ASSIGN_OP ( ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& op, C_TYPE i_op) { \
    return op.operator ASSIGN_OP (ap_fixed_base<_AP_W2,_AP_W2,_AP_S2>(i_op)); \
  }
#define AF_ASSIGN_OP_WITH_INT_SF(ASSIGN_OP, C_TYPE, _AP_W2, _AP_S2) \
  template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N> \
  INLINE ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& operator ASSIGN_OP ( ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& op, C_TYPE i_op) { \
    return op.operator ASSIGN_OP (ap_private<_AP_W2,_AP_S2>(i_op)); \
  }
#else
#define AF_ASSIGN_OP_WITH_INT(ASSIGN_OP, C_TYPE, _AP_W2, _AP_S2) \
  template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N> \
  INLINE ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& operator ASSIGN_OP ( ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& op, C_TYPE i_op) { \
    return op.V.operator ASSIGN_OP (i_op);                                \
  }
#define AF_ASSIGN_OP_WITH_INT_SF(ASSIGN_OP, C_TYPE, _AP_W2, _AP_S2) \
  template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N> \
  INLINE ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& operator ASSIGN_OP ( ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& op, C_TYPE i_op) { \
    return op.V.operator ASSIGN_OP (i_op); \
  }
#endif

#define AF_OPS_WITH_INT(C_TYPE, WI, SI) \
  AF_BIN_OP_WITH_INT(+, C_TYPE, WI, SI, plus) \
  AF_BIN_OP_WITH_INT(-, C_TYPE, WI, SI, minus) \
  AF_BIN_OP_WITH_INT(*, C_TYPE, WI, SI, mult) \
  AF_BIN_OP_WITH_INT(/, C_TYPE, WI, SI, div) \
  AF_BIN_OP_WITH_INT_SF(>>, C_TYPE, WI, SI, arg1) \
  AF_BIN_OP_WITH_INT_SF(<<, C_TYPE, WI, SI, arg1) \
  AF_BIN_OP_WITH_INT(&, C_TYPE, WI, SI, logic) \
  AF_BIN_OP_WITH_INT(|, C_TYPE, WI, SI, logic) \
  AF_BIN_OP_WITH_INT(^, C_TYPE, WI, SI, logic) \
  \
  AF_REL_OP_WITH_INT(==, C_TYPE, WI, SI) \
  AF_REL_OP_WITH_INT(!=, C_TYPE, WI, SI) \
  AF_REL_OP_WITH_INT(>, C_TYPE, WI, SI) \
  AF_REL_OP_WITH_INT(>=, C_TYPE, WI, SI) \
  AF_REL_OP_WITH_INT(<, C_TYPE, WI, SI) \
  AF_REL_OP_WITH_INT(<=, C_TYPE, WI, SI) \
  \
  AF_ASSIGN_OP_WITH_INT(+=, C_TYPE, WI, SI) \
  AF_ASSIGN_OP_WITH_INT(-=, C_TYPE, WI, SI) \
  AF_ASSIGN_OP_WITH_INT(*=, C_TYPE, WI, SI) \
  AF_ASSIGN_OP_WITH_INT(/=, C_TYPE, WI, SI) \
  AF_ASSIGN_OP_WITH_INT_SF(>>=, C_TYPE, WI, SI) \
  AF_ASSIGN_OP_WITH_INT_SF(<<=, C_TYPE, WI, SI) \
  AF_ASSIGN_OP_WITH_INT(&=, C_TYPE, WI, SI) \
  AF_ASSIGN_OP_WITH_INT(|=, C_TYPE, WI, SI) \
  AF_ASSIGN_OP_WITH_INT(^=, C_TYPE, WI, SI)

AF_OPS_WITH_INT(bool, 1, false)
AF_OPS_WITH_INT(char, 8, true)
AF_OPS_WITH_INT(signed char, 8, true)
AF_OPS_WITH_INT(unsigned char, 8, false)
AF_OPS_WITH_INT(short, 16, true)
AF_OPS_WITH_INT(unsigned short, 16, false)
AF_OPS_WITH_INT(int, 32, true)
AF_OPS_WITH_INT(unsigned int, 32, false)
# if defined __x86_64__
AF_OPS_WITH_INT(long, 64, true)
AF_OPS_WITH_INT(unsigned long, 64, false)
# else
AF_OPS_WITH_INT(long, 32, true)
AF_OPS_WITH_INT(unsigned long, 32, false)
# endif
AF_OPS_WITH_INT(ap_slong, 64, true)
AF_OPS_WITH_INT(ap_ulong, 64, false)

#define AF_BIN_OP_WITH_AP_INT(BIN_OP, RTYPE) \
  template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O,int _AP_N, int _AP_W2, bool _AP_S2> \
  INLINE typename ap_fixed_base<_AP_W2,_AP_W2,_AP_S2>::template RType<_AP_W,_AP_I,_AP_S>::RTYPE \
  operator BIN_OP ( const ap_private<_AP_W2,_AP_S2>& i_op, const ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& op) { \
    return ap_fixed_base<_AP_W2,_AP_W2,_AP_S2>(i_op).operator BIN_OP (op); \
  } \
  template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O,int _AP_N, int _AP_W2, bool _AP_S2> \
  INLINE typename ap_fixed_base<_AP_W,_AP_I,_AP_S>::template RType<_AP_W2,_AP_W2,_AP_S2>::RTYPE \
  operator BIN_OP ( const ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& op, const ap_private<_AP_W2,_AP_S2>& i_op) { \
    return op.operator BIN_OP (ap_fixed_base<_AP_W2,_AP_W2,_AP_S2>(i_op)); \
  }

#define AF_REL_OP_WITH_AP_INT(REL_OP) \
  template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O,int _AP_N, int _AP_W2, bool _AP_S2> \
  INLINE bool operator REL_OP ( const ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& op, const ap_private<_AP_W2,_AP_S2>& i_op) { \
    return op.operator REL_OP ( ap_fixed_base<_AP_W2,_AP_W2,_AP_S2>(i_op)); \
  } \
  template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O,int _AP_N, int _AP_W2, bool _AP_S2> \
  INLINE bool operator REL_OP ( const ap_private<_AP_W2,_AP_S2>& i_op, const ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& op) { \
    return ap_fixed_base<_AP_W2,_AP_W2,_AP_S2>(i_op).operator REL_OP (op); \
  }

#define AF_ASSIGN_OP_WITH_AP_INT(ASSIGN_OP) \
  template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O,int _AP_N, int _AP_W2, bool _AP_S2> \
  INLINE ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& operator ASSIGN_OP ( ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& op, const ap_private<_AP_W2,_AP_S2>& i_op) { \
    return op.operator ASSIGN_OP (ap_fixed_base<_AP_W2,_AP_W2,_AP_S2>(i_op)); \
  } \
  template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O,int _AP_N, int _AP_W2, bool _AP_S2> \
  INLINE ap_private<_AP_W2,_AP_S2>& operator ASSIGN_OP ( ap_private<_AP_W2,_AP_S2>& i_op, const ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& op) { \
    return i_op.operator ASSIGN_OP (op.to_ap_private()); \
  }

AF_BIN_OP_WITH_AP_INT(+, plus)
AF_BIN_OP_WITH_AP_INT(-, minus)
AF_BIN_OP_WITH_AP_INT(*, mult)
AF_BIN_OP_WITH_AP_INT(/, div)
AF_BIN_OP_WITH_AP_INT(&, logic)
AF_BIN_OP_WITH_AP_INT(|, logic)
AF_BIN_OP_WITH_AP_INT(^, logic)

AF_REL_OP_WITH_AP_INT(==)
AF_REL_OP_WITH_AP_INT(!=)
AF_REL_OP_WITH_AP_INT(>)
AF_REL_OP_WITH_AP_INT(>=)
AF_REL_OP_WITH_AP_INT(<)
AF_REL_OP_WITH_AP_INT(<=)

AF_ASSIGN_OP_WITH_AP_INT(+=)
AF_ASSIGN_OP_WITH_AP_INT(-=)
AF_ASSIGN_OP_WITH_AP_INT(*=)
AF_ASSIGN_OP_WITH_AP_INT(/=)
AF_ASSIGN_OP_WITH_AP_INT(&=)
AF_ASSIGN_OP_WITH_AP_INT(|=)
AF_ASSIGN_OP_WITH_AP_INT(^=)

#define AF_REF_REL_OP_MIX_INT(REL_OP, C_TYPE, _AP_W2, _AP_S2)  \
template<int _AP_W, int _AP_I, bool _AP_S, \
         ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N> \
  INLINE bool operator REL_OP ( const af_range_ref<_AP_W,_AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> &op, C_TYPE op2) {  \
    return (ap_private<_AP_W, false>(op)).operator REL_OP (ap_private<_AP_W2,_AP_S2>(op2));  \
  }  \
template<int _AP_W, int _AP_I, bool _AP_S, \
         ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N> \
  INLINE bool operator REL_OP ( C_TYPE op2, const af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> &op) {  \
    return ap_private<_AP_W2,_AP_S2>(op2).operator REL_OP (ap_private<_AP_W, false>(op));  \
  } \
template<int _AP_W, int _AP_I, bool _AP_S, \
         ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N> \
  INLINE bool operator REL_OP ( const af_bit_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> &op, C_TYPE op2) {  \
    return (bool(op)) REL_OP op2;  \
  }  \
template<int _AP_W, int _AP_I, bool _AP_S, \
         ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N> \
  INLINE bool operator REL_OP ( C_TYPE op2, const af_bit_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> &op) {  \
    return op2 REL_OP (bool(op));  \
  } 
 
#define AF_REF_REL_MIX_INT(C_TYPE, _AP_WI, _AP_SI) \
AF_REF_REL_OP_MIX_INT(>, C_TYPE, _AP_WI, _AP_SI) \
AF_REF_REL_OP_MIX_INT(<, C_TYPE, _AP_WI, _AP_SI) \
AF_REF_REL_OP_MIX_INT(>=, C_TYPE, _AP_WI, _AP_SI) \
AF_REF_REL_OP_MIX_INT(<=, C_TYPE, _AP_WI, _AP_SI) \
AF_REF_REL_OP_MIX_INT(==, C_TYPE, _AP_WI, _AP_SI) \
AF_REF_REL_OP_MIX_INT(!=, C_TYPE, _AP_WI, _AP_SI) 

AF_REF_REL_MIX_INT(bool, 1, false)
AF_REF_REL_MIX_INT(char, 8, true)
AF_REF_REL_MIX_INT(signed char, 8, true)
AF_REF_REL_MIX_INT(unsigned char, 8, false)
AF_REF_REL_MIX_INT(short, 16, true)
AF_REF_REL_MIX_INT(unsigned short, 16, false)
AF_REF_REL_MIX_INT(int, 32, true)
AF_REF_REL_MIX_INT(unsigned int, 32, false)
# if defined __x86_64__
AF_REF_REL_MIX_INT(long, 64, true)
AF_REF_REL_MIX_INT(unsigned long, 64, false)
# else
AF_REF_REL_MIX_INT(long, 32, true)
AF_REF_REL_MIX_INT(unsigned long, 32, false)
# endif
AF_REF_REL_MIX_INT(ap_slong, 64, true)
AF_REF_REL_MIX_INT(ap_ulong, 64, false)

#define AF_REF_REL_OP_AP_INT(REL_OP)  \
template<int _AP_W, int _AP_I, bool _AP_S, \
         ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N, int _AP_W2, bool _AP_S2> \
  INLINE bool operator REL_OP ( const af_range_ref<_AP_W,_AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> &op, const ap_private<_AP_W2, _AP_S> &op2) {  \
    return (ap_private<_AP_W, false>(op)).operator REL_OP (op2);  \
  }  \
template<int _AP_W, int _AP_I, bool _AP_S, \
         ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N, int _AP_W2, bool _AP_S2> \
  INLINE bool operator REL_OP (const ap_private<_AP_W2, _AP_S2> &op2, const af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> &op) {  \
    return op2.operator REL_OP (ap_private<_AP_W, false>(op));  \
  } \
template<int _AP_W, int _AP_I, bool _AP_S, \
         ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N, int _AP_W2, bool _AP_S2> \
  INLINE bool operator REL_OP ( const af_bit_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> &op, const ap_private<_AP_W2, _AP_S2> &op2) {  \
    return (ap_private<1, false>(op)).operator REL_OP (op2);  \
  }  \
template<int _AP_W, int _AP_I, bool _AP_S, \
         ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N, int _AP_W2, bool _AP_S2> \
  INLINE bool operator REL_OP ( const ap_private<_AP_W2, _AP_S2> &op2, const af_bit_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> &op) {  \
    return op2.operator REL_OP (ap_private<1,false>(op));  \
  } 
 
AF_REF_REL_OP_AP_INT(>) 
AF_REF_REL_OP_AP_INT(<) 
AF_REF_REL_OP_AP_INT(>=) 
AF_REF_REL_OP_AP_INT(<=) 
AF_REF_REL_OP_AP_INT(==) 
AF_REF_REL_OP_AP_INT(!=) 



// Relational Operators with double
template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
INLINE bool operator == ( double op1, const ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& op2) {
  return op2.operator == (op1);
}


template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
INLINE bool operator != ( double op1, const ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& op2) {
  return op2.operator != (op1);
}


template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
INLINE bool operator > ( double op1, const ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& op2) {
  return op2.operator < (op1);
}


template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
INLINE bool operator >= ( double op1, const ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& op2) {
  return op2.operator <= (op1);
}


template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
INLINE bool operator < ( double op1, const ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& op2) {
  return op2.operator > (op1);
}


template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
INLINE bool operator <= ( double op1, const ap_fixed_base<_AP_W,_AP_I,_AP_S,_AP_Q,_AP_O, _AP_N>& op2) {
  return op2.operator >= (op1);
}


#endif //__AESL_GCC_AP_FIXED_H__

#endif //__SYNTHESIS__

#endif //__cplusplus
