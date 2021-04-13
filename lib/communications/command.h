#ifndef _Command_
#define _Command_

#include <string>

class Command {
   public:
      // pure virtual function providing interface framework.
      virtual void confirmation(float *data) = 0;
      virtual void parseCommand(float *data) = 0;

      Command(std::string name, uint8_t id):
      _name(name),
      _id(id)
      {}

      Command(std::string name):
      _name(name)
      {}

      void setId(uint8_t id) {
        _id = id;
      }

      uint8_t ID() {
        return _id;
      }
   protected:
      std::string _name;
      int8_t _id = -1;
};

class CommandArray {
  public:
    CommandArray(const uint8_t size, Command ** backingStore);
    Command * get(uint8_t id);
    void insert(Command *toInsert);
    void updateIds();
  private:
    Command** _backingStore;
    int8_t * _ids;
    uint8_t _size;
    uint8_t currIdx = 0;
};

#endif
