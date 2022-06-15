// Train Ticket System
// Copyright (C) 2022 Lau Yee-Yu & relyt871
//
// This library is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef TICKET_SYSTEM_INCLUDE_BP_TREE_H
#define TICKET_SYSTEM_INCLUDE_BP_TREE_H

#include <iostream>
#include <functional>

#include "memory.h"
#include "vector.h"
#include "utility.h"

template <class KeyT, class ValT,
    class KeyCompare = std::less<KeyT>,
    class ValueCompare = std::less<ValT>,
    class KeyEqual = std::equal_to<KeyT>,
    class ValueEqual = std::equal_to<ValT>>
class BPTree {

    friend class Node;
    friend class NleafNode;
    friend class LeafNode;
    using Ptr = long;

private:
    static constexpr int M = 4064 / (sizeof(KeyT) + 8) - 2;
    static constexpr int HM = M / 2;
    static constexpr int L = 4064 / (sizeof(KeyT) + sizeof(ValT)) - 1;
    static constexpr int HL = (L + 1) / 2;

    KeyCompare keyComp;
    ValueCompare valComp;
    KeyEqual keyEq;
    ValueEqual valEq;

    MemoryManager<4096> memo;

    bool isNew;
    struct Meta {
        Ptr root;
        Ptr head;
    };

    class Node {
    public:
        bool isleaf;
        int siz;
        Ptr pos;
        Node() {}
    };
    Ptr root, head;
    ValT lastVis;

    class NleafNode : public Node {
    public:
        KeyT keys[M + 1];
        Ptr child[M + 2];
        NleafNode() {}

        //find the first position greater than key
        inline int Locate_Single(const KeyT &key, BPTree* tree) {
            int L = 0, R = this -> siz - 1, ret = R + 1;
            while (L <= R) {
                int mid = (L + R) >> 1;
                if (tree -> keyComp(key, keys[mid])) {
                    ret = mid;
                    R = mid - 1;
                } else {
                    L = mid + 1;
                }
            }
            return ret;
        }

        //find the first position greater than or equal to key
        inline int Locate_Multi(const KeyT &key, BPTree* tree) {
            int L = 0, R = this -> siz - 1, ret = R + 1;
            while (L <= R) {
                int mid = (L + R) >> 1;
                if (tree -> keyComp(keys[mid], key)) {
                    L = mid + 1;
                } else {
                    ret = mid;
                    R = mid - 1;
                }
            }
            return ret;
        }

        bool Contains_(const KeyT &key, BPTree* tree) {
            int x = Locate_Single(key, tree);
            char *to = tree -> memo.ReadNode(child[x]);
            if (reinterpret_cast<Node*>(to) -> isleaf) {
                return reinterpret_cast<LeafNode*>(to) -> Contains_(key, tree);
            } else {
                return reinterpret_cast<NleafNode*>(to) -> Contains_(key, tree);
            }
        }

        Vector<ValT> MultiFind_(const KeyT &key, BPTree* tree) {
            int x = Locate_Multi(key, tree);
            char *to = tree -> memo.ReadNode(child[x]);
            if (reinterpret_cast<Node*>(to) -> isleaf) {
                return reinterpret_cast<LeafNode*>(to) -> MultiFind_(key, tree);
            } else {
                return reinterpret_cast<NleafNode*>(to) -> MultiFind_(key, tree);
            }
        }

        Ptr Split(KeyT &reg, BPTree* tree) {
            char *tmp = tree -> memo.AddNode();
            NleafNode* cur = reinterpret_cast<NleafNode*>(tmp);
            cur -> pos = tree -> memo.Last;
            cur -> isleaf = false;
            int i, j;
            for (i = HM + 1, j = 0; i < this -> siz; ++i, ++j) {
                cur -> keys[j] = this -> keys[i];
                cur -> child[j] = this -> child[i];
            }
            cur -> child[j] = this -> child[this -> siz];
            reg = this -> keys[HM];
            this -> siz = HM;
            cur -> siz = j;
            return cur -> pos;
        }

