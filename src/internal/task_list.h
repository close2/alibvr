#pragma once

// mostly copied from: http://stackoverflow.com/questions/18701798/building-and-accessing-a-list-of-types-at-compile-time/18704609#18704609
namespace _task_list {
  struct TaskListEndMarker {
    static inline uint8_t is_enabled() { return 0; }
    template<typename T>
    static inline T run(const T& clock) { return clock;}
  };

  template <typename ...> struct concat;

  template <template <typename ...> class List, typename T>
  struct concat<List<TaskListEndMarker>, T>
  {
    typedef List<T> task;
  };
  
  template <template <typename ...> class List, typename ...Tasks, typename T>
  struct concat<List<Tasks...>, T>
  {
    typedef List<Tasks..., T> task;
  };

  template <typename...> struct TaskList {};

  // TODO find out if we can remove the next 2 lines
  template <>
  struct TaskList<TaskListEndMarker> {};
  typedef TaskList<TaskListEndMarker> TaskListEmpty;
}
