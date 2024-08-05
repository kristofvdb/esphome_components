#pragma once

#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/components/output/float_output.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace is31fl3246 {

class IS31FL3246Output;

class IS31FL3246Channel : public output::FloatOutput, public Parented<IS31FL3246Output> {
 public:
  void set_channel(uint8_t channel) { channel_ = channel; }

 protected:
  friend class IS31FL3246Output;

  void write_state(float state) override;

  uint8_t channel_;
};

/// IS31FL3246 float output component.
class IS31FL3246Output : public Component, public i2c::I2CDevice {
 public:
  IS31FL3246Output(){}

  void register_channel(IS31FL3246Channel *channel);

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::HARDWARE; }
  void loop() override;

 protected:
  friend IS31FL3246Channel;

  void enable(bool enable);
  void update();

  bool set_channel_LFPWM(uint8_t channelNo, uint8_t pwm);
  bool set_channel_HFPWM(uint8_t channelNo, uint16_t pwm);
  
  void set_channel_value_(uint8_t channel, uint8_t value) {
    if (this->pwm_amounts_[channel] != value)
      this->update_ = true;
    this->pwm_amounts_[channel] = value;
  }

  // two times the size because we keep a copy in memory to know wheter we have to write the register
  uint8_t pwm_amounts_[36*2] = {
      0,
  };

  uint8_t min_channel_{36};
  uint8_t max_channel_{0x00};
  bool update_{true};
};

}  // namespace is31fl3246
}  // namespace esphome
