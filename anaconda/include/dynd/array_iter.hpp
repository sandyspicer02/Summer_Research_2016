//
// Copyright (C) 2011-14 Mark Wiebe, Irwin Zaid, DyND Developers
// BSD 2-Clause License, see LICENSE.txt
//

#ifndef _NDOBJECT_ITER_HPP_
#define _NDOBJECT_ITER_HPP_

#include <algorithm>

#include <dynd/array.hpp>
#include <dynd/shape_tools.hpp>

namespace dynd {

namespace detail {
    /** A simple metaprogram to indicate whether a value is within the bounds or not */
    template<int V, int V_START, int V_END>
    struct is_value_within_bounds {
        enum {value = (V >= V_START) && V < V_END};
    };
}

template<int Nwrite, int Nread>
class array_iter;

template<int Nwrite, int Nread>
class array_neighborhood_iter;

template<>
class array_iter<1, 0> {
    intptr_t m_itersize;
    size_t m_iter_ndim;
    dimvector m_iterindex;
    dimvector m_itershape;
    char *m_data;
    const char *m_arrmeta;
    iterdata_common *m_iterdata;
    ndt::type m_array_tp, m_uniform_tp;

    inline void init(const ndt::type& tp0, const char *arrmeta0, char *data0)
    {
        m_array_tp = tp0;
        m_iter_ndim = m_array_tp.get_ndim();
        m_itersize = 1;
        if (m_iter_ndim != 0) {
            m_iterindex.init(m_iter_ndim);
            memset(m_iterindex.get(), 0, sizeof(intptr_t) * m_iter_ndim);
            m_itershape.init(m_iter_ndim);
            m_array_tp.extended()->get_shape(m_iter_ndim, 0, m_itershape.get(), arrmeta0, NULL);

            size_t iterdata_size = m_array_tp.extended()->get_iterdata_size(m_iter_ndim);
            m_iterdata = reinterpret_cast<iterdata_common *>(malloc(iterdata_size));
            if (!m_iterdata) {
                throw std::bad_alloc();
            }
            m_arrmeta = arrmeta0;
            m_array_tp.iterdata_construct(m_iterdata,
                            &m_arrmeta, m_iter_ndim, m_itershape.get(), m_uniform_tp);
            m_data = m_iterdata->reset(m_iterdata, data0, m_iter_ndim);

            for (size_t i = 0, i_end = m_iter_ndim; i != i_end; ++i) {
                m_itersize *= m_itershape[i];
            }
        } else {
            m_iterdata = NULL;
            m_uniform_tp = m_array_tp;
            m_data = data0;
            m_arrmeta = arrmeta0;
        }
    }
public:
    array_iter(const nd::array& op0) {
        init(op0.get_type(), op0.get_arrmeta(), op0.get_readwrite_originptr());
    }

    ~array_iter() {
        if (m_iterdata) {
            m_array_tp.extended()->iterdata_destruct(m_iterdata, m_iter_ndim);
            free(m_iterdata);
        }
    }

    size_t itersize() const {
        return m_itersize;
    }

    bool empty() const {
        return m_itersize == 0;
    }

    bool next() {
        size_t i = m_iter_ndim;
        if (i != 0) {
            do {
                --i;
                if (++m_iterindex[i] != m_itershape[i]) {
                    m_data = m_iterdata->incr(m_iterdata, m_iter_ndim - i - 1);
                    return true;
                } else {
                    m_iterindex[i] = 0;
                }
            } while (i != 0);
        }

        return false;
    }

    char *data() {
        return m_data;
    }

    const char *arrmeta() {
        return m_arrmeta;
    }

    const ndt::type& get_uniform_dtype() const {
        return m_uniform_tp;
    }
};

template<>
class array_iter<0, 1> {
    intptr_t m_itersize;
    size_t m_iter_ndim;
    dimvector m_iterindex;
    dimvector m_itershape;
    const char *m_data;
    const char *m_arrmeta;
    iterdata_common *m_iterdata;
    ndt::type m_array_tp, m_uniform_tp;

