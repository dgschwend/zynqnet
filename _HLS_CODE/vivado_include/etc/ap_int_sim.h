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

#ifndef __AESL_GCC_AP_INT_H__
#define __AESL_GCC_AP_INT_H__

#ifndef __cplusplus
#error C++ is required to include this header file
#else

#ifdef __HLS_SYN__
#error ap_int simulation header file is not applicable for synthesis
#else

#undef _AP_DEBUG_
#include <stdio.h>
#include <iostream>
// for safety
#if (defined(_AP_N)|| defined(_AP_C))
#error One or more of the following is defined: _AP_N, _AP_C. Definition conflicts with their usage as template parameters. 
#endif

// for safety
#if (defined(_AP_W) || defined(_AP_I) || defined(_AP_S) || defined(_AP_Q) || defined(_AP_O) || defined(_AP_W2) || defined(_AP_I2) || defined(_AP_S2) || defined(_AP_Q2) || defined(_AP_O2))
#error One or more of the following is defined: _AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2. Definition conflicts with their usage as template parameters. 
#endif

//for safety
#if (defined(_AP_W3) || defined(_AP_S3) || defined(_AP_W4) || defined(_AP_S4))
#error One or more of the following is defined: _AP_W3, _AP_S3, _AP_W4,_AP_S4. Definition conflicts with their usage as template parameters. 
#endif

//for safety
#if (defined(_AP_W1) || defined(_AP_S1) || defined(_AP_I1) || defined(_AP_T) || defined(_AP_T1) || defined(_AP_T2) || defined(_AP_T3) || defined(_AP_T4))
#error One or more of the following is defined: _AP_W1, _AP_S1, _AP_I1, _AP_T,  _AP_T1, _AP_T2, _AP_T3, _AP_T4. Definition conflicts with their usage as template parameters. 
#endif

#include"etc/ap_private.h"

#ifdef _AP_DEBUG_
#define AP_DEBUG(s) s
#else
#define AP_DEBUG(s)
#endif

#ifndef __SIMULATION__
#define __SIMULATION__
#endif

#if !(defined SYSTEMC_H) && !(defined SYSTEMC_INCLUDED)
#ifndef SC_TRN
#define SC_TRN AP_TRN
#endif
#ifndef SC_RND
#define SC_RND AP_RND
#endif
#ifndef SC_TRN_ZERO
#define SC_TRN_ZERO AP_TRN_ZERO
#endif
#ifndef SC_RND_ZERO
#define SC_RND_ZERO AP_RND_ZERO
#endif
#ifndef SC_RND_INF
#define SC_RND_INF AP_RND_INF
#endif
#ifndef SC_RND_MIN_INF
#define SC_RND_MIN_INF AP_RND_MIN_INF
#endif
#ifndef SC_RND_CONV
#define SC_RND_CONV AP_RND_CONV
#endif
#ifndef SC_WRAP
#define SC_WRAP AP_WRAP
#endif
#ifndef SC_SAT
#define SC_SAT AP_SAT
#endif
#ifndef SC_SAT_ZERO
#define SC_SAT_ZERO AP_SAT_ZERO
#endif
#ifndef SC_SAT_SYM
#define SC_SAT_SYM AP_SAT_SYM
#endif
#ifndef SC_WRAP_SM
#define SC_WRAP_SM AP_WRAP_SM 
#endif
#ifndef SC_BIN
#define SC_BIN 	AP_BIN
#endif
#ifndef SC_OCT
#define SC_OCT  AP_OCT
#endif
#ifndef SC_DEC
#define SC_DEC AP_DEC
#endif
#ifndef SC_HEX
#define SC_HEX AP_HEX
#endif
#endif
#ifndef AP_INT_MAX_W 
#define AP_INT_MAX_W 1024
#endif
#define BIT_WIDTH_UPPER_LIMIT (1 << 15)
#if AP_INT_MAX_W > BIT_WIDTH_UPPER_LIMIT
#error "Bitwidth exceeds 32768 (1 << 15), the maximum allowed value"
#endif
#define MAX_MODE(BITS) ((BITS + 1023) / 1024)

///Forward declaration
template<int _AP_W, bool _AP_S> struct ap_range_ref;
template<int _AP_W, bool _AP_S> struct ap_bit_ref;

template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, 
         ap_o_mode _AP_O, int _AP_N> struct ap_fixed_base;
template<int _AP_W, int _AP_I, bool _AP_S, 
         ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N> struct af_range_ref;
template<int _AP_W, int _AP_I, bool _AP_S, 
         ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N> struct af_bit_ref;
template<int _AP_W> class ap_uint; 

enum {AP_BIN=2,AP_OCT=8,AP_DEC=10,AP_HEX=16};

///Why to use reference?
///Because we will operate the original object indirectly by operating the 
///result object directly after concating or part selecting


///Proxy class which allows concatination to be used as rvalue(for reading) and
//lvalue(for writing)
     
/// Concatination reference.
// ----------------------------------------------------------------
template<int _AP_W1, typename _AP_T1, int _AP_W2, typename _AP_T2>
struct ap_concat_ref {
#ifdef _MSC_VER
#pragma warning( disable : 4521 4522 )
#endif
    enum {_AP_WR=_AP_W1+_AP_W2,};
    _AP_T1& mbv1;
    _AP_T2& mbv2;

    INLINE ap_concat_ref(const ap_concat_ref<_AP_W1, _AP_T1, 
          _AP_W2, _AP_T2>& ref):
           mbv1(ref.mbv1), mbv2(ref.mbv2) {}
    
    INLINE ap_concat_ref(_AP_T1& bv1, _AP_T2& bv2):mbv1(bv1),mbv2(bv2) {}


    template <int _AP_W3, bool _AP_S3>
    INLINE ap_concat_ref& operator = (const ap_private<_AP_W3,_AP_S3>& val) {
        ap_private<_AP_W1+_AP_W2, false>  vval(val);
        int W_ref1=mbv1.length();
        int W_ref2=mbv2.length();
        ap_private<_AP_W1,false> mask1(-1);
        mask1>>=_AP_W1-W_ref1;
        ap_private<_AP_W2,false> mask2(-1);
        mask2>>=_AP_W2-W_ref2;
        mbv1.set(ap_private<_AP_W1,false>((vval>>W_ref2)&mask1));
        mbv2.set(ap_private<_AP_W2,false>(vval&mask2));
        return *this;
    }


    INLINE ap_concat_ref& operator = (unsigned long long val) {
        ap_private<_AP_W1+_AP_W2, false> tmpVal(val);
        return operator = (tmpVal);
    }

    template<int _AP_W3, typename _AP_T3, int _AP_W4, typename _AP_T4>
    INLINE ap_concat_ref& operator = 
        (const ap_concat_ref <_AP_W3, _AP_T3, _AP_W4, _AP_T4>&  val)
    {
        ap_private<_AP_W1+_AP_W2, false> tmpVal(val);
        return operator = (tmpVal);
    }

    INLINE ap_concat_ref& operator = 
        (const ap_concat_ref <_AP_W1, _AP_T1, _AP_W2, _AP_T2>&  val)
    {
        ap_private<_AP_W1+_AP_W2, false> tmpVal(val);
        return operator = (tmpVal);
    }



    template <int _AP_W3, bool _AP_S3>
    INLINE ap_concat_ref& operator =(const ap_bit_ref<_AP_W3, _AP_S3>& val)
    {
        ap_private<_AP_W1+_AP_W2, false> tmpVal(val);
        return operator = (tmpVal);
    }


    template <int _AP_W3, bool _AP_S3>
    INLINE ap_concat_ref& operator =(const ap_range_ref<_AP_W3,_AP_S3>& val)
    {
        ap_private<_AP_W1+_AP_W2, false> tmpVal(val);
        return operator =(tmpVal);
    }

    template<int _AP_W3, int _AP_I3, bool _AP_S3, 
             ap_q_mode _AP_Q3, ap_o_mode _AP_O3, int _AP_N3> 
    INLINE ap_concat_ref& operator= (const af_range_ref<_AP_W3, _AP_I3, _AP_S3,
                                    _AP_Q3, _AP_O3, _AP_N3>& val) {
        return operator = ((const ap_private<_AP_W3, false>)(val));
    } 

    template<int _AP_W3, int _AP_I3, bool _AP_S3, 
             ap_q_mode _AP_Q3, ap_o_mode _AP_O3, int _AP_N3> 
    INLINE ap_concat_ref& operator= (const ap_fixed_base<_AP_W3, _AP_I3, _AP_S3,
                                    _AP_Q3, _AP_O3, _AP_N3>& val) {
        return operator = (val.to_ap_private());
    } 

    template<int _AP_W3, int _AP_I3, bool _AP_S3, 
             ap_q_mode _AP_Q3, ap_o_mode _AP_O3, int _AP_N3> 
    INLINE ap_concat_ref& operator= (const af_bit_ref<_AP_W3, _AP_I3, _AP_S3,
                                    _AP_Q3, _AP_O3, _AP_N3>& val) {
        return operator=((unsigned long long)(bool)(val));
     }


    INLINE operator ap_private<_AP_WR, false> () const
    {
        return get();
    }

    INLINE operator unsigned long long () const 
    {
         return get().to_uint64();
    }

    template<int _AP_W3, bool _AP_S3>
    INLINE ap_concat_ref<_AP_WR, ap_concat_ref, _AP_W3, ap_range_ref<_AP_W3, _AP_S3> >
        operator, (const ap_range_ref<_AP_W3, _AP_S3> &a2) 
    {
        return ap_concat_ref<_AP_WR, ap_concat_ref, 
                           _AP_W3, ap_range_ref<_AP_W3, _AP_S3> >(*this, 
                           const_cast<ap_range_ref<_AP_W3, _AP_S3> &>(a2));
    }