        void Merge(NleafNode* q, const KeyT &mid, BPTree* tree) {
            keys[this -> siz] = mid;
            for (int i = this -> siz + 1, j = 0; j < q -> siz; ++i, ++j) {
                this -> keys[i] = q -> keys[j];
                this -> child[i] = q -> child[j];
            }
            this -> siz += q -> siz + 1;
            this -> child[this -> siz] = q -> child[q -> siz];
            tree -> memo.DelNode(q -> pos);
        }

        void LeftBalance(NleafNode* q, KeyT &mid) {
            int sum = this -> siz + q -> siz;
            int lsiz = (sum + 1) / 2;
            int i, j;
            for (i = q -> siz - 1, j = sum - lsiz - 1; i >= 0; --i, --j) {
                q -> keys[j] = q -> keys[i];
                q -> child[j + 1] = q -> child[i + 1];
            }
            q -> child[j + 1] = q -> child[i + 1];
            q -> keys[j] = mid;
            for (i = this -> siz - 1, --j; i > lsiz; --i, --j) {
                q -> keys[j] = this -> keys[i];
                q -> child[j + 1] = this -> child[i + 1];
            }
            q -> child[0] = this -> child[lsiz + 1];
            mid = this -> keys[lsiz];
            this -> siz = lsiz;
            q -> siz = sum - lsiz;
        }

        void RightBalance(NleafNode* q, KeyT &mid) {
            int sum = this -> siz + q -> siz;
            int lsiz = (sum + 1) / 2;
            int i, j;
            this -> keys[this -> siz] = mid;
            for (i = this -> siz + 1, j = 0; i < lsiz; ++i, ++j) {
                this -> keys[i] = q -> keys[j];
                this -> child[i] = q -> child[j];
            }
            this -> child[i] = q -> child[j];
            mid = q -> keys[j];
            for (i = 0, ++j; j < q -> siz; ++i, ++j) {
                q -> keys[i] = q -> keys[j];
                q -> child[i] = q -> child[j];
            }
            q -> child[i] = q -> child[j];
            this -> siz = lsiz;
            q -> siz = sum - lsiz;
        }

        void Insert_(const KeyT &key, const ValT &val, BPTree* tree) {
            int x = Locate_Multi(key, tree);
            char *tmp = tree -> memo.ReadNode(child[x]);
            Node* to = reinterpret_cast<Node*>(tmp);
            if (to -> isleaf) {
                LeafNode* cur = reinterpret_cast<LeafNode*>(to);
                cur -> Insert_(key, val, tree);
                if (cur -> siz <= L) {
                    return;
                }
                KeyT mid;
                Ptr oth =  cur -> Split(mid, tree);
                for (int i = this -> siz - 1; i >= x; --i) {
                    keys[i + 1] = keys[i];
                    child[i + 2] = child[i + 1];
                }
                keys[x] = mid;
                child[x + 1] = oth;
                ++ this -> siz;
            } else {
                NleafNode* cur = reinterpret_cast<NleafNode*>(to);
                cur -> Insert_(key, val, tree);
                if (cur -> siz < M) {
                    return;
                }
                KeyT mid;
                Ptr oth = cur -> Split(mid, tree);
                for (int i = this -> siz - 1; i >= x; --i) {
                    keys[i + 1] = keys[i];
                    child[i + 2] = child[i + 1];
                }
                keys[x] = mid;
                child[x + 1] = oth;
                ++ this -> siz;
            }
        }

