/*
 * apogeeDetection.h
 *
 *  Created on: Dec 31, 2020
 *      Author: Vamshi
 */

#ifndef APOGEE_H_
#define APOGEE_H_

#include <kalman.h>

using namespace std;
using namespace Eigen;

class ApogeeDetection
{
    public:
      ApogeeDetection();
      ApogeeDetection(double dt, double altitudeVar, double accelVar, double initAlt, double initAcc);
      ~ApogeeDetection();
      void filter(double altitude, double accel_z);
      bool atApogee(double altitude, double accel_z);
    private:
      int _n;
      int _m;
      MatrixXd* _F; // nxn
      MatrixXd* _H; // mxn
      MatrixXd* _Q;
      MatrixXd* _R;

      VectorXd* _x; // n
      VectorXd* _z; // might not even need to store anything here if it gets passed in all the time

      Kalman* kalmanfilter;

      double previousAltitude;
      double DeltaT;
      int outlook = 10; // how many data points should we be descending for before deciding
      int currConsecutiveDecreases = 0;
      double altitude;
      double acc_z;
};

#endif /* APOGEE_H_ */