    template<int _AP_W3, bool _AP_S3>
    INLINE ap_concat_ref<_AP_WR, ap_concat_ref, _AP_W3, ap_private<_AP_W3, _AP_S3> >
        operator, (ap_private<_AP_W3, _AP_S3> &a2) 
    {
          return ap_concat_ref<_AP_WR, ap_concat_ref, 
                               _AP_W3, ap_private<_AP_W3, _AP_S3> >(*this, a2);
    }

    template<int _AP_W3, bool _AP_S3>
    INLINE ap_concat_ref<_AP_WR, ap_concat_ref, _AP_W3, ap_private<_AP_W3, _AP_S3> >
        operator, (const ap_private<_AP_W3, _AP_S3> &a2) 
    {
          return ap_concat_ref<_AP_WR, ap_concat_ref, 
                               _AP_W3, ap_private<_AP_W3, _AP_S3> >(*this, 
                               const_cast<ap_private<_AP_W3, _AP_S3>&>(a2));
    }
        

    template<int _AP_W3, bool _AP_S3>
    INLINE ap_concat_ref<_AP_WR, ap_concat_ref, 1, ap_bit_ref<_AP_W3, _AP_S3> >
        operator, (const ap_bit_ref<_AP_W3, _AP_S3> &a2)
    {
        return ap_concat_ref<_AP_WR, ap_concat_ref, 
                               1, ap_bit_ref<_AP_W3, _AP_S3> >(*this, 
                          const_cast<ap_bit_ref<_AP_W3, _AP_S3> &>(a2));
    }
    
    template<int _AP_W3, typename _AP_T3, int _AP_W4, typename _AP_T4>
    INLINE ap_concat_ref<_AP_WR, ap_concat_ref, _AP_W3+_AP_W4, ap_concat_ref<_AP_W3,_AP_T3,_AP_W4,_AP_T4> >
        operator, (const ap_concat_ref<_AP_W3,_AP_T3,_AP_W4,_AP_T4> &a2) 
    {
        return ap_concat_ref<_AP_WR, ap_concat_ref, 
                               _AP_W3+_AP_W4, ap_concat_ref<_AP_W3,_AP_T3,_AP_W4,
                               _AP_T4> >(*this, const_cast<ap_concat_ref<_AP_W3,
                               _AP_T3,_AP_W4, _AP_T4>& >(a2));
    }

    template <int _AP_W3, int _AP_I3, bool _AP_S3, ap_q_mode _AP_Q3, ap_o_mode _AP_O3, int _AP_N3>
    INLINE
    ap_concat_ref<_AP_WR, ap_concat_ref, _AP_W3, af_range_ref<_AP_W3, _AP_I3, _AP_S3, _AP_Q3, _AP_O3, _AP_N3> >
    operator, (const af_range_ref<_AP_W3, _AP_I3, _AP_S3, _AP_Q3, 
               _AP_O3, _AP_N3> &a2) {
        return ap_concat_ref<_AP_WR, ap_concat_ref, _AP_W3, af_range_ref<_AP_W3,
                _AP_I3, _AP_S3, _AP_Q3, _AP_O3, _AP_N3> >(*this, 
                const_cast<af_range_ref<_AP_W3,_AP_I3, _AP_S3, _AP_Q3, 
                _AP_O3, _AP_N3>& >(a2));
    }
    
    template <int _AP_W3, int _AP_I3, bool _AP_S3, ap_q_mode _AP_Q3, ap_o_mode _AP_O3, int _AP_N3>
    INLINE
    ap_concat_ref<_AP_WR, ap_concat_ref, 1, af_bit_ref<_AP_W3, _AP_I3, _AP_S3, _AP_Q3, _AP_O3, _AP_N3> >
    operator, (const af_bit_ref<_AP_W3, _AP_I3, _AP_S3, _AP_Q3, 
               _AP_O3, _AP_N3> &a2) {
        return ap_concat_ref<_AP_WR, ap_concat_ref, 1, af_bit_ref<_AP_W3,
                _AP_I3, _AP_S3, _AP_Q3, _AP_O3, _AP_N3> >(*this, 
                const_cast<af_bit_ref<_AP_W3,_AP_I3, _AP_S3, 
                _AP_Q3, _AP_O3, _AP_N3>& >(a2));
    }

    template<int _AP_W3, bool _AP_S3>
    INLINE ap_private<AP_MAX(_AP_WR,_AP_W3), _AP_S3>
        operator & (const ap_private<_AP_W3,_AP_S3>& a2) 
    {
        return get() & a2;
    }


    template<int _AP_W3, bool _AP_S3>
    INLINE ap_private<AP_MAX(_AP_WR,_AP_W3), _AP_S3>
        operator | (const ap_private<_AP_W3,_AP_S3>& a2)
    {
        return get() | a2;
    }


    template<int _AP_W3, bool _AP_S3>
    INLINE ap_private<AP_MAX(_AP_WR,_AP_W3), _AP_S3>
        operator ^ (const ap_private<_AP_W3,_AP_S3>& a2) 
    {
      return ap_private<AP_MAX(_AP_WR,_AP_W3), _AP_S3>(get() ^ a2);
    }


    INLINE const ap_private<_AP_WR, false> get() const 
    {
      ap_private<_AP_W1+_AP_W2, false> tmpVal = ap_private<_AP_W1+_AP_W2, false> (mbv1.get());
      ap_private<_AP_W1+_AP_W2, false> tmpVal2 = ap_private<_AP_W1+_AP_W2, false> (mbv2.get());
        int W_ref2 = mbv2.length();
        tmpVal <<= W_ref2;
        tmpVal |= tmpVal2;
        return tmpVal;
    }


    INLINE const ap_private<_AP_WR, false> get() 
    {
      ap_private<_AP_W1+_AP_W2, false> tmpVal =ap_private<_AP_W1+_AP_W2, false> ( mbv1.get());
      ap_private<_AP_W1+_AP_W2, false> tmpVal2 = ap_private<_AP_W1+_AP_W2, false> (mbv2.get());
        int W_ref2 = mbv2.length();
        tmpVal <<= W_ref2;
        tmpVal |= tmpVal2;
        return tmpVal;
    }


    template <int _AP_W3>
    INLINE void set(const ap_private<_AP_W3,false> & val)
    {
        ap_private<_AP_W1+_AP_W2, false> vval(val);
        int W_ref1=mbv1.length();
        int W_ref2=mbv2.length();
        ap_private<_AP_W1,false> mask1(-1);
        mask1>>=_AP_W1-W_ref1;
        ap_private<_AP_W2,false> mask2(-1);
        mask2>>=_AP_W2-W_ref2;
        mbv1.set(ap_private<_AP_W1,false>((vval>>W_ref2)&mask1));
        mbv2.set(ap_private<_AP_W2,false>(vval&mask2));
    }


    INLINE int length() const {
        return mbv1.length()+mbv2.length();
    }
    
    INLINE std::string to_string(uint8_t radix=2) const {
        return get().to_string(radix);
    } 
};

///Proxy class, which allows part selection to be used as rvalue(for reading) and
//lvalue(for writing)

///Range(slice)  reference
//------------------------------------------------------------
template<int _AP_W, bool _AP_S>
struct ap_range_ref {
#ifdef _MSC_VER
#pragma warning( disable : 4521 4522 )
#endif
    ap_private<_AP_W,_AP_S> &d_bv;
    int l_index;
    int h_index;

public:
    INLINE ap_range_ref(const ap_range_ref<_AP_W, _AP_S>& ref):
         d_bv(ref.d_bv), l_index(ref.l_index), h_index(ref.h_index) {}

    INLINE ap_range_ref(ap_private<_AP_W,_AP_S>* bv, int h, int l):
        d_bv(*bv),l_index(l),h_index(h)
    {
        if (h < 0 || l < 0) 
	    fprintf(stderr, "Warning! Higher bound (%d) and lower bound (%d) cannot be negative.\n", h, l);
        if (h >= _AP_W || l >= _AP_W)
	    fprintf(stderr, "Warning! Higher bound (%d) or lower bound (%d) out of range (%d).\n", h, l, _AP_W);
    
        //assert((h>=0 && l>=0) && "Higher bound and lower bound cannot be negative.");
        //assert((h<_AP_W && l<_AP_W) && "Higher bound or lower bound out of range.");
        //if (h < l) 
	  //fprintf(stderr, "Warning! The bits selected will be returned in reverse order\n");
    }


    INLINE operator ap_private<_AP_W, false> () const 
    {
        ap_private<_AP_W, false> val(0);
        if(h_index>=l_index) {
	  if (_AP_W > 64) {
            val=d_bv;
            ap_private<_AP_W,false> mask(-1);
            mask>>=_AP_W-(h_index-l_index+1);
            val>>=l_index;
            val&=mask;
	  } else {
	    const static uint64_t mask = (~0ULL>> (64>_AP_W ? (64-_AP_W):0));	    
	    val = (d_bv >> l_index) & (mask >>(_AP_W-(h_index-l_index+1)));
	  }
        } else {
            for(int i=0, j=l_index;j>=0&&j>=h_index;j--,i++)
                    if((d_bv)[j]) val.set(i);
        }
        return val;
    }

    INLINE operator unsigned long long () const 
    {
        return to_uint64();
    }