        bool Erase_(const KeyT &key, BPTree* tree) {
            int x = Locate_Single(key, tree);
            char *tmp = tree -> memo.ReadNode(child[x]);
            Node* to = reinterpret_cast<Node*>(tmp);
            if (to -> isleaf) {
                LeafNode* cur = reinterpret_cast<LeafNode*>(to);
                if (!cur -> Erase_(key, tree)) {
                    return false;
                }
                if (cur -> siz >= HL) {
                    return true;
                }
                LeafNode* oth;
                if (x > 0) {
                    tmp = tree -> memo.ReadNode(child[x - 1]);
                    oth = reinterpret_cast<LeafNode*>(tmp);
                    if (cur -> siz + oth -> siz <= L) {
                        oth -> Merge(cur, tree);
                        for (int i = x - 1; i < this -> siz - 1; ++i) {
                            keys[i] = keys[i + 1];
                            child[i + 1] = child[i + 2];
                        }
                        -- this -> siz;
                    } else {
                        oth -> LeftBalance(cur, keys[x - 1]);
                    }
                } else {
                    tmp = tree -> memo.ReadNode(child[1]);
                    oth = reinterpret_cast<LeafNode*>(tmp);
                    if (cur -> siz + oth -> siz <= L) {
                        cur -> Merge(oth, tree);
                        for (int i = 0; i < this -> siz - 1; ++i) {
                            keys[i] = keys[i + 1];
                            child[i + 1] = child[i + 2];
                        }
                        -- this -> siz;
                    } else {
                        cur -> RightBalance(oth, keys[0]);
                    }
                }
            } else {
                NleafNode* cur = reinterpret_cast<NleafNode*>(to);
                if (!cur -> Erase_(key, tree)) {
                    return false;
                }
                if (cur -> siz >= HM) {
                    return true;
                }
                NleafNode* oth;
                if (x > 0) {
                    tmp = tree -> memo.ReadNode(child[x - 1]);
                    oth = reinterpret_cast<NleafNode*>(tmp);
                    if (cur -> siz + oth -> siz + 1 < M) {
                        oth -> Merge(cur, keys[x - 1], tree);
                        for (int i = x - 1; i < this -> siz - 1; ++i) {
                            keys[i] = keys[i + 1];
                            child[i + 1] = child[i + 2];
                        }
                        -- this -> siz;
                    } else {
                        oth -> LeftBalance(cur, keys[x - 1]);
                    }
                } else {
                    tmp = tree -> memo.ReadNode(child[1]);
                    oth = reinterpret_cast<NleafNode*>(tmp);
                    if (cur -> siz + oth -> siz + 1 < M) {
                        cur -> Merge(oth, keys[0], tree);
                        for (int i = 0; i < this -> siz - 1; ++i) {
                            keys[i] = keys[i + 1];
                            child[i + 1] = child[i + 2];
                        }
                        -- this -> siz;
                    } else {
                        cur -> RightBalance(oth, keys[0]);
                    }
                }
            }
            return true;
        }
    };

    class LeafNode : public Node {
    public:
        Ptr nxt;
        KeyT keys[L + 1];
        ValT vals[L + 1];
        LeafNode() {}

        //find the first position greater than or equal to key
        inline int Locate(const KeyT &key, BPTree* tree) {
            int L = 0, R = this -> siz - 1, ret = R + 1;
            while (L <= R) {
                int mid = (L + R) >> 1;
                if (tree -> keyComp(keys[mid], key)) {
                    L = mid + 1;
                } else {
                    ret = mid;
                    R = mid - 1;
                }
            }
            return ret;
        }

        bool Contains_(const KeyT &key, BPTree* tree) {
            int x = Locate(key, tree);
            if (x < this -> siz && tree -> keyEq(key, keys[x])) {
                tree -> lastVis = vals[x];
                return true;
            } else {
                return false;
            }
        }

        Vector<ValT> MultiFind_(const KeyT &key, BPTree* tree) {
            int x = Locate(key, tree);
            Vector<ValT> ret;
            LeafNode* cur = this;
            while (1) {
                while (x < cur -> siz) {
                    if (tree -> keyEq(key, cur -> keys[x])) {
                        ret.PushBack(cur -> vals[x++]);
                    } else {
                        return ret;
                    }
                }
                if (cur -> nxt == -1) {
                    return ret;
                }
                char* tmp = tree -> memo.ReadNode(cur -> nxt);
                cur = reinterpret_cast<LeafNode*>(tmp);
                x = 0;
            }
        }

        Ptr Split(KeyT &reg, BPTree* tree) {
            char *tmp = tree -> memo.AddNode();
            LeafNode* cur = reinterpret_cast<LeafNode*>(tmp);
            cur -> pos = tree -> memo.Last;
            cur -> isleaf = true;
            int i, j;
            for (i = HL, j = 0; i < this -> siz; ++i, ++j) {
                cur -> keys[j] = this -> keys[i];
                cur -> vals[j] = this -> vals[i];
            }
            reg = cur -> keys[0];
            this -> siz = HL;
            cur -> siz = j;
            cur -> nxt = this -> nxt;
            this -> nxt = cur -> pos;
            return cur -> pos;
        }

