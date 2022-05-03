# B Plus Tree for the Train Ticket System

This is a mapping class on external storage, mapping every key to a value with a time stamp to make sure user can roll back to a certain time.

此类为一个外部存储上的映射类，需要实现将 `key` 映射到 `value`，并保存对应的时间戳，确保可以按照时间戳回滚。

Required:

要求：

- To maintain a external-storage-based mapping class with unique keys;

  维护一个键唯一的外部存储映射类；

- The class is able to roll back to a certain time;

  要求可以按照时间戳回滚；

- `Key` and `Value` need to have default constructor and a copy construct;

  `Key` 和 `Value` 需要有默认构造和拷贝构造；

- Both `Key` and `Value` *must* be *trivally copable, trivally movable and
  trivally destructable*.

  `Key` 和 `Value` 必须是平凡可复制、平凡可移动、平凡可析构的（所有数据必须存在类空间内部，并保证直接储存数据后读出的内容与之前的数据完全一致）；

- `Compare` class needs to have an overloaded
  `operator()(const Key& lhs, const Key& rhs)`.

  `Compare` 需要有重载 `operator()(const Key& lhs, const Key& rhs)`。

- `Equal` class needs to have an overloaded
  `operator()(const Key& lhs, const Key& rhs)`.

  `Equal` 需要有重载 `operator()(const Key& lhs, const Key& rhs)`。

## Overview 概览

```c++
template<class Key, class Value, class Compare, class Equal>
class BPTree {
public:
    using Ptr = long;

    struct PairData {
        Key key;
        Value value;
        long timeStamp;
    }

    /**
     * Binding the tree with a certain file.  If the file is empty, a head
     * node for a new class is stored, and the tree is constructed.  If not,
     * the metadata of the old tree is read.
     */
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
    bool Insert(const Key& key, const Value& value, long timeStamp);

    /**
     * Move the pair with the input key to deleted zone.
     * @param key
     */
    void Erase(const Key& key);

    /**
     * Roll back to the prevoius time stamp.  If the tree have been cleared
     * after that time stamp, the bahaviour is undefined.
     *
     */
    void RollBack(long timeStamp);

    /**
     * Clear all the data in this tree. All the space is ready to be reused.
     */
    void Clear();

    /**
     * Tell whether there is a node with the input key.
     */
    bool Contains(const Key& key);

    Value

private:
    struct Head_ {
        long nodeSize; // the proper size making sure the block size is 4KiB
        Ptr head = -1;
        Ptr garbage = -1; // a single linked list to store the erased pair(s)

        bool cached = false;
        PairData cachedData; // cached data

        // maybe some other cached stuff
    };

    struct Block_ {
        PairData data[2 * kTargetSize_];
        Ptr      pointers[2 * kTargetSize_]; // For leaf nodes, use it as linked pointer to other leafs
        long     count = 0;
        bool     leaf  = false;
    };

    constexpr static long kBlockSize_  = 4096;
    constexpr static long kTargetSize_ = (kBlockSize - 2 * sizeof(Ptr)) / (sizeof(Ptr) + sizeof(PairData)) / 2 - 1;

    static_assert(kTargetSize_ >= 2 && sizeof(Block_) > kBlockSize_);

    template<class Type>
    Type Read_(Ptr position);

    /**
     * Read the block from the file.
     * @param position
     * @return the block at the position
     */
    Block_ ReadBlock_(Ptr position);

    /**
     * Read the head meta from the class.
     * @param position
     * @return the ptr data
     */
    Head_ ReadHead_(Ptr position);

    /**
     * Allocate a space at the end of the file.
     */
    Ptr New_(long size);

    /**
     * Store the value at the given position.
     */
    template<class Type>
    void Write_(Ptr position, const Type& value);

    Head_ head_;
};
```
## Technical Details 技术细节

### block size 块大小

The total storage of a block should be `4KiB`.

一个块的大小应当为 `4KiB`。

### Erasion 数据移除

Every erased pair should not be actually removed.  Instead, it should be move
to a place dedicated for garbages (reserved for roll back operations).
(Clearing operation is excluded.)

被移除的节点不应被真的删除，它们应当被移到专门放置移除节点的地方（为回滚操作预留）。（`Clear` 操作除外）

### Cache 快取 / 缓存

The following cases may be cached.

以下是需要被快取（缓存）的情形。

- The node that have been queried just now, say `Contains()`.

  此前刚刚被询问的节点，如 `Contains()`。

- Head node.

  头节点。

- (Optional) The nodes that is frequently used, like the nodes close to the head.

  （可选）经常访问的节点，如离头节点很近的节点。