    template<int _AP_W2,bool _AP_S2>
    INLINE ap_range_ref& operator =(const ap_private<_AP_W2,_AP_S2>& val)
    {
      ap_private<_AP_W,false> vval=ap_private<_AP_W,false>(val);
      if(l_index>h_index)
	{
	  for(int i=0, j=l_index;j>=0&&j>=h_index;j--,i++)
	    (vval)[i]? d_bv.set(j):d_bv.clear(j);
        } else {
	if (_AP_W > 64) {
	  ap_private<_AP_W,false> mask(-1);
	  if(l_index>0)
	    {
	      mask<<=l_index;
	      vval<<=l_index;
	    }
	  if(h_index<_AP_W-1)
	    {
	      ap_private<_AP_W,false> mask2(-1);
	      mask2>>=_AP_W-h_index-1;
	      mask&=mask2;
	      vval&=mask2;
	    }
	  mask.flip();
	  d_bv&=mask;
	  d_bv|=vval;
	} else {
	  unsigned shift = 64-_AP_W;
	  uint64_t mask = ~0ULL>>(shift);
	  if(l_index>0)
	    {
	      vval = mask & vval << l_index;
	      mask = mask & mask << l_index;
	    }
	  if(h_index<_AP_W-1)
	    {
	      uint64_t mask2 = mask;
	      mask2 >>= (_AP_W-h_index-1);
	      mask&=mask2;
	      vval&=mask2;
	    }
	  mask = ~mask;
	  d_bv&=mask;
	  d_bv|=vval;
	}
      }
      return *this;
    }
      
  INLINE ap_range_ref& operator = (unsigned long long val)
    {
        const ap_private<_AP_W,_AP_S> vval=val;
        return operator = (vval);
    }


    INLINE ap_range_ref& operator =(const ap_range_ref<_AP_W, _AP_S>& val)
    {
        const ap_private<_AP_W, false> tmpVal(val);
        return operator =(tmpVal);
    }

    

    template<int _AP_W3, typename _AP_T3, int _AP_W4, typename _AP_T4>
    INLINE ap_range_ref& operator = 
        (const ap_concat_ref <_AP_W3, _AP_T3, _AP_W4, _AP_T4>& val)
    {
        const ap_private<_AP_W, false> tmpVal(val);
        return operator = (tmpVal);
    }

    template <int _AP_W3, bool _AP_S3>
    INLINE ap_range_ref& operator =(const ap_range_ref<_AP_W3,_AP_S3>& val)
    {
        const ap_private<_AP_W, false> tmpVal(val);
        return operator =(tmpVal);
    }

    template<int _AP_W2, int _AP_I2, bool _AP_S2, 
             ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2> 
    INLINE ap_range_ref& operator= (const af_range_ref<_AP_W2, _AP_I2, _AP_S2,
                                    _AP_Q2, _AP_O2, _AP_N2>& val) {
        return operator=((const ap_private<_AP_W2, _AP_S2>)(val));
    } 

    template<int _AP_W2, int _AP_I2, bool _AP_S2, 
             ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2> 
    INLINE ap_range_ref& operator= (const ap_fixed_base<_AP_W2, _AP_I2, _AP_S2,
                                    _AP_Q2, _AP_O2, _AP_N2>& val) {
        return operator=(val.to_ap_private());
    }
 
    template<int _AP_W2, int _AP_I2, bool _AP_S2, 
             ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2> 
    INLINE ap_range_ref& operator= (const af_bit_ref<_AP_W2, _AP_I2, _AP_S2,
                                    _AP_Q2, _AP_O2, _AP_N2>& val) {
        return operator=((unsigned long long)(bool)(val));
    }

    template<int _AP_W2, bool _AP_S2>
    INLINE ap_range_ref& operator= (const ap_bit_ref<_AP_W2, _AP_S2>& val) {
        return operator=((unsigned long long)(bool)(val));
    }

    template <int _AP_W2, bool _AP_S2>
    INLINE 
    ap_concat_ref<_AP_W,ap_range_ref,_AP_W2,ap_range_ref<_AP_W2,_AP_S2> > 
    operator, (const ap_range_ref<_AP_W2,_AP_S2> &a2) 
    {
        return 
            ap_concat_ref<_AP_W, ap_range_ref, _AP_W2, 
                   ap_range_ref<_AP_W2,_AP_S2> >(*this,
                   const_cast<ap_range_ref<_AP_W2,_AP_S2>& >(a2));
    }


    template <int _AP_W2, bool _AP_S2>
    INLINE ap_concat_ref<_AP_W,ap_range_ref,_AP_W2,ap_private<_AP_W2,_AP_S2> > 
    operator , (ap_private<_AP_W2,_AP_S2>& a2)
    {
        return 
            ap_concat_ref<_AP_W, ap_range_ref, _AP_W2, ap_private<_AP_W2,_AP_S2> >(*this, a2);
    }

    INLINE ap_concat_ref<_AP_W,ap_range_ref,_AP_W,ap_private<_AP_W,_AP_S> > 
    operator , (ap_private<_AP_W, _AP_S>& a2)
    {
        return 
            ap_concat_ref<_AP_W, ap_range_ref, _AP_W, 
                          ap_private<_AP_W,_AP_S> >(*this, a2);
    }



    template <int _AP_W2, bool _AP_S2>
    INLINE
    ap_concat_ref<_AP_W,ap_range_ref,1,ap_bit_ref<_AP_W2,_AP_S2> > 
    operator, (const ap_bit_ref<_AP_W2,_AP_S2> &a2) 
    {
        return 
            ap_concat_ref<_AP_W, ap_range_ref, 1, 
                      ap_bit_ref<_AP_W2,_AP_S2> >(*this, const_cast<ap_bit_ref<
                      _AP_W2,_AP_S2>& >(a2));
    }


    template <int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
    INLINE
    ap_concat_ref<_AP_W, ap_range_ref, _AP_W2+_AP_W3, ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3> >
    operator, (const ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3> &a2) 
    {
        return ap_concat_ref<_AP_W, ap_range_ref, _AP_W2+_AP_W3, 
                       ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3> >(*this, 
                       const_cast<ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, 
                       _AP_T3>& >(a2));
    }

    template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2>
    INLINE
    ap_concat_ref<_AP_W, ap_range_ref, _AP_W2, af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >
    operator, (const af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, 
               _AP_O2, _AP_N2> &a2) {
        return ap_concat_ref<_AP_W, ap_range_ref, _AP_W2, af_range_ref<_AP_W2,
                _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >(*this, 
                const_cast<af_range_ref<_AP_W2,_AP_I2, _AP_S2, 
                _AP_Q2, _AP_O2, _AP_N2>& >(a2));
    }
    
    template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2>
    INLINE
    ap_concat_ref<_AP_W, ap_range_ref, 1, af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >
    operator, (const af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, 
               _AP_O2, _AP_N2> &a2) {
        return ap_concat_ref<_AP_W, ap_range_ref, 1, af_bit_ref<_AP_W2,
                _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >(*this, 
                const_cast<af_bit_ref<_AP_W2,_AP_I2, _AP_S2, 
                _AP_Q2, _AP_O2, _AP_N2>& >(a2));
    }

    template<int _AP_W2, bool _AP_S2>
    INLINE bool operator == (const ap_range_ref<_AP_W2, _AP_S2>& op2)
    {
        ap_private<_AP_W,false> lhs=get();
        ap_private<_AP_W2,false> rhs=op2.get();
        return lhs==rhs;
    }


    template<int _AP_W2, bool _AP_S2>
    INLINE bool operator != (const ap_range_ref<_AP_W2, _AP_S2>& op2) 
    {
        ap_private<_AP_W,false> lhs=get();
        ap_private<_AP_W2,false> rhs=op2.get();
        return lhs!=rhs;
    }


    template<int _AP_W2, bool _AP_S2>
    INLINE bool operator > (const ap_range_ref<_AP_W2, _AP_S2>& op2) 
    {
        ap_private<_AP_W,false> lhs=get();
        ap_private<_AP_W2,false> rhs=op2.get();
        return lhs>rhs;
    }


    template<int _AP_W2, bool _AP_S2>
    INLINE bool operator >= (const ap_range_ref<_AP_W2, _AP_S2>& op2)
    {
        ap_private<_AP_W,false> lhs=get();
        ap_private<_AP_W2,false> rhs=op2.get();
        return lhs>=rhs;
    }


    template<int _AP_W2, bool _AP_S2>
    INLINE bool operator < (const ap_range_ref<_AP_W2, _AP_S2>& op2)
    {
        ap_private<_AP_W,false> lhs=get();
        ap_private<_AP_W2,false> rhs=op2.get();
        return lhs<rhs;
    }


    template<int _AP_W2, bool _AP_S2>
    INLINE bool operator <= (const ap_range_ref<_AP_W2, _AP_S2>& op2) 
    {
        ap_private<_AP_W,false> lhs=get();
        ap_private<_AP_W2,false> rhs=op2.get();
        return lhs<=rhs;
    }


    template<int _AP_W2>
    INLINE void set(const ap_private<_AP_W2,false>& val)
    {
        ap_private<_AP_W,_AP_S> vval=val;
        if(l_index>h_index)
        {
            for(int i=0, j=l_index;j>=0&&j>=h_index;j--,i++)
                    (vval)[i]? d_bv.set(j):d_bv.clear(j);
        } else {
	  if (_AP_W>64 ) {
            ap_private<_AP_W,_AP_S> mask(-1);
            if(l_index>0)
	      {
                ap_private<_AP_W,false> mask1(-1);
                mask1>>=_AP_W-l_index;
                mask1.flip();
                mask=mask1;
                //vval&=mask1;
                vval<<=l_index;
	      }
            if(h_index<_AP_W-1)
	      {
                ap_private<_AP_W,false> mask2(-1);
                mask2<<=h_index+1;
                mask2.flip();
                mask&=mask2;
                vval&=mask2;
	      }
            mask.flip();
            d_bv&=mask;
            d_bv|=vval;
	  } else {
	    uint64_t mask = ~0ULL >> (64-_AP_W);
            if(l_index>0)
	      {
		uint64_t mask1 = mask;
		mask1=mask & (mask1>>(_AP_W-l_index));
		vval =mask&( vval <<l_index);
		mask=~mask1&mask;
		//vval&=mask1;
	      }
            if(h_index<_AP_W-1) {
		uint64_t mask2 = ~0ULL >> (64-_AP_W);
		mask2 = mask &(mask2<<(h_index+1));
		mask&=~mask2;
		vval&=~mask2;
	      }
            d_bv&=(~mask&(~0ULL >> (64-_AP_W)));
            d_bv|=vval;
	  }
	}
    }