    inline void init(const ndt::type& tp0, const char *arrmeta0, const char *data0, size_t ndim)
    {
        m_array_tp = tp0;
        m_iter_ndim = ndim ? ndim : m_array_tp.get_ndim();
        m_itersize = 1;
        if (m_iter_ndim != 0) {
            m_iterindex.init(m_iter_ndim);
            memset(m_iterindex.get(), 0, sizeof(intptr_t) * m_iter_ndim);
            m_itershape.init(m_iter_ndim);
            m_array_tp.extended()->get_shape(m_iter_ndim, 0, m_itershape.get(), arrmeta0, NULL);

            size_t iterdata_size = m_array_tp.extended()->get_iterdata_size(m_iter_ndim);
            m_iterdata = reinterpret_cast<iterdata_common *>(malloc(iterdata_size));
            if (!m_iterdata) {
                throw std::bad_alloc();
            }
            m_arrmeta = arrmeta0;
            m_array_tp.iterdata_construct(m_iterdata,
                            &m_arrmeta, m_iter_ndim, m_itershape.get(), m_uniform_tp);
            m_data = m_iterdata->reset(m_iterdata, const_cast<char *>(data0), m_iter_ndim);

            for (size_t i = 0, i_end = m_iter_ndim; i != i_end; ++i) {
                m_itersize *= m_itershape[i];
            }
        } else {
            m_iterdata = NULL;
            m_uniform_tp = m_array_tp;
            m_data = data0;
            m_arrmeta = arrmeta0;
        }
    }

public:
    array_iter(const ndt::type& tp0, const char *arrmeta0, const char *data0, size_t ndim = 0) {
        init(tp0, arrmeta0, data0, ndim);
    }

    array_iter(const nd::array& op0) {
        init(op0.get_type(), op0.get_arrmeta(), op0.get_readonly_originptr(), 0);
    }

    ~array_iter() {
        if (m_iterdata) {
            m_array_tp.extended()->iterdata_destruct(m_iterdata, m_iter_ndim);
            free(m_iterdata);
        }
    }

    size_t itersize() const {
        return m_itersize;
    }

    bool empty() const {
        return m_itersize == 0;
    }

    bool next() {
        size_t i = m_iter_ndim;
        if (i != 0) {
            do {
                --i;
                if (++m_iterindex[i] != m_itershape[i]) {
                    m_data = m_iterdata->incr(m_iterdata, m_iter_ndim - i - 1);
                    return true;
                } else {
                    m_iterindex[i] = 0;
                }
            } while (i != 0);
        }

        return false;
    }

    const char *data() {
        return m_data;
    }

    const char *arrmeta() {
        return m_arrmeta;
    }

    const ndt::type& get_uniform_dtype() const {
        return m_uniform_tp;
    }
};

template<>
class array_iter<1, 1> {
protected:
    intptr_t m_itersize;
    size_t m_iter_ndim[2];
    dimvector m_iterindex;
    dimvector m_itershape;
    char *m_data[2];
    const char *m_arrmeta[2];
    iterdata_common *m_iterdata[2];
    ndt::type m_array_tp[2], m_uniform_tp[2];

    array_iter() {
        m_iterdata[0] = NULL;
        m_iterdata[1] = NULL;
        m_data[0] = NULL;
        m_data[1] = NULL;
        m_arrmeta[0] = NULL;
        m_arrmeta[1] = NULL;
    }

