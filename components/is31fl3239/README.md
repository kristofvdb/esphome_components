# is31fl3239

## About
This library implements rudimentary support for is31fl3239. It needs I2C support.
It is largely based on the code for TLC59208F and should be similar in terms of functionality

future improvements:
- optimize register writes in the loop
- support 16 bit mode 
- maintain led current scaling register
- support diagnostic functions
- add sleep mode (through enable function)
 
## Configuration

Tested with two identical components on the same i2c bus 

example configuration:

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/kristofvdb/esphome_components
    components: [ is31fl3239 ]

is31fl3239:
  - address: 0x34
    id: is31fl3239_1
  - address: 0x37
    id: is31fl3239_2

# example of two RGB lights on 2 different IC's

# expose the output channels
output:
  - platform: is31fl3239
    id: 'is31fl3239_1_ch0'
    channel: 0
    is31fl3239_id: 'is31fl3239_1'
  - platform: is31fl3239
    id: 'is31fl3239_1_ch1'
    channel: 1
    is31fl3239_id: 'is31fl3239_1'
  - platform: is31fl3239
    id: 'is31fl3239_1_ch2'
    channel: 2
    is31fl3239_id: 'is31fl3239_1'
  - platform: is31fl3239
    id: 'is31fl3239_2_ch0'
    channel: 0
    is31fl3239_id: 'is31fl3239_2'
  - platform: is31fl3239
    id: 'is31fl3239_2_ch1'
    channel: 1
    is31fl3239_id: 'is31fl3239_2'
  - platform: is31fl3239
    id: 'is31fl3239_2_ch2'
    channel: 2
    is31fl3239_id: 'is31fl3239_2'

# example of the two RGB lights
# sidenote: grouping can be done on a higher level
#  by creating "light groups" (helper) 
light:
  - platform: rgb
    name: "Led8"
    red: is31fl3239_1_ch1
    green: is31fl3239_1_ch0
    blue: is31fl3239_1_ch2
    id: led8
  - platform: rgb
    name: "Led12"
    green: is31fl3239_2_ch1
    red: is31fl3239_2_ch0
    blue: is31fl3239_2_ch2
    id: led12
```