    INLINE ap_private<_AP_W,false> get() const
    {
      ap_private<_AP_W,false> val(0);
      if(h_index<l_index) {
	for(int i=0, j=l_index;j>=0&&j>=h_index;j--,i++)
	  if((d_bv)[j]) val.set(i);
      } else {
	val=d_bv;
	val>>=l_index;
	if(h_index<_AP_W-1)
	  {
	    if (_AP_W <= 64) {
	      const static uint64_t mask = (~0ULL>> (64>_AP_W ? (64-_AP_W):0));
	      val &=  (mask>> (_AP_W-(h_index-l_index+1)));
	    } else {
	      ap_private<_AP_W,false> mask(-1);
	      mask>>=_AP_W-(h_index-l_index+1);
	      val&=mask;
	    }
	  }
      }
      return val;
    }
  
  
  INLINE ap_private<_AP_W,false> get() 
  {
    ap_private<_AP_W,false> val(0);
    if(h_index<l_index) {
      for(int i=0, j=l_index;j>=0&&j>=h_index;j--,i++)
	if((d_bv)[j]) val.set(i);
    } else {
      val=d_bv;
      val>>=l_index;
      if(h_index<_AP_W-1)
	{
	  if (_AP_W <= 64 ) {
	    static const uint64_t mask = ~0ULL>> (64>_AP_W ? (64-_AP_W):0);
	    return val &= ( (mask) >> (_AP_W - (h_index-l_index+1)));
	  } else {
	    ap_private<_AP_W,false> mask(-1);
	    mask>>=_AP_W-(h_index-l_index+1);
	    val&=mask;
	  }
	}
    }
    return val;
  }


    INLINE int length() const
    {
        return h_index>=l_index?h_index-l_index+1:l_index-h_index+1;
    }


    INLINE int to_int() const 
    {
        ap_private<_AP_W,false> val=get();
        return val.to_int();
    }


    INLINE unsigned int to_uint() const 
    {
        ap_private<_AP_W,false> val=get();
        return val.to_uint();
    }


    INLINE long to_long() const 
    {
        ap_private<_AP_W,false> val=get();
        return val.to_long();
    }


    INLINE unsigned long to_ulong() const 
    {
        ap_private<_AP_W,false> val=get();
        return val.to_ulong();
    }


    INLINE ap_slong to_int64() const 
    {
        ap_private<_AP_W,false> val=get();
        return val.to_int64();
    }


    INLINE ap_ulong to_uint64() const
    {
        ap_private<_AP_W,false> val=get();
        return val.to_uint64();
    }
    
    INLINE std::string to_string(uint8_t radix=2) const {
        return get().to_string(radix);
    } 

    INLINE bool and_reduce() {
        bool ret = true;
        bool reverse = l_index > h_index;
        unsigned low = reverse ? h_index : l_index;
        unsigned high = reverse ? l_index : h_index;
        for (unsigned i = low; i != high; ++i)
            ret &= d_bv[i];
        return ret;
    }

    INLINE bool or_reduce() {
        bool ret = false;
        bool reverse = l_index > h_index;
        unsigned low = reverse ? h_index : l_index;
        unsigned high = reverse ? l_index : h_index;
        for (unsigned i = low; i != high; ++i)
            ret |= d_bv[i];
        return ret;
    }

    INLINE bool xor_reduce() {
        bool ret = false;
        bool reverse = l_index > h_index;
        unsigned low = reverse ? h_index : l_index;
        unsigned high = reverse ? l_index : h_index;
        for (unsigned i = low; i != high; ++i)
            ret ^= d_bv[i];
        return ret;
    }
};

///Proxy class, which allows bit selection to be used as rvalue(for reading) and
//lvalue(for writing)

///Bit reference
//--------------------------------------------------------------
template <int _AP_W, bool _AP_S>
struct ap_bit_ref {
#ifdef _MSC_VER
#pragma warning( disable : 4521 4522 )
#endif
    ap_private<_AP_W,_AP_S>& d_bv;
    int d_index;

public:
    INLINE ap_bit_ref(const ap_bit_ref<_AP_W, _AP_S>& ref):
            d_bv(ref.d_bv), d_index(ref.d_index) {}

    INLINE ap_bit_ref(ap_private<_AP_W,_AP_S>& bv, int index=0):
        d_bv(bv),d_index(index)
    {
        if (d_index  < 0 ) 
	    fprintf(stderr, "Warning! Index of bit vector  (%d) cannot be negative.\n", d_index);
        if (d_index >= _AP_W)
	    fprintf(stderr, "Warning! Index of bit vector (%d) out of range (%d).\n", d_index, _AP_W);
        //assert(d_index<_AP_W&&"Index of bit vector out of bound");
        //assert(d_index>=0&&"index of bit vector cannot be negative");
    }


    INLINE operator bool () const 
    {
      return d_bv.get_bit(d_index);
    }


    INLINE bool to_bool() const
    {
        return operator bool ();
    }


    INLINE ap_bit_ref& operator = (unsigned long long val)
    {
        if(val)
            d_bv.set(d_index);
        else
            d_bv.clear(d_index);
        return *this;
    }


#if 0
    INLINE ap_bit_ref& operator = (bool val)
    {
        if(val)
            d_bv.set(d_index);
        else
            d_bv.clear(d_index);
        return *this;
    }
#endif
    template<int _AP_W2, bool _AP_S2>
    INLINE ap_bit_ref& operator =(const ap_private<_AP_W2,_AP_S2>& val)
    {
        return operator =((unsigned long long)(val != 0));
    }


    template<int _AP_W2, bool _AP_S2>
    INLINE ap_bit_ref& operator =(const ap_bit_ref<_AP_W2,_AP_S2>& val)
    {
        return operator =((unsigned long long)(bool)val);
    }

    INLINE ap_bit_ref& operator =(const ap_bit_ref<_AP_W,_AP_S>& val)
    {
        return operator =((unsigned long long)(bool)val);
    }

    template<int _AP_W2, bool _AP_S2>
    INLINE ap_bit_ref& operator =(const ap_range_ref<_AP_W2,_AP_S2>&  val)
    {
        return operator =((unsigned long long)(bool) val);
    }


    template<int _AP_W2, int _AP_I2, bool _AP_S2, 
             ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2> 
    INLINE ap_bit_ref& operator= (const af_range_ref<_AP_W2, _AP_I2, _AP_S2,
                                    _AP_Q2, _AP_O2, _AP_N2>& val) {
        return operator=((const ap_private<_AP_W2, false>)(val));
    } 

    template<int _AP_W2, int _AP_I2, bool _AP_S2, 
             ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2> 
    INLINE ap_bit_ref& operator= (const af_bit_ref<_AP_W2, _AP_I2, _AP_S2,
                                    _AP_Q2, _AP_O2, _AP_N2>& val) {
        return operator=((unsigned long long)(bool)(val));
    }

    template<int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
    INLINE ap_bit_ref& operator= (const ap_concat_ref<_AP_W2, _AP_T3, _AP_W3, _AP_T3>& val) {
        return operator=((const ap_private<_AP_W2 + _AP_W3, false>)(val));
    }
 


    template<int _AP_W2, bool _AP_S2>
    INLINE ap_concat_ref<1, ap_bit_ref, _AP_W2, ap_private<_AP_W2,_AP_S2> > 
    operator , (ap_private<_AP_W2, _AP_S2>& a2)
     {
        return ap_concat_ref<1, ap_bit_ref, _AP_W2, ap_private<_AP_W2,_AP_S2> >(*this, a2);
    }

    template<int _AP_W2, bool _AP_S2>
    INLINE ap_concat_ref<1, ap_bit_ref, _AP_W2, ap_range_ref<_AP_W2,_AP_S2> >
    operator, (const ap_range_ref<_AP_W2, _AP_S2> &a2)
    {
        return 
            ap_concat_ref<1, ap_bit_ref, _AP_W2, ap_range_ref<_AP_W2,_AP_S2> >(*this, 
                    const_cast<ap_range_ref<_AP_W2, _AP_S2> &>(a2));
    }


    template<int _AP_W2, bool _AP_S2>
    INLINE ap_concat_ref<1, ap_bit_ref, 1, ap_bit_ref<_AP_W2,_AP_S2> >
    operator, (const ap_bit_ref<_AP_W2, _AP_S2> &a2) 
    {
        return 
            ap_concat_ref<1, ap_bit_ref, 1, ap_bit_ref<_AP_W2,_AP_S2> >(*this, 
                    const_cast<ap_bit_ref<_AP_W2, _AP_S2> &>(a2));
    }


    INLINE ap_concat_ref<1, ap_bit_ref, 1, ap_bit_ref >
    operator, (const ap_bit_ref &a2) 
    {
        return 
            ap_concat_ref<1, ap_bit_ref, 1, ap_bit_ref >(*this, 
                const_cast<ap_bit_ref&>(a2));
    }


    template<int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
    INLINE ap_concat_ref<1, ap_bit_ref, _AP_W2+_AP_W3, ap_concat_ref<_AP_W2,_AP_T2,_AP_W3,_AP_T3> >
    operator, (const ap_concat_ref<_AP_W2,_AP_T2,_AP_W3,_AP_T3> &a2) 
    {
        return 
            ap_concat_ref<1,ap_bit_ref,_AP_W2+_AP_W3,
                    ap_concat_ref<_AP_W2,_AP_T2,_AP_W3,_AP_T3> >(*this, 
                    const_cast<ap_concat_ref<_AP_W2,_AP_T2,_AP_W3,_AP_T3>& >(a2));
    }

    template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2>
    INLINE
    ap_concat_ref<1, ap_bit_ref, _AP_W2, af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >
    operator, (const af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, 
               _AP_O2, _AP_N2> &a2) {
        return ap_concat_ref<1, ap_bit_ref, _AP_W2, af_range_ref<_AP_W2,
                _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >(*this, 
                const_cast<af_range_ref<_AP_W2,_AP_I2, _AP_S2, _AP_Q2, 
                _AP_O2, _AP_N2>& >(a2));
    }
    
