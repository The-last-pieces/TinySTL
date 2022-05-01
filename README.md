# TinySTL

对STL的部分复现和扩展.

## 目的

- 学习C++新特性
- 学习STL源码
- 学习数据结构的实现原理
- 学习常用算法的实现原理

## 编译环境

- c++17
- mingw gcc 8.1.0
- cmake 3.10.2

## 参考书籍:

- STL源码剖析
- 算法导论

# 项目结构

## 通用工具库

- [ ] pair  
  复现标准库pair
- [ ] string  
  复现标准库string
- [ ] tuple  
  复现标准库tuple
- [ ] qstring  
  复现Qt中QString的大部分功能(如COW)

## 空间配置器

负责内存的配置和管理

- [x] 内存分配器
- [ ] 智能指针

## 迭代器

作为容器组件和算法组件的粘合剂

## 容器

本项目的核心组件

### 序列容器

- [ ] array  
  静态数组
- [x] vector  
  动态数组
- [ ] list  
  双向链表
- [ ] deque  
  双端队列
- [ ] stack  
  栈
- [ ] queue  
  队列
- [ ] priority_queue  
  优先队列
- [ ] forward_list  
  单向链表

### 关联式容器

- [ ] RB-tree  
  红黑树,一系列O(lgn)容器的基础
- [ ] set  
  有序集合
- [ ] map  
  有序映射
- [ ] multiset  
  有序可重复集合
- [ ] multimap  
  有序可重复映射
- [ ] hashtable  
  哈希表,一系列O(1)容器的基础
- [ ] unordered_set  
  无序集合
- [ ] unordered_map  
  无序映射
- [ ] unordered_multiset  
  无序可重复集合
- [ ] unordered_multimap  
  无序可重复映射

### 扩展数据结构

- [ ] bloom  
  布隆过滤器
- [ ] vbitmap  
  变长bitmap
- [ ] mono_stack  
  单调栈
- [ ] mono_queue  
  单调队列
- [ ] heap  
  堆
- [ ] tree_array  
  树状数组
- [ ] segment_tree    
  线段树
- [ ] skip_list  
  跳表
- [ ] lru_list
  LRU缓存
- [ ] trie  
  字典树
- [ ] AVL-tree  
  平衡二叉树
- [ ] B-tree  
  B树
- [ ] matrix  
  矩阵
- [ ] graphic  
  抽象图

## 算法

以迭代器为核心的一系列算法

## 仿函数

实现std::function和以其为核心的一系列算法

## 适配器

实现STL中的适配器模式

# 代码约定

命名空间:ttl  
命名风格:小写_小写  