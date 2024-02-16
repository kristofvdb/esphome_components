# is31fl3239

This library implements rudimentary support for is31fl3239.
future improvements:
 optimize register writes in the loop
 support 16 bit mode 
 maintain led current scaling register
 
Tested with two identical components on the same i2c bus 

example configuration:

external_components:
  - source:
      type: local
      path: ../../esphome_components/


is31fl3239:
  - address: 0x34
    id: is31fl3239_1
  - address: 0x37
    id: is31fl3239_2