    template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2, int _AP_N2>
    INLINE
    ap_concat_ref<1, ap_bit_ref, 1, af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >
    operator, (const af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, 
               _AP_O2, _AP_N2> &a2) {
        return ap_concat_ref<1, ap_bit_ref, 1, af_bit_ref<_AP_W2,
                _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2> >(*this, 
                const_cast<af_bit_ref<_AP_W2, _AP_I2, _AP_S2, 
                _AP_Q2, _AP_O2, _AP_N2>& >(a2));
    }

    template<int _AP_W2, bool _AP_S2>
    INLINE bool operator == (const ap_bit_ref<_AP_W2, _AP_S2>& op) {
        return get() == op.get();
    }

    template<int _AP_W2, bool _AP_S2>
    INLINE bool operator != (const ap_bit_ref<_AP_W2, _AP_S2>& op) {
        return get() != op.get();
    }


    INLINE bool get() const
    {
        return operator bool ();
    }
    

    INLINE bool get() 
    {
        return operator bool ();
    }


    template <int _AP_W3>
    INLINE void set(const ap_private<_AP_W3, false>& val) 
    {
        operator = (val);
    }

    INLINE bool operator ~ () const {
        bool bit = (d_bv)[d_index];
        return bit ? false : true;
    }

    INLINE int length() const { return 1; }
    
    INLINE std::string to_string() const {
        bool val = get();
        return val ? "1" : "0";
    } 
};

//char a[100];
//char* ptr = a;
//ap_int<2> n = 3;
//char* ptr2 = ptr + n*2;
//avoid ambiguous errors
#define OP_BIN_MIX_PTR(BIN_OP) \
    template<typename PTR_TYPE, int _AP_W, bool _AP_S> \
    INLINE PTR_TYPE* \
    operator BIN_OP (PTR_TYPE* i_op, const ap_private<_AP_W,_AP_S> &op) {  \
        typename ap_private<_AP_W,_AP_S>::ValType op2 = op; \
        return i_op BIN_OP op2;  \
    } \
    template<typename PTR_TYPE, int _AP_W, bool _AP_S> \
    INLINE PTR_TYPE*  \
    operator BIN_OP (const ap_private<_AP_W,_AP_S> &op, PTR_TYPE* i_op) {  \
        typename ap_private<_AP_W,_AP_S>::ValType op2 = op;  \
        return op2 BIN_OP i_op;  \
    } 

OP_BIN_MIX_PTR(+)
OP_BIN_MIX_PTR(-)

//float OP ap_int
//when ap_int<wa>'s width > 64, then trunc ap_int<w> to ap_int<64>
#define OP_BIN_MIX_FLOAT(BIN_OP, C_TYPE) \
    template<int _AP_W, bool _AP_S> \
    INLINE C_TYPE  \
    operator BIN_OP (C_TYPE i_op, const ap_private<_AP_W,_AP_S> &op) {  \
        typename ap_private<_AP_W,_AP_S>::ValType op2 = op;  \
        return i_op BIN_OP op2;  \
    }  \
    template<int _AP_W, bool _AP_S> \
    INLINE C_TYPE  \
    operator BIN_OP (const ap_private<_AP_W,_AP_S> &op, C_TYPE i_op) {  \
        typename ap_private<_AP_W,_AP_S>::ValType op2 = op;  \
        return op2 BIN_OP i_op;  \
    } 

#define OPS_MIX_FLOAT(C_TYPE)		\
    OP_BIN_MIX_FLOAT(*, C_TYPE)	\
    OP_BIN_MIX_FLOAT(/, C_TYPE)	\
    OP_BIN_MIX_FLOAT(+, C_TYPE)	\
    OP_BIN_MIX_FLOAT(-, C_TYPE)

OPS_MIX_FLOAT(float)
OPS_MIX_FLOAT(double)

/// Operators mixing Integers with AP_Int
// ----------------------------------------------------------------
#if 0
#define OP_BIN_MIX_INT(BIN_OP, C_TYPE, _AP_WI, _AP_SI, RTYPE)  \
  template<int _AP_W, bool _AP_S>					\
  INLINE typename ap_private<_AP_W,_AP_S>::template RType<_AP_WI,_AP_SI>::RTYPE \
  operator BIN_OP ( const ap_private<_AP_W,_AP_S> &op, C_TYPE i_op) {	\
    return op.operator BIN_OP (ap_private<_AP_WI,_AP_SI>(i_op));	\
  }  
#else
// partially specialize template argument _AP_C in order that:
// for _AP_W > 64, we will explicitly convert operand with native data type
// into corresponding ap_private
// for _AP_W <= 64, we will implicitly convert operand with ap_private into
// (unsigned) long long
#define OP_BIN_MIX_INT(BIN_OP, C_TYPE, _AP_WI, _AP_SI, RTYPE)  \
    template<int _AP_W, bool _AP_S> \
    INLINE typename ap_private<_AP_WI,_AP_SI>::template RType<_AP_W,_AP_S>::RTYPE \
    operator BIN_OP ( C_TYPE i_op, const ap_private<_AP_W,_AP_S> &op) {  \
        return ap_private<_AP_WI,_AP_SI>(i_op).operator BIN_OP (op);  \
      } \
    template<int _AP_W, bool _AP_S>   \
    INLINE typename ap_private<_AP_W,_AP_S>::template RType<_AP_WI,_AP_SI>::RTYPE \
      operator BIN_OP ( const ap_private<_AP_W,_AP_S> &op, C_TYPE i_op) {  \
        return op.operator BIN_OP (ap_private<_AP_WI,_AP_SI>(i_op));  \
    } 
#endif
#define OP_REL_MIX_INT(REL_OP, C_TYPE, _AP_W2, _AP_S2)  \
  template<int _AP_W, bool _AP_S>					\
  INLINE bool operator REL_OP ( const ap_private<_AP_W,_AP_S> &op, C_TYPE op2) { \
    return op.operator REL_OP (ap_private<_AP_W2, _AP_S2>(op2));	\
  }									\
  template<int _AP_W, bool _AP_S>					\
  INLINE bool operator REL_OP ( C_TYPE op2, const ap_private<_AP_W,_AP_S, false> &op) { \
    return ap_private<_AP_W2,_AP_S2>(op2).operator REL_OP (op);		\
  }									
#define OP_ASSIGN_MIX_INT(ASSIGN_OP, C_TYPE, _AP_W2, _AP_S2)		\
  template<int _AP_W, bool _AP_S>					\
  INLINE ap_private<_AP_W,_AP_S> &operator ASSIGN_OP ( ap_private<_AP_W,_AP_S> &op, C_TYPE op2) { \
    return op.operator ASSIGN_OP (ap_private<_AP_W2,_AP_S2>(op2));	\
  }

#define OP_BIN_SHIFT_INT(BIN_OP, C_TYPE, _AP_WI, _AP_SI, RTYPE)		\
  template<int _AP_W, bool _AP_S>					\
  C_TYPE  operator BIN_OP ( C_TYPE i_op, const ap_private<_AP_W,_AP_S, false> &op) { \
    return i_op BIN_OP (op.get_VAL());					\
  }									\
  template<int _AP_W, bool _AP_S>					\
  INLINE typename ap_private<_AP_W,_AP_S>::template RType<_AP_WI,_AP_SI>::RTYPE \
  operator BIN_OP ( const ap_private<_AP_W,_AP_S> &op, C_TYPE i_op) {	\
    return op.operator BIN_OP (i_op);		\
  }
#define OP_ASSIGN_RSHIFT_INT(ASSIGN_OP, C_TYPE, _AP_W2, _AP_S2)		\
  template<int _AP_W, bool _AP_S>					\
  INLINE  ap_private<_AP_W,_AP_S> &operator ASSIGN_OP ( ap_private<_AP_W,_AP_S> &op, C_TYPE op2) { \
    op = op.operator >> (op2);					\
    return op;						\
  }
#define OP_ASSIGN_LSHIFT_INT(ASSIGN_OP, C_TYPE, _AP_W2, _AP_S2)		\
  template<int _AP_W, bool _AP_S>					\
  INLINE  ap_private<_AP_W,_AP_S> &operator ASSIGN_OP ( ap_private<_AP_W,_AP_S> &op, C_TYPE op2) { \
    op = op.operator << (op2);					\
    return op;						\
  }

#define OPS_MIX_INT(C_TYPE, WI, SI)		\
  OP_BIN_MIX_INT(*, C_TYPE, WI, SI, mult)	\
    OP_BIN_MIX_INT(+, C_TYPE, WI, SI, plus)	\
    OP_BIN_MIX_INT(-, C_TYPE, WI, SI, minus)	\
    OP_BIN_MIX_INT(/, C_TYPE, WI, SI, div)	\
    OP_BIN_MIX_INT(%, C_TYPE, WI, SI, mod)	\
    OP_BIN_MIX_INT(&, C_TYPE, WI, SI, logic)	\
    OP_BIN_MIX_INT(|, C_TYPE, WI, SI, logic)	\
    OP_BIN_MIX_INT(^, C_TYPE, WI, SI, logic)	\
    OP_BIN_SHIFT_INT(>>, C_TYPE, WI, SI, arg1)	\
    OP_BIN_SHIFT_INT(<<, C_TYPE, WI, SI, arg1)	\
						\
    OP_REL_MIX_INT(==, C_TYPE, WI, SI)		\
    OP_REL_MIX_INT(!=, C_TYPE, WI, SI)		\
    OP_REL_MIX_INT(>, C_TYPE, WI, SI)		\
    OP_REL_MIX_INT(>=, C_TYPE, WI, SI)		\
    OP_REL_MIX_INT(<, C_TYPE, WI, SI)		\
    OP_REL_MIX_INT(<=, C_TYPE, WI, SI)		\
						\
    OP_ASSIGN_MIX_INT(+=, C_TYPE, WI, SI)	\
    OP_ASSIGN_MIX_INT(-=, C_TYPE, WI, SI)	\
    OP_ASSIGN_MIX_INT(*=, C_TYPE, WI, SI)	\
    OP_ASSIGN_MIX_INT(/=, C_TYPE, WI, SI)	\
    OP_ASSIGN_MIX_INT(%=, C_TYPE, WI, SI)	\
    OP_ASSIGN_MIX_INT(&=, C_TYPE, WI, SI)	\
    OP_ASSIGN_MIX_INT(|=, C_TYPE, WI, SI)	\
    OP_ASSIGN_MIX_INT(^=, C_TYPE, WI, SI)	\
    OP_ASSIGN_RSHIFT_INT(>>=, C_TYPE, WI, SI)	\
    OP_ASSIGN_LSHIFT_INT(<<=, C_TYPE, WI, SI)	


