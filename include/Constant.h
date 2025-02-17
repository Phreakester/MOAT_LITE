#ifndef constant_h
#define constant_h

#include <SD.h>
#include <map>

#define dancing 13
// IF YOU WANT TO CHANGE MODEL NUMBER DO SO IN BEGINNING OF PRIVATE MEMBERS

struct Constant
{
  private:
  
  // This sets which model number is currently selected
  // std::map<String, int> pins = m_20_pins;
  // std::map<String, float> float_constants = m_20_float_constants;
  // std::map<String, int> int_constants = m_20_int_constants;

  std::map<String, int> pins = {
    {"estop", 36},
    {"enc_a", 3},
    {"enc_b", 4},
    {"hall_inbound", 22},
    {"hall_outbound", 23},
    {"engine_geartooth", 37},
    {"gearbox_geartooth", 36},
    {"thermistor_1", 40},
    {"thermistor_2", 39},
    {"thermistor_3", 38}
  };

  std::map<String, float> float_constants = {
    {"proportional_gain", 0.015},
    {"integral_gain", 0},
    {"derivative_gain", 0},
    {"exponential_filter_alpha", 0.5},
    {"overdrive_ratio", 0.85},
    {"ecvt_max_ratio", 4.25}
  };

  std::map<String, int> int_constants = {
    {"actuator_motor_number", 1},
    {"cooling_motor_number", 0},
    {"homing_timeout", 50e6}, // ms
    {"cycle_period", 10},     // ms
    // This also has to be changed in actuator header
    {"gearbox_rolling_frames", 60}
  };
  
  public:
  // These constants do not change between models

  // Engine Constants (ty Tyler)
  const unsigned int engine_idle = 1750;      // rpm
  const unsigned int engine_engage = 2100;    // rpm
  const unsigned int engine_launch = 2600;    // rpm
  const unsigned int engine_torque = 2700;    // rpm
  const unsigned int engine_power = 3400;     // rpm
  const unsigned int desired_rpm = 2250;      // rpm
  const float rpm_target_multiplier = 1.5;

  const static int minimum_rpm = 1000;        // rpm

  // These constants change between models

  // Pins
  const int estop_pin = pins["estop"];
  const int encoder_a_pin = pins["enc_a"];
  const int encoder_b_pin = pins["enc_b"];
  const int hall_inbound_pin = pins["hall_inbound"];
  const int hall_outbound_pin = pins["hall_outbound"];
  const int engine_geartooth_pin = pins["engine_geartooth"];
  const int gearbox_geartooth_pin = pins["gearbox_geartooth"];
  const int thermistor_1_pin = pins["thermistor_1"];
  const int thermistor_2_pin = pins["thermistor_2"];
  const int thermistor_3_pin = pins["thermistor_3"];

  // Actuator Constants
  const int actuator_motor_number = int_constants["actuator_motor_number"];     // odrive axis
  const int cooling_motor_number = int_constants["cooling_motor_number"];       // odrive axis
  const int homing_timeout = int_constants["homing_timeout"];                   // ms
  const int cycle_period = int_constants["cycle_period"];                       // ms

  const int gearbox_rolling_frames = int_constants["gearbox_rolling_frames"];     // number of frames

  const float proportional_gain = float_constants["proportional_gain"];
  const float integral_gain = float_constants["integral_gain"];
  const float derivative_gain = float_constants["derivative_gain"];
  const float exponential_filter_alpha = float_constants["exponential_filter_alpha"];

  const float position_p_gain = proportional_gain;

  // Physical Constants
  const float ecvt_max_ratio = float_constants["ecvt_max_ratio"];
  const float overdrive_ratio = float_constants["overdrive_ratio"];
  const int gearbox_engage_rpm = engine_engage / ecvt_max_ratio;
  const int gearbox_power_rpm = engine_power / ecvt_max_ratio;
  const int gearbox_overdrive_rpm = engine_power / overdrive_ratio;


  

  // Linear Actuator Math
  constexpr static float linear_distance_per_rotation = 0.125;            // inches/rotation
  constexpr static float linear_shift_length = 3;                         // inches
  constexpr static int32_t encoder_count_shift_length =
      (linear_shift_length / linear_distance_per_rotation) * 4 * 2048;    //encoder count
  constexpr static float linear_engage_length = 1;                        //inches
  constexpr static int32_t encoder_engage_dist = 
      (linear_engage_length / linear_distance_per_rotation) * 4 * 2048;   //encoder count
  constexpr static float linear_engage_buffer = .2;                       // inches
  constexpr static int32_t encoder_engage_buffer = 
      (linear_engage_buffer) / linear_distance_per_rotation * 4 * 2048;   // encoder count
  const float cycle_period_minutes = (cycle_period / 1e3) / 60;         // minutes
  constexpr static int eg_teeth_per_rotation = 88;
  

  
};

#endif