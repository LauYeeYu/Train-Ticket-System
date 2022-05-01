# B Plus Tree for Train Ticket System

此类为一个映射类，需要实现将 `Key` 映射到 `Value`，

要求：

- 维护一个键唯一的外部存储映射类；

- 要求可以按照时间戳回滚；

- `Key` 和 `Value` 需要有默认构造和拷贝构造；

- `Key` 和 `Value` 必须是平凡可复制、平凡可移动、平凡可析构的（所有数据必须存在类空间内部，并保证直接储存数据后读出的内容与之前的数据完全一致）；

- `Compare` 需要有重载 `operator()(const Key& lhs, const Key& rhs)`。

## Overview 概览

```c++
template<class Key, class Value, class Compare>
class BPTree {
private:
    using Ptr = long;

    BPTree(const char* fileName);

    BPTree(const std::string& fileName);

    BPTree(const BPTree&) = delete;

    BPTree& operator=(const BPTree&) = delete;

    ~BPTree();

    /**
     * Insert a key value pair into this tree.
     * @param key
     * @param value
     * @return a bool to tell whether this operation is success or not
     */
    bool Insert(const Key& key, const Value& value, int timeStamp);

    /**
     * Erase the pair with the input key.
     * @param key
     */
    void Erase(const Key& key);

    /**
     * Roll back to the prevoius time stamp.  If the tree have been cleared
     * after that time stamp, the bahaviour is undefined.
     *
     */
    void RollBack(int timeStamp);

    /**
     * Clear all the data in this tree. All the space is ready to be reused.
     */
    void Clear();

    /**
     * Tell whether there is a node with the input key.
     */
    bool Contains(const Key& key);
};
```