OPS_MIX_INT(bool, 1, false)
OPS_MIX_INT(char, 8, true)
OPS_MIX_INT(signed char, 8, true)
OPS_MIX_INT(unsigned char, 8, false)
OPS_MIX_INT(short, 16, true)
OPS_MIX_INT(unsigned short, 16, false)
OPS_MIX_INT(int, 32, true)
OPS_MIX_INT(unsigned int, 32, false)
# if defined __x86_64__
OPS_MIX_INT(long, 64, true)
OPS_MIX_INT(unsigned long, 64, false)
# else
OPS_MIX_INT(long, 32, true)
OPS_MIX_INT(unsigned long, 32, false)
# endif
OPS_MIX_INT(ap_slong, 64, true)
OPS_MIX_INT(ap_ulong, 64, false)

#define OP_BIN_MIX_RANGE(BIN_OP, RTYPE) \
  template<int _AP_W1, bool _AP_S1, int _AP_W2, bool _AP_S2> \
  INLINE typename ap_private<_AP_W1,_AP_S1>::template RType<_AP_W2,_AP_S2>::RTYPE \
  operator BIN_OP ( const ap_range_ref<_AP_W1,_AP_S1>& op1, const ap_private<_AP_W2,_AP_S2>& op2) { \
      return ap_private<_AP_W1, false>(op1).operator BIN_OP (op2); \
  } \
  template<int _AP_W1, bool _AP_S1, int _AP_W2, bool _AP_S2> \
  INLINE typename ap_private<_AP_W1,_AP_S1>::template RType<_AP_W2,_AP_S2>::RTYPE \
  operator BIN_OP ( const ap_private<_AP_W1,_AP_S1>& op1, const ap_range_ref<_AP_W2,_AP_S2>& op2) { \
      return op1.operator BIN_OP (ap_private<_AP_W2, false>(op2)); \
  }

#define OP_REL_MIX_RANGE(REL_OP) \
  template<int _AP_W1, bool _AP_S1, int _AP_W2, bool _AP_S2>   \
  INLINE bool operator REL_OP ( const ap_range_ref<_AP_W1,_AP_S1>& op1, const ap_private<_AP_W2,_AP_S2>& op2) { \
    return ap_private<_AP_W1,false>(op1).operator REL_OP (op2); \
  } \
  template<int _AP_W1, bool _AP_S1, int _AP_W2, bool _AP_S2> \
  INLINE bool operator REL_OP ( const ap_private<_AP_W1,_AP_S1>& op1, const ap_range_ref<_AP_W2,_AP_S2>& op2) { \
    return op1.operator REL_OP (op2.operator ap_private<_AP_W2, false>()); \
  }

#define OP_ASSIGN_MIX_RANGE(ASSIGN_OP) \
  template<int _AP_W1, bool _AP_S1, int _AP_W2, bool _AP_S2> \
  INLINE ap_private<_AP_W1,_AP_S1>& operator ASSIGN_OP ( ap_private<_AP_W1,_AP_S1>& op1, const ap_range_ref<_AP_W2,_AP_S2>& op2) { \
    return op1.operator ASSIGN_OP (ap_private<_AP_W2, false>(op2)); \
  } \
  template<int _AP_W1, bool _AP_S1, int _AP_W2, bool _AP_S2> \
  INLINE ap_range_ref<_AP_W1,_AP_S1>& operator ASSIGN_OP (ap_range_ref<_AP_W1,_AP_S1>& op1, ap_private<_AP_W2,_AP_S2>& op2) { \
    ap_private<_AP_W1, false> tmp(op1); \
    tmp.operator ASSIGN_OP (op2); \
    op1 = tmp; \
    return op1; \
  }


OP_ASSIGN_MIX_RANGE(+=)
OP_ASSIGN_MIX_RANGE(-=)
OP_ASSIGN_MIX_RANGE(*=)
OP_ASSIGN_MIX_RANGE(/=)
OP_ASSIGN_MIX_RANGE(%=)
OP_ASSIGN_MIX_RANGE(>>=)
OP_ASSIGN_MIX_RANGE(<<=)
OP_ASSIGN_MIX_RANGE(&=)
OP_ASSIGN_MIX_RANGE(|=)
OP_ASSIGN_MIX_RANGE(^=)

OP_REL_MIX_RANGE(==)
OP_REL_MIX_RANGE(!=)
OP_REL_MIX_RANGE(>)
OP_REL_MIX_RANGE(>=)
OP_REL_MIX_RANGE(<)
OP_REL_MIX_RANGE(<=)

OP_BIN_MIX_RANGE(+, plus)
OP_BIN_MIX_RANGE(-, minus)
OP_BIN_MIX_RANGE(*, mult)
OP_BIN_MIX_RANGE(/, div)
OP_BIN_MIX_RANGE(%, mod)
OP_BIN_MIX_RANGE(>>, arg1)
OP_BIN_MIX_RANGE(<<, arg1)
OP_BIN_MIX_RANGE(&, logic)
OP_BIN_MIX_RANGE(|, logic)
OP_BIN_MIX_RANGE(^, logic)

#define OP_BIN_MIX_BIT(BIN_OP, RTYPE) \
  template<int _AP_W1, bool _AP_S1, int _AP_W2, bool _AP_S2> \
  INLINE typename ap_private<1, false>::template RType<_AP_W2,_AP_S2>::RTYPE \
  operator BIN_OP ( const ap_bit_ref<_AP_W1,_AP_S1>& op1, const ap_private<_AP_W2,_AP_S2>& op2) { \
      return ap_private<1, false>(op1).operator BIN_OP (op2); \
  } \
  template<int _AP_W1, bool _AP_S1, int _AP_W2, bool _AP_S2> \
  INLINE typename ap_private<_AP_W1,_AP_S1>::template RType<1,false>::RTYPE \
  operator BIN_OP ( const ap_private<_AP_W1,_AP_S1>& op1, const ap_bit_ref<_AP_W2,_AP_S2>& op2) { \
      return op1.operator BIN_OP (ap_private<1, false>(op2)); \
  }

#define OP_REL_MIX_BIT(REL_OP) \
  template<int _AP_W1, bool _AP_S1, int _AP_W2, bool _AP_S2>   \
  INLINE bool operator REL_OP ( const ap_bit_ref<_AP_W1,_AP_S1>& op1, const ap_private<_AP_W2,_AP_S2>& op2) { \
    return ap_private<_AP_W1,false>(op1).operator REL_OP (op2); \
  } \
  template<int _AP_W1, bool _AP_S1, int _AP_W2, bool _AP_S2> \
  INLINE bool operator REL_OP ( const ap_private<_AP_W1,_AP_S1>& op1, const ap_bit_ref<_AP_W2,_AP_S2>& op2) { \
    return op1.operator REL_OP (ap_private<1, false>(op2)); \
  }

#define OP_ASSIGN_MIX_BIT(ASSIGN_OP) \
  template<int _AP_W1, bool _AP_S1, int _AP_W2, bool _AP_S2> \
  INLINE ap_private<_AP_W1,_AP_S1>& operator ASSIGN_OP ( ap_private<_AP_W1,_AP_S1>& op1, ap_bit_ref<_AP_W2,_AP_S2>& op2) { \
    return op1.operator ASSIGN_OP (ap_private<1, false>(op2)); \
  } \
  template<int _AP_W1, bool _AP_S1, int _AP_W2, bool _AP_S2> \
  INLINE ap_bit_ref<_AP_W1,_AP_S1>& operator ASSIGN_OP ( ap_bit_ref<_AP_W1,_AP_S1>& op1, ap_private<_AP_W2,_AP_S2>& op2) { \
    ap_private<1, false> tmp(op1); \
    tmp.operator ASSIGN_OP (op2); \
    op1 = tmp; \
    return op1; \
  }


OP_ASSIGN_MIX_BIT(+=)
OP_ASSIGN_MIX_BIT(-=)
OP_ASSIGN_MIX_BIT(*=)
OP_ASSIGN_MIX_BIT(/=)
OP_ASSIGN_MIX_BIT(%=)
OP_ASSIGN_MIX_BIT(>>=)
OP_ASSIGN_MIX_BIT(<<=)
OP_ASSIGN_MIX_BIT(&=)
OP_ASSIGN_MIX_BIT(|=)
OP_ASSIGN_MIX_BIT(^=)

OP_REL_MIX_BIT(==)
OP_REL_MIX_BIT(!=)
OP_REL_MIX_BIT(>)
OP_REL_MIX_BIT(>=)
OP_REL_MIX_BIT(<)
OP_REL_MIX_BIT(<=)

OP_BIN_MIX_BIT(+, plus)
OP_BIN_MIX_BIT(-, minus)
OP_BIN_MIX_BIT(*, mult)
OP_BIN_MIX_BIT(/, div)
OP_BIN_MIX_BIT(%, mod)
OP_BIN_MIX_BIT(>>, arg1)
OP_BIN_MIX_BIT(<<, arg1)
OP_BIN_MIX_BIT(&, logic)
OP_BIN_MIX_BIT(|, logic)
OP_BIN_MIX_BIT(^, logic)

