//
// Created by s. on 13.08.2021.
//

#ifndef UNTITLED_SHARED_PTR_H
#define UNTITLED_SHARED_PTR_H

#include <atomic>
#include <cassert>

namespace another_smart_pointer
{
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * shared_ptr - это "умный указатель", который обеспечивает общее владение (shared ownership) объекта, на который ссылается.
 * Несколько объектов типа shared_ptr могут владеть одним и тем же объектом.
 * Объект уничтожается, и его память освобождается, когда уничтожается последний
 * оставшийся shared_ptr, владеющий объектом.
 *
 * Задача.
 *      Написать свою имплементацию shared_ptr.
 *      Ожидаемый набор методов:
 *          Конструктор по умолчанию                                                   (1)
 *          Конструктор копирования                                                    (2)
 *          Конструктор от указателя                                                   (3)
 *          Оператор присваивания (copy-assignment)                                    (4)
 *          Деструктор                                                                 (5)
 *          operator*                                                                  (6)
 *          Метод reset, сбрасывающий владение текущим указателем типа T у shared_ptr
 *              и захватывающий управление временем жизни переданного методу указателя (7)
 *
 * @tparam T - тип объекта, владение которым обеспечивается
 */
template <typename T>
class shared_ptr
{
public:

    /// Default constructor (1)
    shared_ptr() noexcept = default;

    /// Constructor (3)
    /**
     * Construct shared pointer with specified managed object
     * @param pObj - pointer to managed object. May by nullptr
     */
    explicit shared_ptr(T* pObj);

    /// Copy-constructor (2)
    shared_ptr(const shared_ptr<T>& other);
    /// Move-constructor (3*)
    shared_ptr(shared_ptr&& other) noexcept;

    /// Destructor (5)
    ~shared_ptr();

    /// Copy-assignment operator (4)
    shared_ptr<T>& operator=(const shared_ptr<T>& other) noexcept;
    /// Move-assignment operator (4*)
    shared_ptr<T>& operator=(shared_ptr&& other) noexcept;

    /// Get pointer to managed object
    /**
     * @return T* - pointer to managed object
     */
    T* get() const noexcept;

    /// operator* (6)
    /**
     * @return T& - reference to managed object
	 * 		ATTENTION: this function may be used only in case when we have pointer
     * 		to managed object not equal to nullptr.
     */
    T& operator*() const noexcept;

    /// Get ref counter for managed object
    /**
     * @return long - return the number of shared_ptr objects
     * 		referring to the same managed object
     * 		This value is approximate in multithreaded environment.
     */
    long use_count() const noexcept;

    /// Reset the managed object (7)
    /**
     * Release the ownership of managed object.
     */
    void reset() noexcept;

    /// Replace the managed object (7)
    /**
     * Release the ownership of previous managed object and
     * setup new specified managed object.
     *
     * @param T* pObj - pointer to new specified managed object.
     * 		May be equal to nullptr.
     */
    void reset(T* pObj);

private:

    using CounterType = long;
    using Counter = std::atomic<CounterType>;

    // Create counter
    static Counter* createCounter();

    // Increment counter
    void incrementCounter(Counter* pCounter) noexcept;

    // Decrement counter
    /*
     * Decrement counter and return old counter state
     * @return CounterType - old counter state
     */
    CounterType decrementCounter(Counter* pCounter);

    // Try ti increment counter (if managed object specified)
    void tryToIncrementCounter();

    // Destroy counter object and reset pointer to counter object
    void destroyCounter() noexcept;

    // Destroy managed object and reset pointer to managed object
    void destroyObject();

    // Try to destroy managed object
    /*
     * Method releases the ownership of managed object.
     * If it is last ownership, managed object will be destroyed.
     * After that, reset pointers to counter object and to owned object.
     */
    void tryToDestroy();

private:

