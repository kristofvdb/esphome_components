#pragma once

#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/components/output/float_output.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace is31fl3239 {

class IS31FL3239Output;

class IS31FL3239Channel : public output::FloatOutput, public Parented<IS31FL3239Output> {
 public:
  void set_channel(uint8_t channel) { channel_ = channel; }

 protected:
  friend class IS31FL3239Output;

  void write_state(float state) override;

  uint8_t channel_;
};

/// IS31FL3239 float output component.
class IS31FL3239Output : public Component, public i2c::I2CDevice {
 public:
  IS31FL3239Output(bool bit16_mode = false) : bit16_mode_(bit16_mode) {}

  void register_channel(IS31FL3239Channel *channel);

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::HARDWARE; }
  void loop() override;

 protected:
  friend IS31FL3239Channel;

  void set_channel_value_(uint8_t channel, uint8_t value) {
    if (this->pwm_amounts_[channel] != value)
      this->update_ = true;
    this->pwm_amounts_[channel] = value;
  }

  bool bit16_mode_{true};
  uint8_t led_scaling_[24] = {
  	0,
  };
  uint8_t pwm_amounts_[256] = {
      0,
  };

  uint8_t mode_;

  uint8_t min_channel_{0xFF};
  uint8_t max_channel_{0x00};
  bool update_{true};
};

}  // namespace is31fl3239
}  // namespace esphome
