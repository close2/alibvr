#pragma once

namespace _tasks {
  
  /**
   * @brief A minimalistic wrapper for tasks
   * 
   * For variadic template lists functions would only be possible, if
   * they all had the same type.  This simple wrapper "converts"
   * tasks/functions to classes.
   * 
   * [Stackoverflow question about functions as tasks](http://stackoverflow.com/questions/40387760/functions-as-template-arguments)
   * 
   * @see TASK for a macro which automatically determines the types.
   * 
   * @tparam T The return type of the task.
   * @tparam T2 The argument type of the task (i.e. the system clock)
   * @tparam F The task/function.
   **/
  template <typename T, typename T2, T(*F)(T2)>
  struct TaskWrapper {
    static inline T run(T2 clock) {
      return F(clock);
    }
  };
  
  template <uint8_t(*F)(uint8_t)>
  TaskWrapper<uint8_t, uint8_t, F> wrap();
  template <void(*F)(uint8_t)>
  TaskWrapper<void, uint8_t, F> wrap();
  
  template <uint16_t(*F)(uint16_t)>
  TaskWrapper<uint16_t, uint16_t, F> wrap();
  template <void(*F)(uint16_t)>
  TaskWrapper<void, uint16_t, F> wrap();
  
  template <uint32_t(*F)(uint32_t)>
  TaskWrapper<uint32_t, uint32_t, F> wrap();
  template <void(*F)(uint32_t)>
  TaskWrapper<void, uint32_t, F> wrap();
  
  template <uint64_t(*F)(uint64_t)>
  TaskWrapper<void, uint64_t, F> wrap();
  template <uint64_t(*F)(uint64_t)>
  TaskWrapper<void, uint64_t, F> wrap();
  
  template <class T>
  T wrap();
}
