#ifndef STREE_MACROS_HPP_
#define STREE_MACROS_HPP_

// Common helper macros

// Wrapper for BOOST_PP_*_FOR_EACH "callback" macro
#define STREE_FOR_EACH_WRAPPER(_r, MACRO, _elem)    \
    MACRO(_elem)

// Wrapper for BOOST_PP_*_FOR_EACH_I "callback" macro
#define STREE_FOR_EACH_WRAPPER_I(_r, MACRO, _idx, _elem)    \
    MACRO(_idx, _elem)

#endif