    inline void init(const ndt::type& tp0, const char *arrmeta0, char *data0,
                    const ndt::type& tp1, const char *arrmeta1, const char *data1)
    {
        m_array_tp[0] = tp0;
        m_array_tp[1] = tp1;
        m_itersize = 1;
        // The destination shape
        m_iter_ndim[0] = m_array_tp[0].get_ndim();
        m_itershape.init(m_iter_ndim[0]);
        if (m_iter_ndim[0] > 0) {
            m_array_tp[0].extended()->get_shape(m_iter_ndim[0], 0, m_itershape.get(), arrmeta0, NULL);
        }
        // The source shape
        dimvector src_shape;
        m_iter_ndim[1] = m_array_tp[1].get_ndim();
        src_shape.init(m_iter_ndim[1]);
        if (m_iter_ndim[1] > 0) {
            m_array_tp[1].extended()->get_shape(m_iter_ndim[1], 0, src_shape.get(), arrmeta1, NULL);
        }
        // Check that the source shape broadcasts ok
        if (!shape_can_broadcast(m_iter_ndim[0], m_itershape.get(),
                        m_iter_ndim[1], src_shape.get())) {
            throw broadcast_error(m_iter_ndim[0], m_itershape.get(),
                            m_iter_ndim[1], src_shape.get());
        }
        // Allocate and initialize the iterdata
        if (m_iter_ndim[0] != 0) {
            m_iterindex.init(m_iter_ndim[0]);
            memset(m_iterindex.get(), 0, sizeof(intptr_t) * m_iter_ndim[0]);
            // The destination iterdata
            size_t iterdata_size = m_array_tp[0].get_iterdata_size(m_iter_ndim[0]);
            m_iterdata[0] = reinterpret_cast<iterdata_common *>(malloc(iterdata_size));
            if (!m_iterdata[0]) {
                throw std::bad_alloc();
            }
            m_arrmeta[0] = arrmeta0;
            m_array_tp[0].iterdata_construct(m_iterdata[0],
                            &m_arrmeta[0], m_iter_ndim[0], m_itershape.get(), m_uniform_tp[0]);
            m_data[0] = m_iterdata[0]->reset(m_iterdata[0], data0, m_iter_ndim[0]);
            // The source iterdata
            iterdata_size = m_array_tp[1].get_broadcasted_iterdata_size(m_iter_ndim[1]);
            m_iterdata[1] = reinterpret_cast<iterdata_common *>(malloc(iterdata_size));
            if (!m_iterdata[1]) {
                throw std::bad_alloc();
            }
            m_arrmeta[1] = arrmeta1;
            m_array_tp[1].broadcasted_iterdata_construct(m_iterdata[1],
                            &m_arrmeta[1], m_iter_ndim[1],
                            m_itershape.get() + (m_iter_ndim[0] - m_iter_ndim[1]), m_uniform_tp[1]);
            m_data[1] = m_iterdata[1]->reset(m_iterdata[1], const_cast<char *>(data1), m_iter_ndim[0]);

            for (size_t i = 0, i_end = m_iter_ndim[0]; i != i_end; ++i) {
                m_itersize *= m_itershape[i];
            }
        } else {
            m_iterdata[0] = NULL;
            m_iterdata[1] = NULL;
            m_uniform_tp[0] = m_array_tp[0];
            m_uniform_tp[1] = m_array_tp[1];
            m_data[0] = data0;
            m_data[1] = const_cast<char *>(data1);
            m_arrmeta[0] = arrmeta0;
            m_arrmeta[1] = arrmeta1;
        }
    }
public:
    array_iter(const ndt::type& tp0, const char *arrmeta0, char *data0,
                    const ndt::type& tp1, const char *arrmeta1, const char *data1) {
        init(tp0, arrmeta0, data0, tp1, arrmeta1, data1);
    }
    array_iter(const nd::array& op0, const nd::array& op1) {
        init(op0.get_type(), op0.get_arrmeta(), op0.get_readwrite_originptr(),
                        op1.get_type(), op1.get_arrmeta(), op1.get_readonly_originptr());
    }

    ~array_iter() {
        if (m_iterdata[0]) {
            m_array_tp[0].iterdata_destruct(m_iterdata[0], m_iter_ndim[0]);
            free(m_iterdata[0]);
        }
        if (m_iterdata[1]) {
            m_array_tp[1].iterdata_destruct(m_iterdata[1], m_iter_ndim[1]);
            free(m_iterdata[1]);
        }
    }

    size_t itersize() const {
        return m_itersize;
    }

    bool empty() const {
        return m_itersize == 0;
    }

    bool next() {
        size_t i = m_iter_ndim[0];
        if (i != 0) {
            do {
                --i;
                if (++m_iterindex[i] != m_itershape[i]) {
                    m_data[0] = m_iterdata[0]->incr(m_iterdata[0], m_iter_ndim[0] - i - 1);
                    m_data[1] = m_iterdata[1]->incr(m_iterdata[1], m_iter_ndim[0] - i - 1);
                    return true;
                } else {
                    m_iterindex[i] = 0;
                }
            } while (i != 0);
        }

        return false;
    }

