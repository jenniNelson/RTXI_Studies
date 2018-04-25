
#include <cmath>
#include <frequency.h>

// Shifts frequency to align with rt_period_in_ms, and
// constructs sin/cos waves to measure significance with
frequency::frequency(double _frequency_in_hz, double rt_period_in_ms, int _data_history_size){

  // Even frequency's period with real-time period: 1/f + (1/f)% rt_period
  period_for_this_freq = 1/(_frequency_in_hz) + std::fmod((1/(_frequency_in_hz)),1000*rt_period_in_ms);

  frequency_in_hz = 1/ period_for_this_freq;

  data_history_size = _data_history_size;

  cosine = new double[data_history_size];
  sine = new double[data_history_size];

  for (int i = 0; i < data_history_size; i++) {
    cosine[i] = std::cos(2 * PI * frequency_in_hz/1000 * rt_period_in_ms * i);
    sine[i] = std::sin(2 * PI * frequency_in_hz/1000 * rt_period_in_ms * i);
  }

  real_sum = 0;
  imaginary_sum = 0;

  // Because we evened the period with rt_period earlier, this should be nearly an int
  int period_length_in_rt_units = static_cast<int> (period_for_this_freq / (1000* rt_period_in_ms));
  newest_idx = 0;
  oldest_idx = data_history_size - period_length_in_rt_units;
//  newest_imaginary_idx = 0;
//  oldest_imaginary_idx = data_history_size - period_length_in_rt_units;
}

frequency::frequency(){
  // period_for_this_freq = -1;
  // frequency_in_hz = -1;
  // data_history_size = -1;
  // real_sum = -1;
  // imaginary_sum = -1;
  // newest_idx = -1;
  // oldest_idx = -1;
}

frequency::~frequency(){
  delete[] cosine;
  delete[] sine;

}

double frequency::real_significance(){
  return cosine[newest_idx];
}

double frequency::imaginary_significance(){
  return sine[newest_idx];
}

void frequency::increment_one_timestep(){

  newest_idx = (newest_idx + 1)%data_history_size;
  oldest_idx = (oldest_idx + 1)%data_history_size;

}