    Counter* m_pCounter = nullptr;      // Pointer to ref counter
    T*       m_pObject  = nullptr;      // Pointer to owned object
};

//----------------------------------------------------------------------------//
template<typename T>
shared_ptr<T>::shared_ptr(T* pObj)
    : m_pObject(pObj)
{
    tryToIncrementCounter();
}

//----------------------------------------------------------------------------//
template<typename T>
shared_ptr<T>::shared_ptr(const shared_ptr<T>& other)
    : m_pCounter(other.m_pCounter)
    , m_pObject (other.m_pObject)
{
    tryToIncrementCounter();
}

//----------------------------------------------------------------------------//
template<typename T>
shared_ptr<T>::shared_ptr(shared_ptr&& other) noexcept
{
    std::swap(m_pCounter, other.m_pCounter);
    std::swap(m_pObject,  other.m_pObject);
}

//----------------------------------------------------------------------------//
template<typename T>
shared_ptr<T>::~shared_ptr()
{
    tryToDestroy();
}

//----------------------------------------------------------------------------//
template<typename T>
shared_ptr<T>&
shared_ptr<T>::operator=(const shared_ptr<T>& other) noexcept
{
    if (this == &other)
        return *this;

    tryToDestroy();

    m_pCounter = other.m_pCounter;
    m_pObject  = other.m_pObject;

    tryToIncrementCounter();

    return *this;
}

//----------------------------------------------------------------------------//
template<typename T>
shared_ptr<T>&
shared_ptr<T>::operator=(shared_ptr<T>&& other) noexcept
{
    if (this == &other)
        return *this;

    tryToDestroy();

    std::swap(m_pCounter, other.m_pCounter);
    std::swap(m_pObject,  other.m_pObject);

    return *this;
}

//----------------------------------------------------------------------------//
// Get pointer to managed object
template<typename T>
T*
shared_ptr<T>::get() const noexcept
{
    return m_pObject;
}

//----------------------------------------------------------------------------//
// operator*
template<typename T>
T&
shared_ptr<T>::operator*() const noexcept
{
    assert(m_pObject != nullptr);

    return *m_pObject;
}

//----------------------------------------------------------------------------//
// Get ref counter for managed object
template<typename T>
long
shared_ptr<T>::use_count() const noexcept
{
    return m_pCounter == nullptr ? 0 : m_pCounter->load(std::__1::memory_order_relaxed);
}

//----------------------------------------------------------------------------//
template<typename T>
void
shared_ptr<T>::reset() noexcept
{
    tryToDestroy();
}

//----------------------------------------------------------------------------//
template<typename T>
void
shared_ptr<T>::reset(T* pObj)
{
    tryToDestroy();

    m_pObject = pObj;

    tryToIncrementCounter();
}

//----------------------------------------------------------------------------//
template<typename T>
typename shared_ptr<T>::Counter*
shared_ptr<T>::createCounter()
{
    auto pCounter = new Counter();
    *pCounter = 0;
    return pCounter;
}

//----------------------------------------------------------------------------//
template<typename T>
void
shared_ptr<T>::incrementCounter(Counter* pCounter) noexcept
{
    assert(pCounter != nullptr);

    const auto result = pCounter->fetch_add(1);

    if (result < 0)
    	assert(false);
}

//----------------------------------------------------------------------------//
template<typename T>
typename shared_ptr<T>::CounterType
shared_ptr<T>::decrementCounter(Counter* pCounter)
{
    assert(pCounter != nullptr);

    const auto result = pCounter->fetch_sub(1);

    if (result < 0)
    	assert(false);

    return result;
}

//----------------------------------------------------------------------------//
template<typename T>
void
shared_ptr<T>::tryToIncrementCounter()
{
    if (m_pObject != nullptr)
    {
        if (m_pCounter == nullptr)
        {
            m_pCounter = createCounter();
        }

        incrementCounter(m_pCounter);
    }
}

//----------------------------------------------------------------------------//
template<typename T>
void
shared_ptr<T>::destroyCounter() noexcept
{
    delete m_pCounter;
    m_pCounter = nullptr;
}

//----------------------------------------------------------------------------//
template<typename T>
void
shared_ptr<T>::destroyObject()
{
    delete m_pObject;
    m_pObject = nullptr;
}

//----------------------------------------------------------------------------//
template<typename T>
void
shared_ptr<T>::tryToDestroy()
{
    assert((m_pCounter == nullptr) == (m_pObject == nullptr));

    if (m_pCounter != nullptr)
    {
        if (1 == decrementCounter(m_pCounter))
        {
            destroyObject();
            destroyCounter();
        }
        else
        {
            m_pCounter = nullptr;
            m_pObject  = nullptr;
        }
    }

    assert((m_pCounter == nullptr) == (m_pObject == nullptr));
}

//----------------------------------------------------------------------------//

}   //  namespace another_smart_pointer

#endif //UNTITLED_SHARED_PTR_H
