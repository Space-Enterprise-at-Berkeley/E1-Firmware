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

		HX711 ** _lc_amp_boards;
		int _numSensors;

		byte * _dout_pins;
		byte * _sck_pins;
		float * _calibration_vals;

		int init(int numSensors, byte * dout_pins,
									byte * sck_pins, float * calibration_vals) {

			_lc_amp_boards = (HX711 **) malloc(numSensors * sizeof(HX711));

			_dout_pins = (byte *) malloc(numSensors * sizeof(byte));
			_sck_pins = (byte *) malloc(numSensors * sizeof(byte));
			_calibration_vals = (float *) malloc(numSensors * sizeof(float));

			_numSensors = numSensors;

			for (int i = 0; i < numSensors; i++) {
				
				_lc_amp_boards[i] = new HX711();

				if(!_lc_amp_boards[i]->wait_ready_retry()) {
					Serial.println("timed out, loadCell: " + i);
					return -1;
				}

				_lc_amp_boards[i]->begin(dout_pins[i], sck_pins[i]);
				_lc_amp_boards[i]->set_scale(calibration_vals[i]);
				_lc_amp_boards[i]->tare();

			}
			return 0;
		}

		void readLoadCells(float *data) {
			for (int i = 0; i < _numSensors; i++) {
				data[i] = _lc_amp_boards[i]->get_units();
			}
			data[_numSensors] = -1;
		}

		int freeAllResources() {
        free(_lc_amp_boards);
        free(_dout_pins);
		free(_sck_pins);
		free(_calibration_vals);
        return 0;
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
		// #define SCL1 3
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
