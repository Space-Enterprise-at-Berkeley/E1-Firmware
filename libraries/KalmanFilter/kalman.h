/*
 * kalman.h
 *
 *  Created on: Dec 30, 2020
 *      Author: Vamshi
 */

#ifndef KALMAN_H_
#define KALMAN_H_

//#include <Arduino.h>
#include <Eigen/Dense>
#include <Eigen/Core>

using namespace std;
using namespace Eigen;

class Kalman
{
    public:
      Kalman(int stateDim, MatrixXd F, int measurementDim, int inputDim,
        MatrixXd H, MatrixXd B, MatrixXd Q, MatrixXd R);
      ~Kalman();
      void predict(VectorXd u);
      void update(VectorXd z);
      VectorXd _x;
      MatrixXd _P; // nxn
      MatrixXd _K; // nxm
    private:
      int _n;
      int _m;
      int _l; // size of input vector
      MatrixXd _F; // nxn
      MatrixXd _H; // mxn
      MatrixXd _B; // nxl
      MatrixXd _Q;
      MatrixXd _R;
      MatrixXd _S;

      VectorXd _z; // might not even need to store anything here if it gets passed in all the time
      VectorXd _y;

};

#endif /* KALMAN_H_ */
