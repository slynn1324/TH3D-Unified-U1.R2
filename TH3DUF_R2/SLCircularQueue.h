/**
 * Modified from circularqueue.h that is used for the Buzzer class in Marlin to use a larger size 
 * uint16_t -> 65k
 * unit8_t -> 256
 * 
 * Must be careful though -- the board only has 16K ram... so can't actually use ALL of 65K size.
 */

#ifndef __CIRCULARQUEUE_H__
#define __CIRCULARQUEUE_H__

#include <Arduino.h>

/**
 * @brief   Circular Queue class
 * @details Implementation of the classic ring buffer data structure
 */
template<typename T, uint16_t N>
class SLCircularQueue {
  private:

    /**
     * @brief   Buffer structure
     * @details This structure consolidates all the overhead required to handle
     *          a circular queue such as the pointers and the buffer vector.
     */
    struct buffer_t {
      uint16_t head;
      uint16_t tail;
      uint16_t count;
      uint16_t size;
      T queue[N];
    } buffer;

  public:
    /**
     * @brief   Class constructor
     * @details This class requires two template parameters, T defines the type
     *          of item this queue will handle and N defines the maximum number of
     *          items that can be stored on the queue.
     */
    SLCircularQueue<T, N>() {
      this->buffer.size = N;
      this->buffer.count = this->buffer.head = this->buffer.tail = 0;
    }

    /**
     * @brief   Removes and returns a item from the queue
     * @details Removes the oldest item on the queue, pointed to by the
     *          buffer_t head field. The item is returned to the caller.
     * @return  type T item
     */
    T dequeue() {
      if (this->isEmpty()) return T();

      uint16_t index = this->buffer.head;

      --this->buffer.count;
      if (++this->buffer.head == this->buffer.size)
        this->buffer.head = 0;

      return this->buffer.queue[index];
    }

    /**
     * @brief   Adds an item to the queue
     * @details Adds an item to the queue on the location pointed by the buffer_t
     *          tail variable. Returns false if no queue space is available.
     * @param   item Item to be added to the queue
     * @return  true if the operation was successful
     */
    bool enqueue(T const &item) {
      if (this->isFull()) return false;

      this->buffer.queue[this->buffer.tail] = item;

      ++this->buffer.count;
      if (++this->buffer.tail == this->buffer.size)
        this->buffer.tail = 0;

      return true;
    }

    /**
     * @brief   Checks if the queue has no items
     * @details Returns true if there are no items on the queue, false otherwise.
     * @return  true if queue is empty
     */
    bool isEmpty() {
      return this->buffer.count == 0;
    }

    /**
     * @brief   Checks if the queue is full
     * @details Returns true if the queue is full, false otherwise.
     * @return  true if queue is full
     */
    bool isFull() {
      return this->buffer.count == this->buffer.size;
    }

    /**
     * @brief   Gets the queue size
     * @details Returns the maximum number of items a queue can have.
     * @return  the queue size
     */
    uint16_t size() {
      return this->buffer.size;
    }

    /**
     * @brief   Gets the next item from the queue without removing it
     * @details Returns the next item in the queue without removing it
     *          or updating the pointers.
     * @return  first item in the queue
     */
    T peek() {
      return this->buffer.queue[this->buffer.head];
    }

    /**
     * @brief Gets the number of items on the queue
     * @details Returns the current number of items stored on the queue.
     * @return number of items in the queue
     */
    uint16_t count() {
      return this->buffer.count;
    }
};

#endif