    const intptr_t *index() const {
        return m_iterindex.get();
    }

    /**
     * Provide non-const access to the 'write' operands.
     */
    template<int K>
    inline typename enable_if<detail::is_value_within_bounds<K, 0, 1>::value, char *>::type data() {
        return m_data[K];
    }

    /**
     * Provide const access to all the operands.
     */
    template<int K>
    inline typename enable_if<detail::is_value_within_bounds<K, 0, 2>::value, const char *>::type data() const {
        return m_data[K];
    }

    template<int K>
    inline typename enable_if<detail::is_value_within_bounds<K, 0, 2>::value, const char *>::type arrmeta() const {
        return m_arrmeta[K];
    }

    template<int K>
    inline typename enable_if<detail::is_value_within_bounds<K, 0, 2>::value, const ndt::type&>::type get_uniform_dtype() const {
        return m_uniform_tp[K];
    }
};

template<>
class array_neighborhood_iter<1, 1> : public array_iter<1, 1> {
    dimvector m_neighbor_rel_iterindex;
    dimvector m_neighbor_iterindex;
    dimvector m_neighborhood_iteroffset;
    dimvector m_neighborhood_itershape;
    bool m_neighbor_within_bounds;
    char *m_origin_data[2];
    char *m_neighbor_data[2];
    iterdata_common *m_neighborhood_iterdata[2];

    inline void init(const ndt::type& tp0, const char *arrmeta0, char *data0,
                    const ndt::type& tp1, const char *arrmeta1, const char *data1,
                    const intptr_t *neighborhood_shape, const intptr_t *neighborhood_offset)
    {
        array_iter<1, 1>::init(tp0, arrmeta0, data0, tp1, arrmeta1, data1);

        if (m_iter_ndim[0] != 0) {
            m_neighbor_rel_iterindex.init(m_iter_ndim[0]);
            memset(m_neighbor_rel_iterindex.get(), 0, sizeof(intptr_t) * m_iter_ndim[0]);
            m_neighbor_iterindex.init(m_iter_ndim[0]);
            memset(m_neighbor_iterindex.get(), 0, sizeof(intptr_t) * m_iter_ndim[0]);
            m_neighborhood_itershape.init(m_iter_ndim[0], neighborhood_shape);
            if (neighborhood_offset == NULL) {
                m_neighborhood_iteroffset.init(m_iter_ndim[0]);
                memset(m_neighborhood_iteroffset.get(), 0, sizeof(intptr_t) * m_iter_ndim[0]);
            } else {
                m_neighborhood_iteroffset.init(m_iter_ndim[0], neighborhood_offset);
            }

            size_t iterdata_size = m_array_tp[0].extended()->get_iterdata_size(m_iter_ndim[0]);
            m_neighborhood_iterdata[0] = reinterpret_cast<iterdata_common *>(malloc(iterdata_size));
            if (!m_neighborhood_iterdata[0]) {
                throw std::bad_alloc();
            }
            m_array_tp[0].iterdata_construct(m_neighborhood_iterdata[0],
                            &arrmeta0, m_iter_ndim[0], m_itershape.get(), m_uniform_tp[0]);
            m_origin_data[0] = m_neighborhood_iterdata[0]->reset(m_neighborhood_iterdata[0], const_cast<char *>(data0), m_iter_ndim[0]);
            m_neighbor_data[0] = m_origin_data[0];
            iterdata_size = m_array_tp[1].get_broadcasted_iterdata_size(m_iter_ndim[1]);
            m_neighborhood_iterdata[1] = reinterpret_cast<iterdata_common *>(malloc(iterdata_size));
            if (!m_neighborhood_iterdata[1]) {
                throw std::bad_alloc();
            }
            m_array_tp[1].broadcasted_iterdata_construct(m_neighborhood_iterdata[1],
                            &arrmeta1, m_iter_ndim[1],
                            m_itershape.get() + (m_iter_ndim[0] - m_iter_ndim[1]), m_uniform_tp[1]);
            m_origin_data[1] = m_neighborhood_iterdata[1]->reset(m_neighborhood_iterdata[1], const_cast<char *>(data1), m_iter_ndim[0]);
            m_neighbor_data[1] = m_origin_data[1];

            m_neighbor_within_bounds = true;
            for (size_t j = 0; (j != m_iter_ndim[0]) && m_neighbor_within_bounds; ++j) {
                m_neighbor_iterindex[j] = m_iterindex[j] + m_neighborhood_iteroffset[j];
                if ((m_neighbor_iterindex[j] < 0) || (m_neighbor_iterindex[j] >= m_itershape[j])) {
                    m_neighbor_within_bounds = false;
                }
            }
        } else {
            m_neighborhood_iterdata[0] = NULL;
            m_neighborhood_iterdata[1] = NULL;
            m_origin_data[0] = data0;
            m_origin_data[1] = const_cast<char *>(data1);
            m_neighbor_data[0] = m_origin_data[0];
            m_neighbor_data[1] = m_origin_data[1];
        }
    }

public:
    array_neighborhood_iter(const ndt::type& tp0, const char *arrmeta0, char *data0,
                    const ndt::type& tp1, const char *arrmeta1, const char *data1,
                    const intptr_t *neighborhood_shape, const intptr_t *neighborhood_offset = NULL) {
        init(tp0, arrmeta0, data0, tp1, arrmeta1, data1, neighborhood_shape, neighborhood_offset);
    }
    array_neighborhood_iter(const nd::array& op0, const nd::array& op1, const intptr_t *neighborhood_shape,
                    const intptr_t *neighborhood_offset = NULL) {
        init(op0.get_type(), op0.get_arrmeta(), op0.get_readwrite_originptr(),
                        op1.get_type(), op1.get_arrmeta(), op1.get_readonly_originptr(),
                        neighborhood_shape, neighborhood_offset);
    }

