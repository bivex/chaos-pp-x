# /* ********************************************************************
#  *                                                                    *
#  *    (C) Copyright Paul Mensonides 2003-2005.                        *
#  *                                                                    *
#  *    Distributed under the Boost Software License, Version 1.0.      *
#  *    (See accompanying file LICENSE).                                *
#  *                                                                    *
#  *    See http://chaos-pp.sourceforge.net for most recent version.    *
#  *                                                                    *
#  ******************************************************************** */
#
# ifndef CHAOS_PREPROCESSOR_DEBUG_FAILURE_H
# define CHAOS_PREPROCESSOR_DEBUG_FAILURE_H
#
# include <chaos/preprocessor/config.h>
# include <chaos/preprocessor/lambda/ops.h>
#
# /* CHAOS_PP_FAILURE */
#
# if CHAOS_PP_VARIADICS
#    define CHAOS_PP_FAILURE(...) CHAOS_IP_FAILURE_I(__VA_ARGS__ !)
#    define CHAOS_PP_FAILURE_ CHAOS_PP_LAMBDA(CHAOS_PP_FAILURE)
# else
#    define CHAOS_PP_FAILURE() CHAOS_IP_FAILURE_I(!)
# endif
# define CHAOS_PP_FAILURE_ID() CHAOS_PP_FAILURE
#
# /* CHAOS_PP_FAILURE_MSG */
#
# if CHAOS_PP_VARIADICS
#    define CHAOS_PP_FAILURE_MSG(...) CHAOS_IP_FAILURE_I(__VA_ARGS__ !)
#    define CHAOS_PP_FAILURE_MSG_ CHAOS_PP_LAMBDA(CHAOS_PP_FAILURE_MSG)
# else
#    define CHAOS_PP_FAILURE_MSG(msg) CHAOS_IP_FAILURE_I(msg !)
# endif
# define CHAOS_PP_FAILURE_MSG_ID() CHAOS_PP_FAILURE_MSG
#
# define CHAOS_IP_FAILURE_I() CHAOS_IP_FAILURE_I
#
# endif
