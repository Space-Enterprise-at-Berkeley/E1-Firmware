/**
 *
 * HX711 library for Arduino
 *
 * Created by Bruce Deng, Feb 6, 2021.
**/
#ifndef __loadCell__
#define __loadCell__

#include <HX711.h>

namespace LoadCell {

		HX711 *_loadcells;
		int _numSensors;

		byte  *_dout_pins;
		byte  *_sck_pins;
		float *_calibration_vals;

		int init(HX711 *loadcells, int numSensors, byte  *dout_pins,
									byte * sck_pins, float  *calibration_vals) {

      _loadcells = loadcells;

			_numSensors = numSensors;
			_dout_pins = dout_pins;
			_sck_pins = sck_pins;
			_calibration_vals = calibration_vals;

//~~~~~~Needs to happen wherever loadcell.h is used~~~~~~~~~
//     for (int i = 0; i < numSensors; i++) {
//     loadcells[i].begin(doutPins[i], sckPins[i]);
//     loadcells[i].set_scale(calVals[i]);
//     loadcells[i].tare();
//
//     long zero_factor = loadcells[i].read_average(); //Get a baseline reading
//     Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
//     Serial.println(zero_factor);
//     }
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


		 return 0;
		}

		void readLoadCells(float *data) {

			for (int i = 0; i < _numSensors; i++) {
				data[i] = _loadcells[i].get_units() * 0.453592;
			}
			data[_numSensors] = -1;
		}

}

#endif



		// HX711 _lc1;
		// HX711 _lc2;
		//
		// int calibrationValue1; // ======== SET THIS TO SOMETHING ===========
		// int calibrationValue2;
		//
		// #define DOUT1 2
		// #define SC
		//
		// #define DOUT2 4
		// #define SCL2 5
		//
    // void init() {
		// 	_lc1.begin(DOUT1, SCL1);
		//   _lc1.set_scale(calibrationValue1); //This value is obtained by using the SparkFun_HX711_Calibration sketch
		//   _lc1.tare();	//Assuming there is no weight on the scale at start up, reset the scale to 0
		//
		// 	_lc2.begin(DOUT2, SCL2);
		// 	_lc2.set_scale(calibrationValue2);
		// 	_lc2.tare();
		// }
		//
		// void readLoad(float * data) {
		// 	data[0] = _lc1.get_units();
		// 	data[1] = _lc2.get_units();
		// 	data[2] = -1;
		// }