#define REF_REL_OP_MIX_INT(REL_OP, C_TYPE, _AP_W2, _AP_S2)  \
  template<int _AP_W, bool _AP_S>   \
  INLINE bool operator REL_OP ( const ap_range_ref<_AP_W,_AP_S> &op, C_TYPE op2) {  \
    return (ap_private<_AP_W, false>(op)).operator REL_OP (ap_private<_AP_W2,_AP_S2>(op2));  \
  }  \
  template<int _AP_W, bool _AP_S> \
  INLINE bool operator REL_OP ( C_TYPE op2, const ap_range_ref<_AP_W,_AP_S> &op) {  \
    return ap_private<_AP_W2,_AP_S2>(op2).operator REL_OP (ap_private<_AP_W, false>(op));  \
  } \
  template<int _AP_W, bool _AP_S>   \
  INLINE bool operator REL_OP ( const ap_bit_ref<_AP_W,_AP_S> &op, C_TYPE op2) {  \
    return (bool(op)) REL_OP op2;  \
  }  \
  template<int _AP_W, bool _AP_S> \
  INLINE bool operator REL_OP ( C_TYPE op2, const ap_bit_ref<_AP_W,_AP_S> &op) {  \
    return op2 REL_OP (bool(op));  \
  } \
  template<int _AP_W, typename _AP_T, int _AP_W1, typename _AP_T1>   \
  INLINE bool operator REL_OP ( const ap_concat_ref<_AP_W,_AP_T, _AP_W1, _AP_T1> &op, C_TYPE op2) {  \
    return (ap_private<_AP_W + _AP_W1, false>(op)).operator REL_OP (ap_private<_AP_W2,_AP_S2>(op2));  \
  }  \
  template<int _AP_W, typename _AP_T, int _AP_W1, typename _AP_T1>   \
  INLINE bool operator REL_OP ( C_TYPE op2, const ap_concat_ref<_AP_W,_AP_T, _AP_W1, _AP_T1> &op) {  \
    return ap_private<_AP_W2,_AP_S2>(op2).operator REL_OP (ap_private<_AP_W + _AP_W1, false>(op));  \
  }
 
#define REF_REL_MIX_INT(C_TYPE, _AP_WI, _AP_SI) \
REF_REL_OP_MIX_INT(>, C_TYPE, _AP_WI, _AP_SI) \
REF_REL_OP_MIX_INT(<, C_TYPE, _AP_WI, _AP_SI) \
REF_REL_OP_MIX_INT(>=, C_TYPE, _AP_WI, _AP_SI) \
REF_REL_OP_MIX_INT(<=, C_TYPE, _AP_WI, _AP_SI) \
REF_REL_OP_MIX_INT(==, C_TYPE, _AP_WI, _AP_SI) \
REF_REL_OP_MIX_INT(!=, C_TYPE, _AP_WI, _AP_SI) 

REF_REL_MIX_INT(bool, 1, false)
REF_REL_MIX_INT(char, 8, true)
REF_REL_MIX_INT(signed char, 8, true)
REF_REL_MIX_INT(unsigned char, 8, false)
REF_REL_MIX_INT(short, 16, true)
REF_REL_MIX_INT(unsigned short, 16, false)
REF_REL_MIX_INT(int, 32, true)
REF_REL_MIX_INT(unsigned int, 32, false)
# if defined __x86_64__
REF_REL_MIX_INT(long, 64, true)
REF_REL_MIX_INT(unsigned long, 64, false)
# else
REF_REL_MIX_INT(long, 32, true)
REF_REL_MIX_INT(unsigned long, 32, false)
# endif
REF_REL_MIX_INT(ap_slong, 64, true)
REF_REL_MIX_INT(ap_ulong, 64, false)

#define REF_BIN_OP_MIX_INT(BIN_OP, RTYPE, C_TYPE, _AP_W2, _AP_S2)  \
  template<int _AP_W, bool _AP_S>   \
  INLINE typename ap_private<_AP_W, false>::template RType<_AP_W2,_AP_S2>::RTYPE \
  operator BIN_OP ( const ap_range_ref<_AP_W,_AP_S> &op, C_TYPE op2) {  \
    return (ap_private<_AP_W, false>(op)).operator BIN_OP (ap_private<_AP_W2,_AP_S2>(op2));  \
  }  \
  template<int _AP_W, bool _AP_S> \
  INLINE typename ap_private<_AP_W2, _AP_S2>::template RType<_AP_W,false>::RTYPE \
  operator BIN_OP ( C_TYPE op2, const ap_range_ref<_AP_W,_AP_S> &op) {  \
    return ap_private<_AP_W2,_AP_S2>(op2).operator BIN_OP (ap_private<_AP_W, false>(op));  \
  }

#define REF_BIN_MIX_INT(C_TYPE, _AP_WI, _AP_SI) \
REF_BIN_OP_MIX_INT(+, plus, C_TYPE, _AP_WI, _AP_SI) \
REF_BIN_OP_MIX_INT(-, minus, C_TYPE, _AP_WI, _AP_SI) \
REF_BIN_OP_MIX_INT(*, mult, C_TYPE, _AP_WI, _AP_SI) \
REF_BIN_OP_MIX_INT(/, div, C_TYPE, _AP_WI, _AP_SI) \
REF_BIN_OP_MIX_INT(%, mod, C_TYPE, _AP_WI, _AP_SI) \
REF_BIN_OP_MIX_INT(>>, arg1, C_TYPE, _AP_WI, _AP_SI) \
REF_BIN_OP_MIX_INT(<<, arg1, C_TYPE, _AP_WI, _AP_SI) \
REF_BIN_OP_MIX_INT(&, logic, C_TYPE, _AP_WI, _AP_SI) \
REF_BIN_OP_MIX_INT(|, logic, C_TYPE, _AP_WI, _AP_SI) \
REF_BIN_OP_MIX_INT(^, logic, C_TYPE, _AP_WI, _AP_SI)

REF_BIN_MIX_INT(bool, 1, false)
REF_BIN_MIX_INT(char, 8, true)
REF_BIN_MIX_INT(signed char, 8, true)
REF_BIN_MIX_INT(unsigned char, 8, false)
REF_BIN_MIX_INT(short, 16, true)
REF_BIN_MIX_INT(unsigned short, 16, false)
REF_BIN_MIX_INT(int, 32, true)
REF_BIN_MIX_INT(unsigned int, 32, false)
# if defined __x86_64__
REF_BIN_MIX_INT(long, 64, true)
REF_BIN_MIX_INT(unsigned long, 64, false)
#else
REF_BIN_MIX_INT(long, 32, true)
REF_BIN_MIX_INT(unsigned long, 32, false)
#endif
REF_BIN_MIX_INT(ap_slong, 64, true)
REF_BIN_MIX_INT(ap_ulong, 64, false)

#define REF_BIN_OP(BIN_OP, RTYPE) \
template<int _AP_W, bool _AP_S, int _AP_W2, bool _AP_S2> \
INLINE typename ap_private<_AP_W, false>::template RType<_AP_W2, false>::RTYPE \
operator BIN_OP (const ap_range_ref<_AP_W,_AP_S> &lhs, const ap_range_ref<_AP_W2,_AP_S2> &rhs) {  \
  return ap_private<_AP_W,false>(lhs).operator BIN_OP (ap_private<_AP_W2, false>(rhs));  \
}

REF_BIN_OP(+, plus)
REF_BIN_OP(-, minus)
REF_BIN_OP(*, mult)
REF_BIN_OP(/, div)
REF_BIN_OP(%, mod)
REF_BIN_OP(>>, arg1)
REF_BIN_OP(<<, arg1)
REF_BIN_OP(&, logic)
REF_BIN_OP(|, logic)
REF_BIN_OP(^, logic)

