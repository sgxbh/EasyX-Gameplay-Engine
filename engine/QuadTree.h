#pragma once

#include "CoreMinimal.h"
#include "AreaInterFace.h"
#include "Camera.h"
#include "Collider.h"

#include <atomic>
#include <cstdlib>



template<typename T>
class VectorPool {
public: 
    struct Wrapper {
        std::vector<T*> vec; // 真正要用的容器
        Wrapper* next;  // 指向下一个空闲节点


        static std::atomic<size_t> allocCount;

        void* operator new(std::size_t sz) {
            allocCount++;
            return std::malloc(sz);
        }

        void operator delete(void* ptr) {
            std::free(ptr);
        }
    };

    Wrapper* freeList = nullptr; // 空闲链表头
    std::vector<std::unique_ptr<Wrapper[]>> blocks; 
    size_t blockSize;

public:
    VectorPool(size_t blockSize =32 ) : blockSize(blockSize) {}

    Wrapper* Acquire() {
        if (!freeList) {
            // 一次性分配一大块 Wrapper
            auto block = std::make_unique<Wrapper[]>(blockSize);
            for (size_t i = 0; i < blockSize; i++) {
                Wrapper::allocCount++;
                block[i].next = freeList;
                freeList = &block[i];
            }
            blocks.push_back(std::move(block));
        }
        
        Wrapper* w = freeList;
        freeList = freeList->next;
        w->vec.clear(); 
        return w;
    }

    void Release(Wrapper* v) {
        
        v->vec.clear();
        v->next = freeList;
        freeList = v;
    }
};


template<typename T>
class QuadTreeNodePool;

template<typename T>
class QuadTree 
{
    static_assert(std::is_base_of<AreaInterFace, T>::value,
        "T must derive from AreaInterface");
    static_assert(std::is_base_of<Collider, T>::value,
        "T must derive from AreaInterface");

    static QuadTreeNodePool<T> nodePool;
    static VectorPool<T> vectorPool;

    Vector2D pos;
    int size;

    static bool InitConfig;
    static int maxDepth;
    static int maxObjects;

public:
    typename QuadTreeNodePool<T>::Node* _nodePtr = nullptr;
    QuadTree<T>* parent = nullptr;
    QuadTree<T>* children[4];
    int depth = 1;
    int objectCount;
    typename VectorPool<T>::Wrapper* objects;
    bool IsLeaf;

    void DrawDebug()
    {
        Vector2D p = (pos - mainWorld.mainCamera->GetCameraPosition()) * 20.f / mainWorld.mainCamera->springArmLength_virtual + Vector2D(WIN_WIDTH / 2, WIN_HEIGHT / 2);
        Vector2D si = Vector2D(size,size) * 20.f / mainWorld.mainCamera->springArmLength_virtual;
        float left = p.x - si.x / 2, right = p.x + si.x / 2,
            top = p.y - si.y / 2, bottom = p.y + si.y / 2;
        rectangle((int)left, (int)top, (int)right, (int)bottom);
        if (!IsLeaf)
        {
            for (int i = 0; i < 4; ++i)
            {
                children[i]->DrawDebug();
			}
        }
    }

    void Init()
    {
        objectCount = 0;
        IsLeaf = true;
        objects = vectorPool.Acquire();
        depth = 1;
        parent = nullptr;
        //children
    }

    static QuadTree<T>* CreateRoot(int MaxDepth=5, int MaxObjects=4)
    {
        if (!InitConfig)
        {
            maxDepth = MaxDepth;
            maxObjects = MaxObjects;
            InitConfig = true;
            nodePool =  QuadTreeNodePool<T>();
            vectorPool = VectorPool<T>();
        }
        return nodePool.Allocate();
    }

    void InitRect(const Vector2D& position, int Size)
    {
        pos = position;
        size = Size;
    }
    void SetParent(QuadTree<T>* p)
    {
        parent = p;

    }
    void SetDepth(int d)
    {
        depth = d;
    }

    int GetTargetIndex(T* obj)
    {
        
        float top = obj->GetWorldPosition().y - obj->Rheight / 2;
        float bottom = obj->GetWorldPosition().y + obj->Rheight / 2;
        float left = obj->GetWorldPosition().x - obj->Rwidth / 2;
        float right = obj->GetWorldPosition().x + obj->Rwidth / 2;
        
        int idx = 0;
        bool IsLeft = left < pos.x;
        bool IsTop = top < pos.y;
        bool IsRight = right > pos.x;
        bool IsBottom = bottom > pos.y;
        if (IsLeft)
        {
            if (IsTop)idx |= Area::LT;
            if (IsBottom)idx |= Area::LB;
        }
        if (IsRight)
        {
            if (IsTop)idx |= Area::RT;
            if (IsBottom)idx |= Area::RB;
        }
        return idx;
    }

