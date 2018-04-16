#ifndef FREQUENCY_H
#define FREQUENCY_H

#define PI 3.1415926535897932384626433832795

#include <cmath>
class frequency{

private:


public:
  double frequency_in_hz;
  double real_sum;
  double imaginary_sum;

  int data_history_size;
  double* cosine;
  double* sine;
  double period_for_this_freq; //seconds

  int oldest_idx;
  int newest_idx;
  // int oldest_imaginary_idx;
  // int newest_imaginary_idx;


  frequency(double frequency_in_hz, double rt_period_in_ms, int _data_history_size);
  ~frequency();


  double real_significance();
  double imaginary_significance();
  void increment_one_timestep();

};

#endif