    ~array_neighborhood_iter() {
        if (m_neighborhood_iterdata[0]) {
            m_array_tp[0].iterdata_destruct(m_neighborhood_iterdata[0], m_iter_ndim[0]);
            free(m_neighborhood_iterdata[0]);
        }
        if (m_neighborhood_iterdata[1]) {
            m_array_tp[1].iterdata_destruct(m_neighborhood_iterdata[1], m_iter_ndim[1]);
            free(m_neighborhood_iterdata[1]);
        }
    }

    bool next() {
        if (array_iter<1, 1>::next()) {
            memset(m_neighbor_iterindex.get(), 0, sizeof(intptr_t) * m_iter_ndim[0]);
            memset(m_neighbor_rel_iterindex.get(), 0, sizeof(intptr_t) * m_iter_ndim[0]);
            m_neighborhood_iterdata[0]->reset(m_neighborhood_iterdata[0], const_cast<char *>(m_origin_data[0]), m_iter_ndim[0]);
            m_neighborhood_iterdata[1]->reset(m_neighborhood_iterdata[1], const_cast<char *>(m_origin_data[1]), m_iter_ndim[0]);

            size_t i = 0;
            m_neighbor_within_bounds = true;
            while (i != m_iter_ndim[0]) {
                m_neighbor_iterindex[i] = m_iterindex[i] + m_neighborhood_iteroffset[i];
                if ((m_neighbor_iterindex[i] < 0) || (m_neighbor_iterindex[i] >= m_itershape[i])) {
                    m_neighbor_within_bounds = false;
                } else {
                    m_neighbor_data[0] = m_neighborhood_iterdata[0]->adv(m_neighborhood_iterdata[0], m_iter_ndim[0] - i - 1,
                        m_neighbor_iterindex[i]);
                    m_neighbor_data[1] = m_neighborhood_iterdata[1]->adv(m_neighborhood_iterdata[1], m_iter_ndim[0] - i - 1,
                        m_neighbor_iterindex[i]);
                }
                ++i;
            }

            return true;
        }

        return false;
    }