    void Split()
    {
        IsLeaf = false;
        int newdepth = depth + 1;
        children[0] = nodePool.Allocate();
        children[0]->SetDepth(newdepth);
        children[0]->SetParent(this);
        children[0]->InitRect(Vector2D(pos.x - size / 4, pos.y - size / 4), size / 2);
        children[1] = nodePool.Allocate();
        children[1]->SetDepth(newdepth);
        children[1]->SetParent(this);
        children[1]->InitRect(Vector2D(pos.x + size / 4, pos.y - size / 4), size / 2);
        children[2] = nodePool.Allocate();
        children[2]->SetDepth(newdepth);
        children[2]->SetParent(this);
        children[2]->InitRect(Vector2D(pos.x - size / 4, pos.y + size / 4), size / 2);
        children[3] = nodePool.Allocate();
        children[3]->SetDepth(newdepth);
        children[3]->SetParent(this);
        children[3]->InitRect(Vector2D(pos.x + size / 4, pos.y + size / 4), size / 2);

        for (int i = objects->vec.size() - 1; i >= 0; --i)
        {
            objectCount -= 1;
            Insert(objects->vec[i]);
            
        }
        objects->vec.clear();
        vectorPool.Release(objects);
        objects = nullptr;
    }

    void Insert(T* obj)
    {
		obj->ReSetRect();
        if (IsLeaf)
        {
			float m = min(obj->Rwidth, obj->Rheight);
            if (objectCount + 1 > maxObjects && depth < maxDepth && m<size)
            {
                Split();
                Insert(obj);

            }
            else
            {
                objects->vec.push_back(obj);
                ++objectCount;
            }
        }
        else
        {
            int idx = GetTargetIndex(obj);
            if (idx != 0) ++objectCount;
            if ((idx & Area::LT) != 0)children[0]->Insert(obj);
            if ((idx & Area::RT) != 0)children[1]->Insert(obj);
            if ((idx & Area::LB) != 0)children[2]->Insert(obj);
            if ((idx & Area::RB) != 0)children[3]->Insert(obj);
        }
    }
    void Clear()
    {
        if (IsLeaf)
        {
			objectCount = 0;
            objects->vec.clear();
            if (parent) 
            {
				vectorPool.Release(objects);
                objects = nullptr;
            }
        }
        else
        {
            for (int i = 0; i < 4; ++i)
            {
                children[i]->Clear();
                nodePool.Deallocate(children[i]);
                children[i] = nullptr;
            }
            if (parent == nullptr)
            {
                Init();
            }
        }
    }

    static std::atomic<size_t> allocCount;

    void* operator new(std::size_t sz) {
        allocCount++;
        return std::malloc(sz);
    }

    void operator delete(void* ptr) {
        std::free(ptr);
    }
};


template<typename T>
class QuadTreeNodePool {
public:
    struct Node {
        QuadTree<T> node;
        Node* next;
    };

    Node* freeList;
    std::vector<std::unique_ptr<Node[]>> blocks;
    size_t blockSize;

public:
    QuadTreeNodePool(size_t blockSize = 256)
        : freeList(nullptr), blockSize(blockSize) {
    }

    QuadTree<T>* Allocate() {
        if (!freeList) {

            auto block = std::make_unique<Node[]>(blockSize);
            for (size_t i = 0; i < blockSize; i++) {
                
                block[i].next = freeList;
                freeList = &block[i];
            }
            blocks.push_back(std::move(block));
        }
        Node* node = freeList;
        freeList = node->next;
        node->node.Init();
        node->node._nodePtr = node;
        return &node->node;
    }

    void Deallocate(QuadTree<T>* ptr) {
        Node* node = ptr->_nodePtr;
		ptr->_nodePtr = nullptr; 
        if (node) {
            node->next = freeList;
            freeList = node;
        }
        
    }
};


template<typename T>
QuadTreeNodePool<T> QuadTree<T>::nodePool;

template<typename T>
VectorPool<T> QuadTree<T>::vectorPool;

template<typename T>
bool QuadTree<T>::InitConfig = false;

template<typename T>
int QuadTree<T>::maxDepth = 5;

template<typename T>
int QuadTree<T>::maxObjects = 4;


