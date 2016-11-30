#pragma once

#include <stdint.h>

// mostly copied from: http://stackoverflow.com/questions/18701798/building-and-accessing-a-list-of-types-at-compile-time/18704609#18704609
namespace _task_list {
  struct TaskListEndMarker;

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

  typedef TaskList<TaskListEndMarker> TaskListEmpty;
}