    bool next_neighbor() {
        size_t i = m_iter_ndim[0];
        if (i != 0) {
            bool neighborhood_empty = true;
            bool new_neighbor_within_bounds = true;
            do {
                --i;
                if (++m_neighbor_rel_iterindex[i] != m_neighborhood_itershape[i]) {                    
                    ++m_neighbor_iterindex[i];
                    neighborhood_empty = false;
                } else {
                    m_neighbor_rel_iterindex[i] = 0;
                    m_neighbor_iterindex[i] = m_iterindex[i] + m_neighborhood_iteroffset[i];
                }
                if ((m_neighbor_iterindex[i] < 0) || (m_neighbor_iterindex[i] >= m_itershape[i])) {
                    new_neighbor_within_bounds = false;
                }
            } while ((i != 0) && neighborhood_empty);

            for (size_t j = 0; (j != i) && new_neighbor_within_bounds; ++j) {
                if ((m_neighbor_iterindex[j] < 0) || (m_neighbor_iterindex[j] >= m_itershape[j])) {
                    new_neighbor_within_bounds = false;
                }
            }

            if (neighborhood_empty) {
                return false;
            }

            if (m_neighbor_within_bounds) {
                while ((i != 0) && ((m_neighbor_iterindex[i] < 0) || (m_neighbor_iterindex[i] >= m_itershape[i]))) {
                    --i;
                }

                m_neighbor_data[0] = m_neighborhood_iterdata[0]->incr(m_neighborhood_iterdata[0], m_iter_ndim[0] - i - 1);
                m_neighbor_data[1] = m_neighborhood_iterdata[1]->incr(m_neighborhood_iterdata[1], m_iter_ndim[0] - i - 1);
                while (++i != m_iter_ndim[0]) {
                    if (m_neighbor_iterindex[i] >= 0) {
                        m_neighbor_data[0] = m_neighborhood_iterdata[0]->adv(m_neighborhood_iterdata[0], m_iter_ndim[0] - i - 1,
                            m_neighbor_iterindex[i] - m_neighbor_rel_iterindex[i]);
                        m_neighbor_data[1] = m_neighborhood_iterdata[1]->adv(m_neighborhood_iterdata[1], m_iter_ndim[0] - i - 1,
                            m_neighbor_iterindex[i] - m_neighbor_rel_iterindex[i]);
                    }
                }
            }
            m_neighbor_within_bounds = new_neighbor_within_bounds;

            return true;
        }

        return false;
    }

    template<bool R>
    const intptr_t *neighbor_index() const;

    bool neighbor_within_bounds() const {
        return m_neighbor_within_bounds;
    }

    /**
     * Provide non-const access to the 'write' operands.
     */
    template<int K>
    inline typename enable_if<detail::is_value_within_bounds<K, 0, 1>::value, char *>::type neighbor_data() {
        if (m_neighbor_within_bounds) {
            return m_neighbor_data[K];
        } else {
            return NULL;
        }
    }

    /**
     * Provide const access to all the operands.
     */
    template<int K>
    inline typename enable_if<detail::is_value_within_bounds<K, 0, 2>::value, const char *>::type neighbor_data() const {
        if (m_neighbor_within_bounds) {
            return m_neighbor_data[K];
        } else {
            return NULL;
        }
    }
};

template<>
inline const intptr_t *array_neighborhood_iter<1, 1>::neighbor_index<false>() const {
    return m_neighbor_iterindex.get();
}

template<>
inline const intptr_t *array_neighborhood_iter<1, 1>::neighbor_index<true>() const {
    return m_neighbor_rel_iterindex.get();
}

template<>
class array_iter<0, 2> {
    intptr_t m_itersize;
    intptr_t m_iter_ndim;
    dimvector m_iterindex;
    dimvector m_itershape;
    char *m_data[2];
    const char *m_arrmeta[2];
    iterdata_common *m_iterdata[2];
    ndt::type m_array_tp[2], m_uniform_tp[2];
public:
    array_iter(const nd::array& op0, const nd::array& op1) {
        nd::array ops[2] = {op0, op1};
        m_array_tp[0] = op0.get_type();
        m_array_tp[1] = op1.get_type();
        m_itersize = 1;
        shortvector<int> axis_perm; // TODO: Use this to affect the iteration order
        broadcast_input_shapes(2, ops, m_iter_ndim, m_itershape, axis_perm);
        // Allocate and initialize the iterdata
        if (m_iter_ndim != 0) {
            m_iterindex.init(m_iter_ndim);
            memset(m_iterindex.get(), 0, sizeof(intptr_t) * m_iter_ndim);
            // The op iterdata
            for (int i = 0; i < 2; ++i) {
                intptr_t iter_ndim_i = m_array_tp[i].get_ndim();
                size_t iterdata_size = m_array_tp[i].get_broadcasted_iterdata_size(iter_ndim_i);
                m_iterdata[i] = reinterpret_cast<iterdata_common *>(malloc(iterdata_size));
                if (!m_iterdata[i]) {
                    throw std::bad_alloc();
                }
                m_arrmeta[i] = ops[i].get_arrmeta();
                m_array_tp[i].broadcasted_iterdata_construct(m_iterdata[i],
                                &m_arrmeta[i], iter_ndim_i,
                                m_itershape.get() + (m_iter_ndim - iter_ndim_i), m_uniform_tp[i]);
                m_data[i] = m_iterdata[i]->reset(m_iterdata[i], ops[i].get_ndo()->m_data_pointer, m_iter_ndim);
            }

            for (intptr_t i = 0, i_end = m_iter_ndim; i != i_end; ++i) {
                m_itersize *= m_itershape[i];
            }
        } else {
            for (size_t i = 0; i < 2; ++i) {
                m_iterdata[i] = NULL;
                m_uniform_tp[i] = m_array_tp[i];
                m_data[i] = ops[i].get_ndo()->m_data_pointer;
                m_arrmeta[i] = ops[i].get_arrmeta();
            }
        }
    }

