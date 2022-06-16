# Tile Storage Class for the Train Ticket System

This is a tile external storage class, to store data without any structure.
You need to get them with the position of class.  For the sake of the
rollback function, every modification is stored with a single linked list.
Besides, every data is stored with a time stamp.

此为平铺储存类，没有数据结构，使用者须以具体的文件位置访问。为实现回滚操作，任何修改操作都不会真正删除数据，而是透过单链表将点连接一起。此外，所有数据都会储存对应的时间戳。

## Overview 概览

```c++
template<class T>
class TileStorage {
public:
    using Ptr = long;
    /**
     * Binding the class with a certain file.  If the file is not empty, the
     * position of deleted nodes should be read from the very beginning of
     * the file.
     */
    TileStorage(const char* fileName);

    /**
     * Set the data at very beginning to be deletedNodes_;
     */
    ~TileStorage();

    /**
     * Add a new value with a time stamp.
     * @param value
     * @param timeStamp
     * @return the file position in this class.
     */
    Ptr Add(const T& value, long timeStamp);

    /**
     * Get the value at the position.
     * @return the data at the position
     */
    T Get(Ptr position);

    /**
     * Modify the data at the position with the newValue and the time stamp.
     * @return the position of the new value
     */
    Ptr Modify(Ptr position, const T& newValue, long timeStamp);

    /**
     * Roll back the data at the position to a certain time stamp.  The node belongs to the ``future'' can be deleted
     * @return the position of the rolled backed node
     */
    Ptr RollBack(Ptr position, long timeStamp);

    void Clear();
};
```

## Technical Details 技术细节

### Roll Back 回滚

Roll back the data to a certain time stamp.  The nodes of the data whose time
stamp is larger than the given time stamp will be removed.

将对应节点的数据回滚到对应时间节点，时间戳大于回滚时间戳的节点均会被移除。

### Garbage Collection 垃圾回收

There are two cases that will create garbages.

总共两种情况需要执行垃圾回收。

- The clearing operations.

  `Clear` 操作。

- The rolling-back operations.

  回滚操作。

The method to deal with: link deleted nodes with a single linked list whose
first node is stored as the class member `deletedNodes_`.

On deleting nodes:

删除时：

- Make the `previous` pointer of the node pointing to the node right at
   `deletedNodes_`.

  使节点内 `previous` 指向 `deletedNodes_` 的值。

- Let the `deletedNodes_` to be the node to be deleted.

  将 `deletedNodes_` 设为被删除的节点。

On add a new node:

新增节点时：

- If the `deletedNodes_` is `-1`, allocate a space at the end. (You should
use the `New_` member function.)

  如果 `deletedNodes_` 值为 `-1`，使用 `New_` 在文件末尾分配一块新的空间。

- If the `deletedNodes_` is not `-1`, put the node at the top of the single
linked list.  That is, the node at `deletedNodes_`.  And set the
`deletedNodes_` to be the node next to `deleteNodes_`.

  如果 `deletedNodes_` 值不为 `-1`，将节点覆盖在 `deletedNodes_` 的位置，并将 `deletedNodes_` 设定为下一节点。

