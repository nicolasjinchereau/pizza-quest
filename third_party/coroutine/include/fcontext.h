
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

////////////////////////////////////////////////////////////////
// this file contains the win32 version fcontext extracted from
// the boost library, and modified to remove boost dependancies.

#pragma once
#include <cstdint>

#pragma warning(push)
#pragma warning(disable:4351)

struct stack_t
{
	void *sp;
	size_t size;
	void *limit;
	stack_t() : sp(0), size(0), limit(0){}
};

struct fp_t
{
	uint32_t fc_freg[2];
	fp_t() : fc_freg(){}
};

struct fcontext_t
{
	uint32_t fc_greg[6];
	stack_t fc_stack;
	void *fc_excpt_lst;
	void *fc_local_storage;
	fp_t fc_fp;
	uint32_t fc_dealloc;

	fcontext_t() :
		fc_greg(),
		fc_stack(),
		fc_excpt_lst(0),
		fc_local_storage(0),
		fc_fp(),
		fc_dealloc(0){}
};

extern "C"
{

intptr_t __cdecl jump_fcontext( fcontext_t * ofc, fcontext_t const* nfc, intptr_t vp, bool preserve_fpu = true);
fcontext_t * __cdecl make_fcontext( void * sp, size_t size, void (* fn)( intptr_t) );

}

#pragma warning(pop)