    ~array_iter() {
        for (size_t i = 0; i < 2; ++i) {
            if (m_iterdata[i]) {
                m_array_tp[i].iterdata_destruct(m_iterdata[i], m_array_tp[i].get_ndim());
                free(m_iterdata[i]);
            }
        }
    }

    size_t itersize() const {
        return m_itersize;
    }

    bool empty() const {
        return m_itersize == 0;
    }

    bool next() {
        size_t i = m_iter_ndim;
        if (i != 0) {
            do {
                --i;
                if (++m_iterindex[i] != m_itershape[i]) {
                    for (size_t j = 0; j < 2; ++j) {
                        m_data[j] = m_iterdata[j]->incr(m_iterdata[j], m_iter_ndim - i - 1);
                    }
                    return true;
                } else {
                    m_iterindex[i] = 0;
                }
            } while (i != 0);
        }

        return false;
    }

    /**
     * Provide const access to all the operands.
     */
    template<int K>
    inline typename enable_if<detail::is_value_within_bounds<K, 0, 2>::value, const char *>::type data() const {
        return m_data[K];
    }

    template<int K>
    inline typename enable_if<detail::is_value_within_bounds<K, 0, 2>::value, const char *>::type arrmeta() const {
        return m_arrmeta[K];
    }

    template<int K>
    inline typename enable_if<detail::is_value_within_bounds<K, 0, 2>::value, const ndt::type&>::type get_uniform_dtype() const {
        return m_uniform_tp[K];
    }
};

template<>
class array_iter<1, 3> {
    intptr_t m_itersize;
    intptr_t m_iter_ndim[4];
    dimvector m_iterindex;
    dimvector m_itershape;
    char *m_data[4];
    const char *m_arrmeta[4];
    iterdata_common *m_iterdata[4];
    ndt::type m_array_tp[4], m_uniform_tp[4];
public:
    // Constructor which creates the output based on the input's broadcast shape
    array_iter(const ndt::type& op0_dtype, nd::array& out_op0, const nd::array& op1, const nd::array& op2, const nd::array& op3) {
        create_broadcast_result(op0_dtype, op1, op2, op3, out_op0, m_iter_ndim[0], m_itershape);
        nd::array ops[4] = {out_op0, op1, op2, op3};
        m_array_tp[0] = out_op0.get_type();
        m_array_tp[1] = op1.get_type();
        m_array_tp[2] = op2.get_type();
        m_array_tp[3] = op3.get_type();
        m_itersize = 1;
        m_iter_ndim[1] = m_array_tp[1].get_ndim();
        m_iter_ndim[2] = m_array_tp[2].get_ndim();
        m_iter_ndim[3] = m_array_tp[3].get_ndim();
        // Allocate and initialize the iterdata
        if (m_iter_ndim[0] != 0) {
            m_iterindex.init(m_iter_ndim[0]);
            memset(m_iterindex.get(), 0, sizeof(intptr_t) * m_iter_ndim[0]);
            // The destination iterdata
            size_t iterdata_size = m_array_tp[0].get_iterdata_size(m_iter_ndim[0]);
            m_iterdata[0] = reinterpret_cast<iterdata_common *>(malloc(iterdata_size));
            if (!m_iterdata[0]) {
                throw std::bad_alloc();
            }
            m_arrmeta[0] = out_op0.get_arrmeta();
            m_array_tp[0].iterdata_construct(m_iterdata[0],
                            &m_arrmeta[0], m_iter_ndim[0], m_itershape.get(), m_uniform_tp[0]);
            m_data[0] = m_iterdata[0]->reset(m_iterdata[0], out_op0.get_readwrite_originptr(), m_iter_ndim[0]);
            // The op iterdata
            for (int i = 1; i < 4; ++i) {
                iterdata_size = m_array_tp[i].get_broadcasted_iterdata_size(m_iter_ndim[i]);
                m_iterdata[i] = reinterpret_cast<iterdata_common *>(malloc(iterdata_size));
                if (!m_iterdata[i]) {
                    throw std::bad_alloc();
                }
                m_arrmeta[i] = ops[i].get_arrmeta();
                m_array_tp[i].broadcasted_iterdata_construct(m_iterdata[i],
                                &m_arrmeta[i], m_iter_ndim[i],
                                m_itershape.get() + (m_iter_ndim[0] - m_iter_ndim[i]), m_uniform_tp[i]);
                m_data[i] = m_iterdata[i]->reset(m_iterdata[i], ops[i].get_ndo()->m_data_pointer, m_iter_ndim[0]);
            }

            for (intptr_t i = 0, i_end = m_iter_ndim[0]; i != i_end; ++i) {
                m_itersize *= m_itershape[i];
            }
        } else {
            for (size_t i = 0; i < 4; ++i) {
                m_iterdata[i] = NULL;
                m_uniform_tp[i] = m_array_tp[i];
                m_data[i] = ops[i].get_ndo()->m_data_pointer;
                m_arrmeta[i] = ops[i].get_arrmeta();
            }
        }
    }

