#ifndef STREE_MACROS_HPP_
#define STREE_MACROS_HPP_

#include <stree/config.hpp>
#include <boost/preprocessor/list/for_each.hpp>
#include <boost/preprocessor/tuple/to_list.hpp>

// Common helper macros

// Wrapper for BOOST_PP_*_FOR_EACH "callback" macro
#define STREE_FOR_EACH_WRAPPER(_r, MACRO, _elem)    \
    MACRO(_elem)

// Wrapper for BOOST_PP_*_FOR_EACH_I "callback" macro
#define STREE_FOR_EACH_WRAPPER_I(_r, MACRO, _idx, _elem)    \
    MACRO(_idx, _elem)

// List of function arities
#ifndef STREE_FUN_ARITY_TUPLE
# define STREE_FUN_ARITY_TUPLE (0, 1, 2, 3)
#endif

// List of select arities
#ifndef STREE_SELECT_ARITY_TUPLE
# define STREE_SELECT_ARITY_TUPLE (0, 1, 2)
#endif

// Arity width in bits (packed node)
#ifndef STREE_ARITY_WIDTH
# define STREE_ARITY_WIDTH (3)
#endif

// Value type
#ifndef STREE_VALUE_TYPE
# define STREE_VALUE_TYPE float;
#endif

// Data pointer type
#ifndef STREE_DATA_PTR_TYPE
# define STREE_DATA_PTR_TYPE void*
#endif

// Helper macros

// Apply MACRO to each value in STREE_FUN_ARITY_TUPLE:
// MACRO(arity)
#define STREE_FOR_EACH_FUN_ARITY(MACRO)                                 \
    BOOST_PP_LIST_FOR_EACH(                                             \
        STREE_FOR_EACH_WRAPPER, MACRO, BOOST_PP_TUPLE_TO_LIST(STREE_FUN_ARITY_TUPLE))


// Apply MACRO to each index and value in ARITY_TUPLE:
// MACRO(index, arity)
//
// May be used to map arity values stored in Id class to actual arity,
// reducing required number of bits: O(log(MaxArity)) to O(log(ArityTupleSize))
#define STREE_FOR_EACH_FUN_ARITY_I(MACRO)                               \
    BOOST_PP_LIST_FOR_EACH_I(                                           \
        STREE_FOR_EACH_WRAPPER_I, MACRO, BOOST_PP_TUPLE_TO_LIST(STREE_FUN_ARITY_TUPLE))


// Same for selects

#define STREE_FOR_EACH_SELECT_ARITY(MACRO)                              \
    BOOST_PP_LIST_FOR_EACH(                                             \
        STREE_FOR_EACH_WRAPPER, MACRO, BOOST_PP_TUPLE_TO_LIST(STREE_SELECT_ARITY_TUPLE))

#define STREE_FOR_EACH_SELECT_ARITY_I(MACRO)                            \
    BOOST_PP_LIST_FOR_EACH_I(                                           \
        STREE_FOR_EACH_WRAPPER_I, MACRO, BOOST_PP_TUPLE_TO_LIST(STREE_SELECT_ARITY_TUPLE))

#endif
