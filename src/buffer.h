#pragma once


namespace _buffer {

  enum class Marker {
    WithMarkers,
    WithoutMarkers
  };

}

template <uint8_t buffer_size, _buffer::Marker UseMarkers = _buffer::Marker::WithoutMarkers, uint8_t OverwriteIfFull = 0>
class Buffer {

private:
  uint8_t head;
  uint8_t tail;  // if tail == buffer_size then buffer is full
  uint8_t buffer[buffer_size];
  static const uint8_t markerBufferSize = UseMarkers == _buffer::Marker::WithoutMarkers ? 0
                                        : (buffer_size + 7) / 8;
  uint8_t markers[markerBufferSize]; // for every 8 bits one marker byte

public:
  
  Buffer() {
    head = tail = 0;
  }
  
  uint8_t push(uint8_t c, uint8_t mark = 0) {
    if (is_full() && !OverwriteIfFull) return 0;
    
    // calculate new head
    const uint8_t i = head == (buffer_size - 1) ? 0
                    : head + 1;

    // if we fill the buffer, we set the "buffer is full flag"
    if (i == tail) {
      tail = buffer_size;
    }
    
    // first write new value
    buffer[head] = c;
    if (markerBufferSize > 0) {
      auto bit = head & 0b111;
      auto byte = head / 8;
      if (mark) markers[byte] |= 1 << bit;
      else markers[byte] &= ~(1 << bit);
    }
    // then update head
    // otherwise irq could write wrong value
    head = i;
    return 1;
  }
  
  uint8_t pop(uint8_t& marked) {
    marked = 0;
    
    if (is_empty()) return '\0';
    
    const auto current_tail = !is_full() ? tail : head;

    uint8_t c = buffer[current_tail];
    
    if (markerBufferSize > 0) {
      auto bit = current_tail & 0b111;
      auto byte = current_tail / 8;
      marked = markers[byte] & 1 << bit;
    }
    
    tail = (current_tail + 1);
    if (tail == buffer_size) {
      tail = 0;
    }
    return c;
  }
  
  uint8_t pop() {
    uint8_t dummy;
    return pop(dummy);
  }

  uint8_t length() {
    if (is_full()) return buffer_size;
    
    const auto l = head - tail;
    if (head > tail) return l;
    return l + buffer_size;
  }
  
  uint8_t is_empty() {
    return head == tail;
  }
  
  uint8_t is_full() {
    return tail == buffer_size;
  }
  
  void clear() {
    head = tail = 0;
  }
};

template <_buffer::Marker UseMarkers, uint8_t overwriteIfFull>
class Buffer<0, UseMarkers, overwriteIfFull> {
public:
  uint8_t push(uint8_t c, uint8_t mark = 0) {
    return 0;
  }

  uint8_t pop() {
    return '\0';
  }
  
  uint8_t pop(uint8_t& marked) {
    marked = 0;
    return '\0';
  }

  uint8_t length() {
    return 0;
  }
  
  uint8_t is_empty() {
    return 1;
  }
  
  uint8_t is_full() {
    return 1;
  }
  
  void clear() {
    // don't do anything.
  }
};


// Static_Buffer is static → by giving a second template argument, we make sure
// that the compiler sees different types and therefore generates
// different static entities.
template <uint8_t buffer_size, typename unique_id, _buffer::Marker UseMarkers = _buffer::Marker::WithoutMarkers, uint8_t OverwriteIfFull = 0>
class Static_Buffer {
private:
  static inline Buffer<buffer_size, UseMarkers, OverwriteIfFull>& get_buffer() {
    static auto buffer = Buffer<buffer_size, UseMarkers, OverwriteIfFull>();
    return buffer;
  }
public:
  static inline uint8_t push(uint8_t c, uint8_t marked = 0) {
    return get_buffer().push(c, marked);
  }

  static inline uint8_t pop() {
    uint8_t dummy;
    return get_buffer().pop(dummy);
  }

  static inline uint8_t pop(uint8_t& marked) {
    return get_buffer().pop(marked);
  }

  static inline uint8_t length() {
    return get_buffer().length();
  }
  
  static inline uint8_t is_empty() {
    return get_buffer().is_empty();
  }
  
  static inline uint8_t is_full() {
    return get_buffer().is_full();
  }
  
  static inline void clear() {
    get_buffer().clear();
  }
};
