# B Plus Tree for the Train Ticket System

This is a mapping class on external storage, mapping every key to a value with a time stamp to make sure user can roll back to a certain time.

此类为一个外部存储上的映射类，需要实现将 `key` 映射到 `value`，并保存对应的时间戳，确保可以按照时间戳回滚。

Required:

要求：

- To maintain a external-storage-based mapping class with keys that may not be unique;

  维护一个键未必唯一的外部存储映射类；

- The class is able to roll back to a certain time;

  要求可以按照时间戳回滚；

- `Key` and `Value` need to have default constructor and a copy construct;

  `Key` 和 `Value` 需要有默认构造和拷贝构造；

- Both `Key` and `Value` *must* be *trivally copyable, trivally movable and
  trivally destructable*.

  `Key` 和 `Value` 必须是平凡可复制、平凡可移动、平凡可析构的（所有数据必须存在类空间内部，并保证直接储存数据后读出的内容与之前的数据完全一致）；

- `KeyCompare` and `ValueCompare` class needs to have an overloaded
  `operator()(const Key& lhs, const Key& rhs)`.

  `KeyCompare` 和 `ValueCompare` 需要有重载 `operator()(const Key& lhs, const Key& rhs)`。

- `KeyEqual` and `ValueCompare` class needs to have an overloaded
  `operator()(const Key& lhs, const Key& rhs)`.

  `KeyEqual` 和 `ValueCompare` 需要有重载 `operator()(const Key& lhs, const Key& rhs)`。

## Overview 概览

```c++
template<class Key,
         class Value,
         class KeyCompare   = std::less<Key>,
         class ValueCompare = std::less<Value>,
         class KeyEqual     = std::equal_to<Key>,
         class ValueEqual   = std::equal_to<Value>>
class BPTree {
public:
    BPTree(const char* filename): memo(filename, isNew);
    
    ~BPTree();

    bool Empty();

    void Clear();

    bool Contains(const KeyT &key);

    ValT Find();

    Vector<ValT> MultiFind(const KeyT &key);

    void Insert(const KeyT &key, const ValT &val);

    void Erase(const KeyT &key);
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

- The nodes that is frequently used.

  经常访问的节点。