        void Merge(LeafNode* q, BPTree* tree) {
            for (int i = this -> siz, j = 0; j < q -> siz; ++i, ++j) {
                this -> keys[i] = q -> keys[j];
                this -> vals[i] = q -> vals[j];
            }
            this -> siz += q -> siz;
            this -> nxt = q -> nxt;
            tree -> memo.DelNode(q -> pos);
        }

        void LeftBalance(LeafNode* q, KeyT &mid) {
            int sum = this -> siz + q -> siz;
            int lsiz = (sum + 1) / 2;
            int i, j;
            for (i = q -> siz - 1, j = sum - lsiz - 1; i >= 0; --i, --j) {
                q -> keys[j] = q -> keys[i];
                q -> vals[j] = q -> vals[i];
            }
            for (i = this -> siz - 1; i >= lsiz; --i, --j) {
                q -> keys[j] = this -> keys[i];
                q -> vals[j] = this -> vals[i];
            }
            mid = q -> keys[0];
            this -> siz = lsiz;
            q -> siz = sum - lsiz;
        }

        void RightBalance(LeafNode* q, KeyT &mid) {
            int sum = this -> siz + q -> siz;
            int lsiz = (sum + 1) / 2;
            int i, j;
            for (i = this -> siz, j = 0; i < lsiz; ++i, ++j) {
                this -> keys[i] = q -> keys[j];
                this -> vals[i] = q -> vals[j];
            }
            for (i = 0; j < q -> siz; ++i, ++j) {
                q -> keys[i] = q -> keys[j];
                q -> vals[i] = q -> vals[j];
            }
            mid = q -> keys[0];
            this -> siz = lsiz;
            q -> siz = sum - lsiz;
        }

        void Insert_(const KeyT &key, const ValT &val, BPTree* tree) {
            int x = Locate(key, tree);
            for (int i = this -> siz - 1; i >= x; --i) {
                keys[i + 1] = keys[i];
                vals[i + 1] = vals[i];
            }
            keys[x] = key;
            vals[x] = val;
            ++ this -> siz;
        }

        bool Erase_(const KeyT &key, BPTree* tree) {
            int x = Locate(key, tree);
            if (x == this -> siz || !tree -> keyEq(key, keys[x])) {
                return false;
            }
            for (int i = x; i < this -> siz - 1; ++i) {
                keys[i] = keys[i + 1];
                vals[i] = vals[i + 1];
            }
            -- this -> siz;
            return true;
        }
    };

    static_assert(M >= 2 && sizeof(NleafNode) <= 4096);
    static_assert(L >= 1 && sizeof(LeafNode) <= 4096);

    bool Contains_(const KeyT &key) {
        if (root == -1) {
            return false;
        }
        char *tmp = memo.ReadNode(root);
        if (reinterpret_cast<Node*>(tmp) -> isleaf) {
            return reinterpret_cast<LeafNode*>(tmp) -> Contains_(key, this);
        } else {
            return reinterpret_cast<NleafNode*>(tmp) -> Contains_(key, this);
        }
    }

    Vector<ValT> MultiFind_(const KeyT &key) {
        if (root == -1) {
            return Vector<ValT>();
        }
        char *tmp = memo.ReadNode(root);
        if (reinterpret_cast<Node*>(tmp) -> isleaf) {
            return reinterpret_cast<LeafNode*>(tmp) -> MultiFind_(key, this);
        } else {
            return reinterpret_cast<NleafNode*>(tmp) -> MultiFind_(key, this);
        }
    }

