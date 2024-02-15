#include "is31fl3239_output.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace is31fl3239 {

static const char *const TAG = "is31fl3239";

// This is a minimal implementation
// Only supports setting the PWM value of a channel with 8 bit PWM
// No use of diagnostic functionality


// --- Registers ---
// All registers are readable/writable, except reset and update register (0-write triggers functionality)
const uint8_t IS31FL3239_CTL_REG = 0x00;
const uint8_t IS31FL3239_PWM_REG[] = { 0x05, 0x11, 0x1d, 0x25, 0x31, 0x3d }; 
const uint8_t IS31FL3239_PWM_REG_SUB_SIZE = 8;
const uint8_t IS31FL3239_UPDATE_REG = 0x49;
const uint8_t IS31FL3239_SCALING_REG[] = { 0x4c, 0x52, 0x58, 0x5c, 0x62, 0x68 }; 
const uint8_t IS31FL3239_SCALING_REG_SUB_SIZE = 4;
const uint8_t IS31FL3239_GCC_REG = 0x6E;
const uint8_t IS31FL3239_PHD_CLPH_REG = 0x70;
const uint8_t IS31FL3239_SHORT_DETECT_REG = 0x70;
const uint8_t IS31FL3239_OPEN_SHORT_DETECT_ENABLE_REG = 0x71;
const uint8_t IS31FL3239_OPEN_SHORT_REG0 = 0x72;
const uint8_t IS31FL3239_TEMP_SENSOR_REG = 0x77;
const uint8_t IS31FL3239_SPREAD_SPECTRUM_REG = 0x78;
const uint8_t IS31FL3239_RESET_REG = 0x7F;

// * marks register defaults
// note that IS31FL3239 initalizes all registers to 0
// 0*: Register auto increment disabled, 1: Register auto increment enabled
const uint8_t IS31FL3239_CTL_SSD = (1 << 0);

// 0*: oscillator clock frequency selection
const uint8_t IS31FL3239_CTL_OSC_16MHZ = (0x0) << 4;
const uint8_t IS31FL3239_CTL_OSC_8MHZ = (0x1) << 4;
const uint8_t IS31FL3239_CTL_OSC_1MHZ = (0x2) << 4;
const uint8_t IS31FL3239_CTL_OSC_500KHZ = (0x3) << 4;
const uint8_t IS31FL3239_CTL_OSC_250KHZ = (0x4) << 4;
const uint8_t IS31FL3239_CTL_OSC_125KHZ = (0x5) << 4;
const uint8_t IS31FL3239_CTL_OSC_62KHZ = (0x6) << 4;
const uint8_t IS31FL3239_CTL_OSC_31KHZ = (0x7) << 4;

// 0*: PWM resolution
const uint8_t IS31FL3239_CTL_PWM_8BIT = (0x0) << 1;
const uint8_t IS31FL3239_CTL_PWM_10BIT = (0x1) << 1;
const uint8_t IS31FL3239_CTL_PWM_12BIT = (0x2) << 1;
const uint8_t IS31FL3239_CTL_PWM_16BIT = (0x3) << 1;


void IS31FL3239Output::setup() {
  ESP_LOGCONFIG(TAG, "Setting up IS31FL3239OutputComponent...");

  ESP_LOGV(TAG, "  Resetting all devices on the bus...");

  // Reset all devices on the bus
  //if (this->bus_->write(IS31FL3239_SWRST_ADDR >> 1, IS31FL3239_RESET_REG, 0) != i2c::ERROR_OK) {

  // Reset device
  if (!this->write_byte(IS31FL3239_RESET_REG, 0)) {
    ESP_LOGE(TAG, "RESET failed");
    this->mark_failed();
    return;
  }

  enable(true);

  // Set Global Current Control 
  if (!this->write_byte(IS31FL3239_GCC_REG, 0xFF)) {
    ESP_LOGE(TAG, "GLOBAL CURRENT failed");
    this->mark_failed();
    return;
  }

  // turn on all LEDs
  //
  for (int channelNo = 0 ; channelNo < 24 ; channelNo++)
  {
    set_channel_pwm(channelNo, 0x00);
    set_channel_scaling(channelNo, 0x80);
  }   
    
/*
  // Sanity check: turn on 1 led
  if (!this->write_byte(0x4c, 0xFF)) {
    ESP_LOGE(TAG, "RESET failed");
    this->mark_failed();
    return;
  }
  if (!this->write_byte(0x05, 0xFF)) {
    ESP_LOGE(TAG, "RESET failed");
    this->mark_failed();
    return;
  }
  if (!this->write_byte(0x06, 0xFF)) {
    ESP_LOGE(TAG, "RESET failed");
    this->mark_failed();
    return;
  }
*/  
  // update
  delayMicroseconds(500);

  this->loop();
}

