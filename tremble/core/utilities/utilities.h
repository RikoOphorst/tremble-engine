#pragma once
#include "debug.h"
#include "types.h"

namespace tremble
{
	/**
	* @brief Just read the function name, come on man..
	* @param[in] num_to_round The number to round
	* @param[in] multiple The multiple to round to
	*/
	int RoundUpToMultiple(int num_to_round, int multiple);

	/**
	* @def HAS_MEM_FUNC(func, name)
	* @brief Creates a function that allows to check whether a class has a member function implemented
	* http://stackoverflow.com/questions/257288/is-it-possible-to-write-a-c-template-to-check-for-a-functions-existence
	*/
#define HAS_MEM_FUNC(func, name)                                        \
    template<typename T, typename Sign>                                 \
    struct name {                                                       \
        typedef char yes[1];                                            \
        typedef char no [2];                                            \
        template <typename U, U> struct type_check;                     \
        template <typename _1> static yes &chk(type_check<Sign, &_1::func > *); \
        template <typename   > static no  &chk(...);                    \
        static bool const value = sizeof(chk<T>(0)) == sizeof(yes);     \
    }																	\
//
//	/**
//	* @def HAS_MEM_FUNC(func, name)
//	* @brief Creates a function that allows to check whether a class has a member function implemented
//	* http://stackoverflow.com/questions/257288/is-it-possible-to-write-a-c-template-to-check-for-a-functions-existence
//	*/
//#define HAS_MEM_FUNC_WO_SIGN(func, name)                                        \
//    template<typename T>                                 \
//    struct name {                                                       \
//        typedef char yes[1];                                            \
//        typedef char no [2];                                            \
//        template <U, U> struct type_check;                     \
//        template <typename _1> static yes &chk(type_check<&_1::func, &_1::func > *); \
//        template <typename   > static no  &chk(...);                    \
//        static bool const value = sizeof(chk<T>(0)) == sizeof(yes);     \
//    }	
}