#if 1
#define CONCAT_OP_MIX_INT(C_TYPE, _AP_WI, _AP_SI) \
template<int _AP_W, bool _AP_S> \
INLINE \
ap_private< _AP_W +  _AP_WI, false > \
  operator, (const ap_private<_AP_W, _AP_S> &op1, C_TYPE op2) { \
  ap_private<_AP_WI + _AP_W, false> val(op2); \
  ap_private<_AP_WI + _AP_W, false> ret(op1); \
  ret <<= _AP_WI; \
  if (_AP_SI) { \
      val <<= _AP_W; val >>= _AP_W; \
  }\
  ret |= val; \
  return ret;\
} \
template<int _AP_W, bool _AP_S> \
INLINE \
ap_private< _AP_W +  _AP_WI, false > \
  operator, (C_TYPE op1, const ap_private<_AP_W, _AP_S>& op2)  { \
  ap_private<_AP_WI + _AP_W, false> val(op1); \
  ap_private<_AP_WI + _AP_W, false> ret(op2); \
  if (_AP_S) { \
     ret <<= _AP_WI; ret >>= _AP_WI; \
  } \
  ret |= val << _AP_W; \
  return ret; \
} \
template<int _AP_W, bool _AP_S> \
INLINE \
ap_private< _AP_W +  _AP_WI, false > \
  operator, (const ap_range_ref<_AP_W, _AP_S> &op1, C_TYPE op2) { \
  ap_private<_AP_WI + _AP_W, false> val(op2); \
  ap_private<_AP_WI + _AP_W, false> ret(op1); \
  ret <<= _AP_WI; \
  if (_AP_SI) { \
      val <<= _AP_W; val >>= _AP_W; \
  } \
  ret |= val; \
  return ret; \
} \
template<int _AP_W, bool _AP_S> \
INLINE \
ap_private< _AP_W +  _AP_WI, false > \
  operator, (C_TYPE op1, const ap_range_ref<_AP_W, _AP_S> &op2)  { \
  ap_private<_AP_WI + _AP_W, false> val(op1); \
  ap_private<_AP_WI + _AP_W, false> ret(op2); \
  int len = op2.length(); \
  val <<= len; \
  ret |= val; \
  return ret; \
} \
template<int _AP_W, bool _AP_S> \
INLINE \
ap_private<_AP_WI + 1, false > \
  operator, (const ap_bit_ref<_AP_W, _AP_S> &op1, C_TYPE op2) { \
  ap_private<_AP_WI + 1, false> val(op2); \
  val[_AP_WI] = op1; \
  return val; \
} \
template<int _AP_W, bool _AP_S> \
INLINE \
ap_private<_AP_WI + 1, false > \
  operator, (C_TYPE op1, const ap_bit_ref<_AP_W, _AP_S> &op2)  { \
  ap_private<_AP_WI + 1, false> val(op1); \
  val <<= 1; \
  val[0] = op2; \
  return val; \
} \
template<int _AP_W, typename _AP_T, int _AP_W2, typename _AP_T2> \
INLINE \
ap_private<_AP_W + _AP_W2 + _AP_WI, false > \
   operator, (const ap_concat_ref<_AP_W, _AP_T, _AP_W2, _AP_T2> &op1, C_TYPE op2) {\
   ap_private<_AP_WI + _AP_W + _AP_W2, _AP_SI> val(op2);\
   ap_private<_AP_WI + _AP_W + _AP_W2, _AP_SI> ret(op1);\
   if (_AP_SI) { \
       val <<= _AP_W + _AP_W2; val >>= _AP_W + _AP_W2; \
   } \
   ret <<= _AP_WI; \
   ret |= val; \
   return ret; \
}\
template<int _AP_W, typename _AP_T, int _AP_W2, typename _AP_T2> \
INLINE \
ap_private<_AP_W + _AP_W2 + _AP_WI, false > \
   operator, (C_TYPE op1, const ap_concat_ref<_AP_W, _AP_T, _AP_W2, _AP_T2> &op2) {\
   ap_private<_AP_WI + _AP_W + _AP_W2, _AP_SI> val(op1);\
   ap_private<_AP_WI + _AP_W + _AP_W2, _AP_SI> ret(op2);\
   int len = op2.length(); \
   val <<= len; \
   ret |= val;\
   return ret; \
}\
template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, \
         int _AP_N > \
INLINE \
ap_private< _AP_W + _AP_WI, false > \
  operator, (const af_range_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, _AP_N> &op1, C_TYPE op2) { \
  ap_private<_AP_WI + _AP_W, false> val(op2); \
  ap_private<_AP_WI + _AP_W, false> ret(op1); \
  if (_AP_SI) { \
      val <<= _AP_W; val >>= _AP_W; \
  }\
  ret <<= _AP_WI; \
  ret |= val; \
  return ret; \
} \
template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, \
         int _AP_N > \
INLINE \
ap_private< _AP_W + _AP_WI, false > \
  operator, (C_TYPE op1, const af_range_ref<_AP_W, _AP_I, _AP_S, \
             _AP_Q, _AP_O, _AP_N> &op2) { \
  ap_private<_AP_WI + _AP_W, false> val(op1); \
  ap_private<_AP_WI + _AP_W, false> ret(op2); \
  int len = op2.length(); \
  val <<= len; \
  ret |= val; \
  return ret; \
} \
template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, \
         int _AP_N > \
INLINE \
ap_private< 1 + _AP_WI, false> \
  operator, (const af_bit_ref<_AP_W, _AP_I, _AP_S, _AP_Q, _AP_O, \
             _AP_N> &op1, C_TYPE op2) { \
  ap_private<_AP_WI + 1, _AP_SI> val(op2); \
  val[_AP_WI] = op1; \
  return val; \
} \
template<int _AP_W, int _AP_I, bool _AP_S, ap_q_mode _AP_Q, ap_o_mode _AP_O, \
         int _AP_N > \
INLINE \
ap_private< 1 + _AP_WI, false> \
  operator, (C_TYPE op1, const af_bit_ref<_AP_W, _AP_I, _AP_S, _AP_Q,\
        _AP_O, _AP_N> &op2) { \
  ap_private<_AP_WI + 1, _AP_SI> val(op1); \
  val <<= 1; \
  val[0] = op2; \
  return val; \
} 

CONCAT_OP_MIX_INT(bool, 1, false)
CONCAT_OP_MIX_INT(char, 8, true)
CONCAT_OP_MIX_INT(signed char, 8, true)
CONCAT_OP_MIX_INT(unsigned char, 8, false)
CONCAT_OP_MIX_INT(short, 16, true)
CONCAT_OP_MIX_INT(unsigned short, 16, false)
CONCAT_OP_MIX_INT(int, 32, true)
CONCAT_OP_MIX_INT(unsigned int, 32, false)
# if defined __x86_64__
CONCAT_OP_MIX_INT(long, 64, true)
CONCAT_OP_MIX_INT(unsigned long, 64, false)
# else
CONCAT_OP_MIX_INT(long, 32, true)
CONCAT_OP_MIX_INT(unsigned long, 32, false)
# endif
CONCAT_OP_MIX_INT(ap_slong, 64, true)
CONCAT_OP_MIX_INT(ap_ulong, 64, false)
#endif

#if 1
#define CONCAT_SHIFT_MIX_INT(C_TYPE, op) \
template<int _AP_W, typename _AP_T, int _AP_W1, typename _AP_T1>   \
INLINE ap_uint<_AP_W+_AP_W1> operator op (const ap_concat_ref<_AP_W, _AP_T, _AP_W1, _AP_T1> lhs, C_TYPE rhs) { \
  return ((ap_uint<_AP_W+_AP_W1>)lhs.get()) op ((int)rhs); \
}

CONCAT_SHIFT_MIX_INT(long, <<)
CONCAT_SHIFT_MIX_INT(unsigned long, <<)
CONCAT_SHIFT_MIX_INT(unsigned int, <<)
CONCAT_SHIFT_MIX_INT(ap_ulong, <<)
CONCAT_SHIFT_MIX_INT(ap_slong, <<)
CONCAT_SHIFT_MIX_INT(long, >>)
CONCAT_SHIFT_MIX_INT(unsigned long, >>)
CONCAT_SHIFT_MIX_INT(unsigned int, >>)
CONCAT_SHIFT_MIX_INT(ap_ulong, >>)
CONCAT_SHIFT_MIX_INT(ap_slong, >>)
#endif

#if defined(SYSTEMC_H) || defined(SYSTEMC_INCLUDED) 
template<int _AP_W, bool _AP_S>
INLINE void sc_trace(sc_core::sc_trace_file *tf, const ap_private<_AP_W, _AP_S> &op,
                     const std::string &name) {
    if (tf)
        tf->trace(sc_dt::sc_lv<_AP_W>(op.to_string(2).c_str()), name);
}
#endif

template<int _AP_W, bool _AP_S>
INLINE std::ostream& operator<<(std::ostream& out, const ap_private<_AP_W,_AP_S> &op)
{
  ap_private<_AP_W, _AP_S> v=op;
    const std::ios_base::fmtflags basefield = out.flags() & std::ios_base::basefield;
    unsigned radix = (basefield == std::ios_base::hex) ? 16 : 
                     ((basefield == std::ios_base::oct) ? 8 : 10);
    std::string str=v.toString(radix,_AP_S);
    out<<str;
    return out;

}

template<int _AP_W, bool _AP_S>
INLINE std::istream& operator >> (std::istream& in, ap_private<_AP_W,_AP_S> &op)
{
    std::string str;
    in >> str;
    const std::ios_base::fmtflags basefield = in.flags() & std::ios_base::basefield;
    unsigned radix = (basefield == std::ios_base::hex) ? 16 : 
                     ((basefield == std::ios_base::oct) ? 8 : 10);
    op = ap_private<_AP_W, _AP_S>(str.c_str(), radix);
    return in;

}

template<int _AP_W, bool _AP_S>
INLINE std::ostream& operator<<(std::ostream& out, const ap_range_ref<_AP_W,_AP_S> &op)
{
    return operator<<(out, ap_private<_AP_W, _AP_S>(op));
}

template<int _AP_W, bool _AP_S>
INLINE std::istream& operator >> (std::istream& in, ap_range_ref<_AP_W,_AP_S> &op)
{
    return operator>>(in, ap_private<_AP_W, _AP_S>(op));;
}

template<int _AP_W, bool _AP_S>
INLINE void print(const ap_private<_AP_W,_AP_S> &op, bool fill=true )
{
    ap_private<_AP_W, _AP_S> v=op;
    uint32_t ws=v.getNumWords();
    const uint64_t *ptr=v.getRawData();
    int i=ws-1;
//match SystemC output
    if(_AP_W%64 != 0) {
        uint32_t offset=_AP_W%64;
        uint32_t count=(offset+3)/4;
        int64_t data=*(ptr+i);
        if(_AP_S) 
            data=(data<<(64-offset))>>(64-offset);
        else
            count=(offset+4)/4;
        while(count-->0)
#if defined(__x86_64__) && !defined(__MINGW32__)
            printf("%lx",(data>>(count*4))&0xf);
#else
            printf("%llx",(data>>(count*4))&0xf);
#endif
    } else {
        if(_AP_S==false)
            printf("0");
#if defined(__x86_64__) && !defined(__MINGW32__)

        printf("%016lx",*(ptr+i));
#else
        printf("%016llx",*(ptr+i));
#endif
    }
    for(--i;i>=0;i--)
#if defined(__x86_64__) && !defined(__MINGW32__)
        printf("%016lx",*(ptr+i));
#else
        printf("%016llx",*(ptr+i));
#endif
    printf("\n");

}

#endif //__SYNTHESIS__

#endif //__cplusplus

#endif //__AESL_GCC_AP_INT_H__
