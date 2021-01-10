/**
 *
 * HX711 library for Arduino
 * https://github.com/bogde/HX711
 *
 * MIT License
 * (c) 2018 Bogdan Necula
 *
**/
#include <HX711.h>

namespace LoadCell {
		HX711 _lc1;
		HX711 _lc2;

		int calibrationValue1; // ======== SET THIS TO SOMETHING ===========
		int calibrationValue2;

		#define DOUT1 2
		#define SCL1 3

		#define DOUT2 4
		#define SCL2 5

    void init() {
			_lc1.begin(DOUT1, SCL1);
		  _lc1.set_scale(calibrationValue1); //This value is obtained by using the SparkFun_HX711_Calibration sketch
		  _lc1.tare();	//Assuming there is no weight on the scale at start up, reset the scale to 0

			_lc2.begin(DOUT2, SCL2);
			_lc2.set_scale(calibrationValue2);
			_lc2.tare();
		}

		void readLoad(float * data) {
			data[0] = _lc1.get_units();
			data[1] = _lc2.get_units();
			data[2] = -1;
		}
}
