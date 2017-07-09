/*
 * LRUCache11 - a templated C++11 based LRU cache class that allows
 * specification of
 * key, value and optionally the map container type (defaults to
 * std::unordered_map)
 * By using the std::map and a linked list of keys it allows O(1) insert, delete
 * and
 * refresh operations.
 *
 * This is a header-only library and all you need is the LRUCache11.hpp file
 *
 * Github: https://github.com/mohaps/lrucache11
 *
 * This is a follow-up to the LRUCache project -
 * https://github.com/mohaps/lrucache
 *
 * Copyright (c) 2012-22 SAURAV MOHAPATRA <mohaps@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#pragma once
#include <algorithm>
#include <cstdint>
#include <list>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <unordered_map>
#include "ObjectPool.hpp"
namespace lru11 {
/**
 * base class to prevent copy
 * use as ClassName : private NoCopy {}
 * to prevent copy constructor of ClassName and assignment by copy
 */
class NoCopy {
 public:
  virtual ~NoCopy() = default;

 protected:
  NoCopy() = default;

 private:
  NoCopy(const NoCopy&) = delete;
  const NoCopy& operator=(const NoCopy&) = delete;
};
/*
 * a noop lockable concept that can be used in place of std::mutex
 */
class NullLock {
 public:
  void lock() {}
  void unlock() {}
  bool try_lock() { return true; }
};

/**
 * error raised when a key not in cache is passed to get()
 */
class KeyNotFound : public std::invalid_argument {
 public:
  KeyNotFound() : std::invalid_argument("key_not_found") {}
};

template <typename K, typename V>
struct KeyValuePair {
 public:
  K key;
  V value;

  KeyValuePair(const K& k, const V& v) : key(k), value(v) {}
};

/**
 *	The LRU Cache class templated by
 *		Key - key type
 *		Value - value type
 *		MapType - an associative container like std::unordered_map
 *		LockType - a lock type derived from the Lock class (default:
 *NullLock = no synchronization)
 *
 *	The default NullLock based template is not thread-safe, however passing
 *Lock=std::mutex will make it
 *	thread-safe
 */
template <class Key, class Value, class Lock = NullLock,
          class Map = std::unordered_map<
              Key, typename std::list<KeyValuePair<Key, Value*>>::iterator>>
class Cache : private NoCopy {
 public:
  typedef KeyValuePair<Key, Value*> node_type;
  typedef std::list<KeyValuePair<Key, Value*>> list_type;
  typedef Map map_type;
  typedef Lock lock_type;
  typedef OPool::ObjectPool<Value>  Pool;
   using Guard = std::lock_guard<lock_type>;
  /**
   * the max size is the hard limit of keys and (maxSize + elasticity) is the
   * soft limit
   * the cache is allowed to grow till maxSize + elasticity and is pruned back
   * to maxSize keys
   * set maxSize = 0 for an unbounded cache (but in that case, you're better off
   * using a std::unordered_map
   * directly anyway! :)
   */
  explicit Cache(size_t maxSize = 64, size_t elasticity = 10)
      : maxSize_(maxSize), elasticity_(elasticity)
  {
	  pPool_ = new Pool(maxSize);
  }
  virtual ~Cache()
  {
	  if (pPool_)
	  {
		  delete pPool_;
	  }
  }

  size_t size() const
  {
    Guard g(lock_);
    return cache_.size();
  }

  Value* acquireObject()
  {
	  return &pPool_->acquireObject();
  }

  bool empty() const {
    Guard g(lock_);
    return cache_.empty();
  }
  void clear() {
    Guard g(lock_);
    cache_.clear();
    keys_.clear();
  }
  void insert(const Key& k, Value* v) {
    Guard g(lock_);
    const auto iter = cache_.find(k);
    if (iter != cache_.end()) 
	{
		//std::list<KeyValuePair<Key, Value>>::iterator iter;
		//����������ŵ��Ǽ�ֵ�ԣ�����ֵ���� value
	 
      iter->second->value = v;

	  //Transfers only the element pointed by iter->second from keys_ into the container(keys._begin).
	  //���������ŵ��б��ͷ������ʾ���ʹ�õĶ���
      keys_.splice(keys_.begin(), keys_, iter->second);

      return;
    }
	//Inserts a new element at the beginning of the list, right before its current first element.
	//This new element is constructed in place using args as the arguments for its construction.
    keys_.emplace_front(k, v);
	//�����ض��Ķ���
    cache_[k] = keys_.begin();
	//�������
    prune();
  }
  bool tryGet(const Key& kIn, Value** vOut) 
  {
    Guard g(lock_);
    const auto iter = cache_.find(kIn);
	//���û���ҵ������ش���
    if (iter == cache_.end()) 
	{
      return false;
    }
	//����
    keys_.splice(keys_.begin(), keys_, iter->second);
	//���ض�Ӧ��ֵ
    *vOut = iter->second->value;

    return true;
  }
  Value* get(const Key& k)
  {
    Guard g(lock_);
    const auto iter = cache_.find(k);
	//�Ҳ�����Ӧ�Ķ���
    if (iter == cache_.end())
	{
      return nullptr;
    }
	//���ڶ��е�ͷ��
    keys_.splice(keys_.begin(), keys_, iter->second);
	//���ض�Ӧ��ֵ
    return iter->second->value;
  }

  bool remove(const Key& k) 
  {
    Guard g(lock_);
    auto iter = cache_.find(k);
    if (iter == cache_.end())
	{
      return false;
    }
	//�Ӷ���ص��н������Ƴ�
	pPool_->releaseObject(*iter->second->value);
	//�������
    keys_.erase(iter->second);
    cache_.erase(iter);

    return true;
  }
  //�ж�key�Ƿ����
  bool contains(const Key& k) 
  {
    Guard g(lock_);
    return cache_.find(k) != cache_.end();
  }
  //��ʼ���õ����size
  size_t getMaxSize() const { return maxSize_; }
  //����size
  size_t getElasticity() const { return elasticity_; }
  //�������Ķ�������
  size_t getMaxAllowedSize() const { return maxSize_ + elasticity_; }

  // lamada ��������
  template <typename F>
  void cwalk(F& f) const 
  {
    Guard g(lock_);
    std::for_each(keys_.begin(), keys_.end(), f);
  }

 protected:
  size_t prune() {
	//�������Ĵ�С
    size_t maxAllowed = maxSize_ + elasticity_;
	//û�г���
    if (maxSize_ == 0 || cache_.size() < maxAllowed) 
	{
      return 0;
    }
	//�����˵Ļ����������Ķ��������
	//? ��α�֤�����Ķ���δ��ʹ�ã� ʹ�õ�ʱ����Ҫget�����治�����κεĻ���
    size_t count = 0;
    while (cache_.size() > maxSize_)
	{
	  //�Ӷ���ص��н������Ƴ�
	  pPool_->releaseObject(*keys_.back().value);

      cache_.erase(keys_.back().key);
      keys_.pop_back();
      ++count;
    }
    return count;
  }

 private:
  mutable Lock lock_;
  Map cache_;
  list_type keys_;
  size_t maxSize_;
  size_t elasticity_;
  Pool * pPool_;
};

}  // namespace LRUCache11