bool IS31FL3239Output::set_channel_scaling(uint8_t channelNo, uint8_t scaling) {
  uint8_t reg = get_scaling_reg_for_channel(channelNo);
  if (!this->write_byte(reg, scaling)) {
    this->status_set_warning();
    return false;
  }
  return true;
}

bool IS31FL3239Output::set_channel_pwm(uint8_t channelNo, uint8_t pwm) {
  uint8_t reg = get_PWM_reg_for_channel(channelNo);

  // this only sets the low value of PWM as 8 bit is the only resolution we support
  if (!this->write_byte(reg, pwm)) {
    this->status_set_warning();
    return false;
  }
  return true;
}

void IS31FL3239Output::update() {
  if (!this->write_byte(IS31FL3239_UPDATE_REG, 0x00)) {
    ESP_LOGE(TAG, "UPDATE failed");
    this->mark_failed();
    return;
  }
}

void IS31FL3239Output::enable(bool enable) {
  if(enable)
  {
    // Enable device
    if (!this->write_byte(IS31FL3239_CTL_REG, IS31FL3239_CTL_SSD)) {
      ESP_LOGE(TAG, "ENABLE failed");
      this->mark_failed();
      return;
    }
  }
  else
  {
    // Disable device
    if (!this->write_byte(IS31FL3239_CTL_REG, 0)) {
      ESP_LOGE(TAG, "DISABLE failed");
      this->mark_failed();
      return;
    }
  }
}

void IS31FL3239Output::dump_config() {
  ESP_LOGCONFIG(TAG, "IS31FL3239:");
  //ESP_LOGCONFIG(TAG, "  Mode: 0x%02X", this->mode_);

  if (this->is_failed()) {
    ESP_LOGE(TAG, "Setting up IS31FL3239 failed!");
  }
}

uint8_t IS31FL3239Output::get_PWM_reg_for_channel(uint8_t channel) {
    uint8_t index = channel / (IS31FL3239_PWM_REG_SUB_SIZE/2);
    uint8_t subIndex = channel % (IS31FL3239_PWM_REG_SUB_SIZE/2);

    uint8_t reg = IS31FL3239_PWM_REG[index] + subIndex*2;
    ESP_LOGVV(TAG, "Channel %02u: PWM reg=%02x ", channel, reg);

    return reg;
}

uint8_t IS31FL3239Output::get_scaling_reg_for_channel(uint8_t channel) {
    uint8_t index = channel / IS31FL3239_SCALING_REG_SUB_SIZE;
    uint8_t subIndex = channel % IS31FL3239_SCALING_REG_SUB_SIZE;

    uint8_t reg = IS31FL3239_SCALING_REG[index] + subIndex;
    ESP_LOGVV(TAG, "Channel %02u: Scaling reg=%02x ", channel, reg);

    return reg;
}

void IS31FL3239Output::loop() {
  if (this->min_channel_ == 0xFF || !this->update_)
    return;

  for (uint8_t channel = this->min_channel_; channel <= this->max_channel_; channel++) {
    uint8_t pwm = this->pwm_amounts_[channel];
    ESP_LOGVV(TAG, "Channel %02u: pwm=%02u ", channel, pwm);

    uint8_t reg = get_PWM_reg_for_channel(channel);
    if (!this->write_byte(reg, pwm)) {
      this->status_set_warning();
      return;
    }
  }
  update();
  this->status_clear_warning();
  this->update_ = false;
}

void IS31FL3239Output::register_channel(IS31FL3239Channel *channel) {
  auto c = channel->channel_;
  this->min_channel_ = std::min(this->min_channel_, c);
  this->max_channel_ = std::max(this->max_channel_, c);
  channel->set_parent(this);
}

void IS31FL3239Channel::write_state(float state) {
  const uint8_t max_duty = 255;
  const float duty_rounded = roundf(state * max_duty);
  auto duty = static_cast<uint8_t>(duty_rounded);
  this->parent_->set_channel_value_(this->channel_, duty);
}

}  // namespace is31fl3239
}  // namespace esphome
