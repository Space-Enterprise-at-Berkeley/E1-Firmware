#ifndef _ACTUATOR_
#define _ACTUATOR_

#include <string>

class Actuator {
   public:
      // pure virtual function providing interface framework.
      virtual void confirmation(float *data) = 0;
      virtual void parseCommand(float *data) = 0;
      Actuator(std::string name, uint8_t id):
      _name(name),
      _id(id)
      {}
      Actuator(std::string name):
      _name(name)
      {}
      void setId(uint8_t id){
        _id = id;
      }
   protected:
      std::string _name;
      uint8_t _id;
};

#endif
