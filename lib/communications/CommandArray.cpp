#include "command.h"
#include <cstdlib>
#include <Arduino.h>

int8_t binary_search(uint8_t *array, uint8_t start, uint8_t end, uint8_t val);
int8_t linear_search(uint8_t *array, uint8_t size, uint8_t val);

CommandArray::CommandArray(const uint8_t size, Command ** backingStore) {
  _backingStore = backingStore;

  #if DEBUG
    Serial.println("Size of Command **: " + String(sizeof(_backingStore)));
    Serial.flush();
  #endif

  _ids = (uint8_t *) std::malloc(size * sizeof(uint8_t));
  _size = size;
}

Command * CommandArray::get(uint8_t id) {
  #if DEBUG
    Serial.println("Get Command, looking for id: " + String(id));
    Serial.flush();
  #endif
  int8_t index = linear_search(_ids, _size, id); //binary_search(_ids, 0, _size, id);
  if (index != -1) {
    #if DEBUG
      Serial.println("index: " + String(index));
      Serial.flush();
    #endif
    return _backingStore[index];
  } else {
    Serial.println("ERROR. ID NOT FOUND IN COMMAND ARRAY.");
    Serial.flush();
    return nullptr;
  }
}

void CommandArray::insert(Command * toInsert) {
  if(currIdx >= _size){
    Serial.println("Command Array is Full. Error!");
    Serial.flush();
    exit(1);
  }

  _backingStore[currIdx] = toInsert;
  _ids[currIdx] = toInsert->ID();
  currIdx++;
}

void CommandArray::updateIds() {
  for(int i = 0; i < _size; i++){
    _ids[i] = _backingStore[i]->ID();
    Serial.println(i);
    Serial.println(_ids[i]);
    Serial.flush();
  }
}

int8_t linear_search(uint8_t *array, uint8_t size, uint8_t val) {
  for (int i = 0; i < size; i++){
    if(array[i] == val){
      return i;
    }
  }
  return -1;
}

int8_t binary_search(uint8_t *array, uint8_t start, uint8_t end, uint8_t val) {
  #if DEBUG
    Serial.println("binary search, start: " + String(start) + ", end: " + String(end));
    Serial.flush();
  #endif
  if (array[start] == val){
    return start;
  } else if (start == end){
    return -1;
  } else {
    uint8_t new_size = (end - start + 1) / 2;
    uint8_t res1 = binary_search(array, start + new_size, end, val);
    uint8_t res2 = binary_search(array, start, end - new_size, val);
    if (res1 != -1){
      return res1;
    } else if (res2 != -1) {
      return res2;
    } else {
      return -1;
    }
  }
}