    ~array_iter() {
        for (size_t i = 0; i < 4; ++i) {
            if (m_iterdata[i]) {
                m_array_tp[i].iterdata_destruct(m_iterdata[i], m_iter_ndim[i]);
                free(m_iterdata[i]);
            }
        }
    }

    size_t itersize() const {
        return m_itersize;
    }

    bool empty() const {
        return m_itersize == 0;
    }

    bool next() {
        size_t i = m_iter_ndim[0];
        if (i != 0) {
            do {
                --i;
                if (++m_iterindex[i] != m_itershape[i]) {
                    for (size_t j = 0; j < 4; ++j) {
                        m_data[j] = m_iterdata[j]->incr(m_iterdata[j], m_iter_ndim[0] - i - 1);
                    }
                    return true;
                } else {
                    m_iterindex[i] = 0;
                }
            } while (i != 0);
        }

        return false;
    }

    /**
     * Provide non-const access to the 'write' operands.
     */
    template<int K>
    inline typename enable_if<detail::is_value_within_bounds<K, 0, 1>::value, char *>::type data() {
        return m_data[K];
    }

    /**
     * Provide const access to all the operands.
     */
    template<int K>
    inline typename enable_if<detail::is_value_within_bounds<K, 0, 4>::value, const char *>::type data() const {
        return m_data[K];
    }

    template<int K>
    inline typename enable_if<detail::is_value_within_bounds<K, 0, 4>::value, const char *>::type arrmeta() const {
        return m_arrmeta[K];
    }

    template<int K>
    inline typename enable_if<detail::is_value_within_bounds<K, 0, 4>::value, const ndt::type&>::type get_uniform_dtype() const {
        return m_uniform_tp[K];
    }
};

} // namespace dynd

#endif // _NDOBJECT_ITER_HPP_
