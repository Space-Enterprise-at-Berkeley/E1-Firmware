#include <Heaters.h>
#include <tempController.h>

namespace Heaters
{   
    HeaterTask::HeaterTask(Heater* heater, int target, void (Heater::*func)(uint32_t, int)){
        _heater = heater;
        _target = target;
        _func = func;
    }
    void HeaterTask::run(uint32_t exec_time) {
        (_heater->*_func)(exec_time, _target);
    }

    const uint8_t numHeaters = 6;

    uint8_t heaterChannels[numHeaters] = {2, 3, 1, 0, 4, 5};
    uint8_t heaterCommandIds[numHeaters] = {40, 41, 45, 42, 43, 44};
    uint8_t heaterINAAddrs[numHeaters] = {0x4B, 0x4C, 0x4A, 0x49, 0x4D, 0x4E};
    std::string names[numHeaters] = {"loxTankPTHeater", "loxGemsHeater", "propInjectorPTHeater", "propTankPTHeater", "propGemsHeater", "loxInjectorPTHeater"};
    float maxPTHeaterCurrent = 3.0;
    int tempSetPoint = 10;
    int algorithm = 2;
    float shuntR = 0.033;
    float maxExpectedCurrent = 5.0;

    const uint8_t numGPIOExpanders = 1;
    uint8_t gpioExpAddr[numGPIOExpanders] = {TCA6408A_ADDR1};
    uint8_t gpioExpIntPin[numGPIOExpanders] = {-1};
    GpioExpander heaterCtl(gpioExpAddr[0], gpioExpIntPin[0], &Wire);

    Heater& initHeater(TwoWire *localwire){
        std::string *namePtrs[numHeaters];
        for (int i = 0; i < numHeaters; i++) {
            namePtrs[i] = &names[i];
        }
        Heater *out = new Heater((uint8_t)numHeaters, namePtrs, heaterCommandIds, tempSetPoint, algorithm, &heaterCtl, heaterChannels, localwire, heaterINAAddrs, shuntR, maxExpectedCurrent, maxPTHeaterCurrent);
        return *out;
    }

    Heater::Heater(uint8_t numHeaters, std::string **names, uint8_t *heaterCommandIds, int tempSetPoint, int algorithmChoice, GpioExpander *expander, uint8_t *channels, TwoWire *wire, uint8_t *addrs, float shuntR, float maxExpectedCurrent, float maxPTHeaterCurrent) {
        _names = (std::string **)malloc(numHeaters * sizeof(std::string *));
        _heaterCommands = (HeaterCommand **)malloc(numHeaters * sizeof(HeaterCommand *));
        _maxPTHeaterCurrent = maxPTHeaterCurrent;
        _voltages = (float *) malloc((numHeaters) * sizeof(float));
        _currents = (float *) malloc((numHeaters) * sizeof(float));
        _overCurrents = (float *) malloc((numHeaters) * sizeof(float));

        for (int i = 0; i < numHeaters; i++) {
            HeaterCommand *newHeater = new HeaterCommand(*names[i], heaterCommandIds[i], tempSetPoint, algorithmChoice, expander, channels[i], wire, addrs[i], shuntR, maxExpectedCurrent);
            _heaterCommands[i] = newHeater;
            _names[i] = names[i];
        }
    }

    void Heater::controlTemp(uint32_t exec_time, int target){
        // TODO: get current temperature from thermocouple
        // TODO: save controlTemp output (heater status) somewhere
        float currTemp = 0.0;
        _heaterCommands[target]->controlTemp(currTemp);
    }

    void Heater::readCurrentDraw(uint32_t exec_time, int target){
        _currents[target] = _heaterCommands[target]->readCurrentDraw();
        // TODO: update timestamps, queue for Ethernet/radio output
    }

    void Heater::readBusVoltage(uint32_t exec_time, int target){
        _voltages[target] = _heaterCommands[target]->readBusVoltage();
        // TODO: update timestamps, queue for Ethernet/radio output
    }

    void Heater::checkOverCurrent(uint32_t exec_time, int target){
        _overCurrents[target] = _heaterCommands[target]->checkOverCurrent(_currents[target], _maxPTHeaterCurrent);
        // TODO: update timestamps, queue for Ethernet/radio output
    }

}
