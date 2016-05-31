//
// Copyright (C) 2011-14 Mark Wiebe, DyND Developers
// BSD 2-Clause License, see LICENSE.txt
//

#ifndef _DYND__UNARY_ELWISE_EVAL_HPP_
#define _DYND__UNARY_ELWISE_EVAL_HPP_

#include <deque>

#include <dynd/eval/eval_context.hpp>

namespace dynd { namespace eval {

/**
 * Evaluates a node which is purely unary elementwise.
 */
ndarray_node_ptr evaluate_unary_elwise_array(ndarray_node* node, const eval::eval_context *ectx, bool copy, uint32_t access_flags);

/**
 * Applies the unary kernel to the input strided array node.
 */
ndarray_node_ptr evaluate_strided_with_unary_kernel(ndarray_node *node, const eval::eval_context *ectx,
                                bool copy, uint32_t access_flags,
                                const ndt::type& dst_tp, kernel_instance<unary_operation_pair_t>& operation);

/**
 * If the node is just a chain of unary operations, pushes all of the
 * kernels needed in order to evaluate it. Returns the strided array
 * leaf node.
 */
ndarray_node *push_front_node_unary_kernels(ndarray_node* node,
                    const eval::eval_context *ectx,
                    std::deque<kernel_instance<unary_operation_pair_t>>& out_kernels,
                    std::deque<intptr_t>& out_element_sizes);

}} // namespace dynd::eval

#endif // _DYND__UNARY_ELWISE_EVAL_HPP_