    bool Insert_(const KeyT &key, const ValT &val) {
        if (root == -1) {
            char *tmp = memo.AddNode();
            LeafNode* cur = reinterpret_cast<LeafNode*>(tmp);
            cur -> pos = memo.Last;
            cur -> isleaf = true;
            cur -> siz = 1;
            cur -> keys[0] = key;
            cur -> vals[0] = val;
            root = head = cur -> pos;
            cur -> nxt = -1;
            return true;
        }
        char *tmp = memo.ReadNode(root);
        if (reinterpret_cast<Node*>(tmp) -> isleaf) {
            LeafNode* rt = reinterpret_cast<LeafNode*>(tmp);
            rt -> Insert_(key, val, this);
            if (rt -> siz <= L) {
                return true;
            }
            char* tmp2 = memo.AddNode();
            NleafNode* cur = reinterpret_cast<NleafNode*>(tmp2);
            cur -> pos = memo.Last;
            cur -> isleaf = false;
            cur -> siz = 1;
            cur -> child[0] = root;
            KeyT mid;
            cur -> child[1] = reinterpret_cast<LeafNode*>(tmp) -> Split(mid, this);
            cur -> keys[0] = mid;
            root = cur -> pos;
        } else {
            NleafNode* rt = reinterpret_cast<NleafNode*>(tmp);
            rt -> Insert_(key, val, this);
            if (rt -> siz < M) {
                return true;
            }
            char* tmp2 = memo.AddNode();
            NleafNode* cur = reinterpret_cast<NleafNode*>(tmp2);
            cur -> pos = memo.Last;
            cur -> isleaf = false;
            cur -> siz = 1;
            cur -> child[0] = root;
            KeyT mid;
            cur -> child[1] = reinterpret_cast<NleafNode*>(tmp) -> Split(mid, this);
            cur -> keys[0] = mid;
            root = cur -> pos;
        }
        return true;
    }

    bool Erase_(const KeyT &key) {
        if (root == -1) {
            return false;
        }
        char *tmp = memo.ReadNode(root);
        Node* rt = reinterpret_cast<Node*>(tmp);
        if (rt -> isleaf) {
            if (!reinterpret_cast<LeafNode*>(tmp) -> Erase_(key, this)) {
                return false;
            }
            if (rt -> siz == 0) {
                memo.DelNode(root);
                root = -1;
            }
        } else {
            if (!reinterpret_cast<NleafNode*>(tmp) -> Erase_(key, this)) {
                return false;
            }
            if (rt -> siz == 0) {
                root = reinterpret_cast<NleafNode*>(rt) -> child[0];
                memo.DelNode(rt -> pos);
            }
        }
        return true;
    }

#ifdef TEST
    void Traverse_() {
        for (Ptr pos = head; pos != -1; ) {
            char *tmp = memo.ReadNode(pos);
            LeafNode* p = reinterpret_cast<LeafNode*>(tmp);
            for (int i = 0; i < p -> siz; ++i) {
                std::cout << p -> keys[i] << " " << p -> vals[i] << " ";
            }
            std::cout << std::endl;
            pos = p -> nxt;
        }
        //std::cout << std::endl;
    }
#endif

public:
    BPTree(const char* filename): memo(filename, isNew) {
        Meta *tmp = reinterpret_cast<Meta*>(memo.GetMeta());
        if (isNew) {
            root = -1;
            head = -1;
        } else {
            root = tmp -> root;
            head = tmp -> head;
        }
    }
    ~BPTree() {
        Meta *tmp = reinterpret_cast<Meta*>(memo.GetMeta());
        tmp -> root = root;
        tmp -> head = head;
    }

    bool Empty() {
        return root == -1;
    }

    void Clear() {
        memo.Clear();
        root = -1;
        head = -1;
    }

    bool Contains(const KeyT &key) {
        return Contains_(key);
    }

    ValT Find() {
        return lastVis;
    }

    Vector<ValT> MultiFind(const KeyT &key) {
        return MultiFind_(key);
    }

    bool Insert(const KeyT &key, const ValT &val) {
        return Insert_(key, val);
    }

    bool Erase(const KeyT &key) {
        return Erase_(key);
    }

#ifdef TEST
    void Traverse() {
        std::cerr << "start traverse" << std::endl;
        std::cerr << M << " " << L << std::endl;
        Traverse_();
        std::cerr << "finish traverse" << std::endl;
    }
#endif
};

#endif
