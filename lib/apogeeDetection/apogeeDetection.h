/*
 * apogeeDetection.h
 *
 *  Created on: Dec 31, 2020
 *      Author: Vamshi
 */

#ifndef APOGEE_H_
#define APOGEE_H_

#include <kalman.h>
#include <cmath>

using namespace std;
using namespace Eigen;

const uint8_t numPreviousAccel = 10;

class ApogeeDetection
{
    public:
      ApogeeDetection();
      ApogeeDetection(double dt, double altitudeVar, double accelVar, double initAlt, double initAcc, double mainChuteDeployLoc);
      ~ApogeeDetection();
      bool onGround();
      bool engineLit();
      bool engineOff();
      bool drogueReleased();

      void filter(double altitude, double accel_z);
      bool atApogee(double altitude, double accel_z);
      bool atMainChuteDeployLoc(double altitude, double accel_z);
      bool MeCo(double altitude, double accel_z);
      bool engineStarted(double altitude, double accel_z);


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
      double previousAccel[numPreviousAccel] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      double mainDeployLoc;
      double DeltaT;
      int outlook = 10; // how many data points should we be descending for before deciding
      double tolerance = 20; // how close to mainDeployLoc does the reading need to be (metres)
      int currConsecutiveDecreases = 0;
      int currConsecutiveIncreases = 0;
      int currConsecutiveAccelDecreases = 0;

      bool _onGround = true;
      bool _engineLit = false;

      bool _drogueReleased = false;
};

#endif /* APOGEE_H_ */
