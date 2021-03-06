/********************************************************************
	created:		2011/02/13
	filename: 		atomic_impl_byte.hpp
	author:			Tigran Avanesov
	description:	C++0x atomics implementation
*********************************************************************/

#ifndef ATOMIC_IMPL_BYTE_HPP_INCLUDED
#define ATOMIC_IMPL_BYTE_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "atomic_impl.hpp"

#ifdef __cplusplus_cli
# pragma managed(push, off)
#endif

namespace ATOMICS_NAMESPACE
{
	namespace detail
	{
		template <typename T> class atomic_impl<T, 1>
			: private atomic_impl_base<T>
		{
		public:
			// default constructor
			atomic_impl() {}
			// initialization constructor
			explicit atomic_impl(T val): atomic_impl_base(val) {}
			// store into atomic variable
			void store(T val, memory_order mo)
			{
				_ReadWriteBarrier();
				if (mo == memory_order_seq_cst)
				{
					__asm mov al, val;
					__asm mov edi, this
					__asm lock xchg [edi]impl_base_type.var, al;
				} else
					var = val;
				_ReadWriteBarrier();
			}
			// load from atomic variable
			T load(memory_order) const
			{
				_ReadWriteBarrier();
				const T value(var);
				_ReadWriteBarrier();
				return value;
			}
			// increment atomic variable
			T fetch_add(T val, memory_order)
			{
				T old_val;
				_ReadWriteBarrier();
				__asm mov al, val;
				__asm mov edi, this
				__asm lock xadd [edi]impl_base_type.var, al;
				__asm mov old_val, al;
				_ReadWriteBarrier();
				return old_val;
			}
			// swap atomic variable's value
			T exchange(T val, memory_order)
			{
				T old_val;
				_ReadWriteBarrier();
				__asm mov al, val;
				__asm mov edi, this
				__asm lock xchg [edi]impl_base_type.var, al;
				__asm mov old_val, al;
				_ReadWriteBarrier();
				return old_val;
			}
			// compare and swap atomic variable's value on success
			bool compare_exchange(T& exp, T val, memory_order, memory_order)
			{
				unsigned char res;
				_ReadWriteBarrier();
				__asm mov bl, val;
				__asm mov edi, this
				__asm mov esi, exp;
				__asm mov al, byte ptr [esi];
				__asm lock cmpxchg [edi]impl_base_type.var, bl;
				__asm setz res;
				__asm je success;
				__asm mov byte ptr [esi], al;
				__asm success:
				_ReadWriteBarrier();
				return res != 0;
			}
			// implementation is lock-free
			enum { is_lock_free = 1 };
		};
	}
}

#ifdef __cplusplus_cli
# pragma managed(pop)
#endif

#endif // ATOMIC_IMPL_BYTE_HPP_INCLUDED
