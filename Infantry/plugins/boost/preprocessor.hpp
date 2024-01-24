# /* Copyright (C) 2001
#  * Housemarque Oy
#  * http://www.housemarque.com
#  *
#  * Distributed under the Boost Software License, Version 1.0. (See
#  * accompanying file LICENSE_1_0.txt or copy at
#  * http://www.boost.org/LICENSE_1_0.txt)
#  */
#
# /* Revised by Paul Mensonides (2002)  */
#
# /* See http://www.boost.org/libs/preprocessor for documentation. */
#
# pragma onece
#
# include <preprocessor/library.hpp>

#define GENERATE_TWO_D_LIST_INNER(index, data, elem) \
BOOST_PP_IF(BOOST_PP_SUB(index, data), BOOST_PP_COMMA, BOOST_PP_EMPTY)() elem

#define GENERATE_TWO_D_LIST_OUTER(index, data, elem) \
BOOST_PP_IF(BOOST_PP_SUB(index, 1), BOOST_PP_COMMA, BOOST_PP_EMPTY)() \
{                                      \
    BOOST_PP_LIST_FOR_EACH(GENERATE_TWO_D_LIST_INNER, BOOST_PP_ADD(index, 1), BOOST_PP_TUPLE_TO_LIST(elem)) \
}
#define GENERATE_TWO_D_LIST(...) \
{BOOST_PP_SEQ_FOR_EACH(GENERATE_TWO_D_LIST_OUTER, "NAME", BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))}

#define GENERATE_ONE_D_LIST_INNER(index, data, elem) \
BOOST_PP_IF(BOOST_PP_SUB(index, 1), BOOST_PP_COMMA, BOOST_PP_EMPTY)() elem

#define GENERATE_ONE_D_LIST(...) \
{BOOST_PP_SEQ_FOR_EACH(GENERATE_ONE_D_LIST_INNER, "NAME", BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))}


