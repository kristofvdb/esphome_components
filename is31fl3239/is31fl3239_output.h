#pragma once

#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/components/output/float_output.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace is31fl3239 {

// 0*: Group dimming, 1: Group blinking
extern const uint8_t IS31FL3239_MODE2_DMBLNK;
// 0*: Output change on Stop command, 1: Output change on ACK
extern const uint8_t IS31FL3239_MODE2_OCH;
// 0*: WDT disabled, 1: WDT enabled
extern const uint8_t IS31FL3239_MODE2_WDTEN;
// WDT timeouts
extern const uint8_t IS31FL3239_MODE2_WDT_5MS;
extern const uint8_t IS31FL3239_MODE2_WDT_15MS;
extern const uint8_t IS31FL3239_MODE2_WDT_25MS;
extern const uint8_t IS31FL3239_MODE2_WDT_35MS;

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
  IS31FL3239Output(uint8_t mode = IS31FL3239_MODE2_OCH) : mode_(mode) {}

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

  uint8_t mode_;

  uint8_t min_channel_{0xFF};
  uint8_t max_channel_{0x00};
  uint8_t pwm_amounts_[256] = {
      0,
  };
  bool update_{true};
};

}  // namespace is31fl3239
}  // namespace esphome
