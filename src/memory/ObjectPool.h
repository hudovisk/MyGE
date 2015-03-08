
#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include "debug/Logger.h"

/**
 * @brief Generic Object Pool.
 * @details Generic Object Pool based (copied)
 *  on this one: https://support.godaddy.com/library/simple-object-pool-in-c/
 * 
 * @tparam T Class to be handled by the pool.
 * 
 * @todo Better handle the out of bounds error, resizing the buffer.
 */
template <class T>
class ObjectPool
{
public:
	ObjectPool();
	~ObjectPool();

	bool init(unsigned int maxSize);
	bool destroy();

	void release(T* instance);

	T* create();

	T* getBuffer() { return m_objectBuffer; }
	unsigned int getBufferSize() { return m_maxSize; }

	T** getUsedBufferCache() { return &m_objectFreeCache[m_freeCacheTop]; }
	unsigned int getUsedSize() { return m_maxSize - m_freeCacheTop; }

private:
	T* m_objectBuffer;
	T** m_objectFreeCache;

	unsigned int m_maxSize;
	unsigned int m_freeCacheTop;
	bool m_isInitialised;
};


template<class T>
ObjectPool<T>::ObjectPool()
	: m_objectBuffer(nullptr), m_objectFreeCache(nullptr),
	m_maxSize(0), m_freeCacheTop(0), m_isInitialised(false)
{

}

template<class T>
ObjectPool<T>::~ObjectPool()
{
	if(m_isInitialised)
		destroy();
}

template<class T>
bool ObjectPool<T>::init(unsigned int maxSize)
{
	if(m_isInitialised)
	{
		LOG(WARN, "Calling init on an already initialised object pool");
		return false;
	}

	m_maxSize = maxSize;

	m_objectBuffer = new T[m_maxSize];
	m_objectFreeCache = new T*[m_maxSize];

	int i = m_maxSize - 1;
	for(m_freeCacheTop=0; m_freeCacheTop < m_maxSize; m_freeCacheTop++)
	{
		m_objectFreeCache[m_freeCacheTop] = &m_objectBuffer[i--];
	}

	m_isInitialised = true;

	return true;
}

template<class T>
bool ObjectPool<T>::destroy()
{
	if(!m_isInitialised)
	{
		LOG(WARN, "Calling destroy on an unitialised object pool.");
		return false;
	}

	if(m_freeCacheTop < m_maxSize)
	{
		LOG(WARN, "Calling destroy without releasing all objects. Possible memory leak!" <<
			"\n\t" << m_maxSize - m_freeCacheTop << " objects not released." );
	}

	delete [] m_objectBuffer;
	delete [] m_objectFreeCache;

	m_isInitialised = false;

	return true;
}

template<class T>
void ObjectPool<T>::release(T* instance)
{
	if(m_freeCacheTop < m_maxSize)
	{
		m_objectFreeCache[m_freeCacheTop++] = instance;
	}
	else
	{
		// LOG(ERROR, "ObjectPool.release with invalid instance");
	}
}

template<class T>
T* ObjectPool<T>::create()
{
	if(m_freeCacheTop > 0)
	{
		return m_objectFreeCache[--m_freeCacheTop];
	}
	else
	{
		LOG(ERROR, "Object pool size exceded: "<<m_maxSize);
		return nullptr;
	}
}

#endif //OBJECT_POOL_H