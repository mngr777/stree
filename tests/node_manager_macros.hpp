#ifndef STREE_TEST_NODE_MANAGER_MACROS_
#define STREE_TEST_NODE_MANAGER_MACROS_

#define CHECK_STATS(nms, type, arity, pool_size_answer, buffer_size_answer) \
    {                                                                   \
        auto item = nms.item(type, arity);                             \
        bool valid_pool_size = (item.pool_size == pool_size_answer);    \
        bool valid_buffer_size = (item.buffer_size == buffer_size_answer); \
        if (!valid_pool_size || ! valid_buffer_size) {                  \
            cerr << "[" << stree::type_to_string(type);                 \
            if (type == TypeFunction || type == TypeSelect)             \
                cerr << " " << static_cast<unsigned>(arity);            \
            cerr << "] "                                                \
                 << "pool size: " << item.pool_size                     \
                 << " (answer is " << pool_size_answer << "), "         \
                 << "buffer size: " << item.buffer_size                 \
                 << " (answer is " << buffer_size_answer << ")"         \
                 << endl;                                               \
            return -3;                                                  \
        }                                                               \
    }

#endif
