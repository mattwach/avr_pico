# avr_pico
My AVR libraries I've created over the years.  Many of them have been augmented
to be usable with Pi Pico as well.

Everything is `C` based and no external frameworks are needed.

You'll need to install `avr-gcc` yourself before using.  Some optional tooling
also needs Python3. Only tested on Linux but it should also work in other OSes
with approprate tweaking.

# Getting Started

```
cd examples/blink_atmel328p
make
```

# Other Examples (As of 2025-5-12)

```
.
├── adc
│   └── entropy
├── atmega_pwm
├── blink_32k_osc
├── blink_atmel328p
├── blink_attiny84
├── blink_attiny85
├── digital_pot_sweep
├── dotstar
│   ├── basic
│   └── counter
├── ds3231
├── epaper
│   ├── bitmap
│   ├── counter
│   ├── lowlevel
│   └── text_16x32
├── ina260
├── ina260_with_oled
├── lowpower
├── ms8607
├── ms8607_with_32k_osc
├── nmea_oled
├── oledm
│   ├── adc_plot
│   ├── bitmap
│   ├── console
│   ├── console_lowlevel
│   ├── count_one_million
│   ├── display_on_off
│   ├── graph_display
│   ├── horiz_fill
│   ├── lowlevel
│   ├── ssd1780
│   │   ├── bitmap
│   │   ├── color_index
│   │   ├── console
│   │   ├── lowlevel
│   │   ├── monochrome_lowlevel
│   │   ├── mono_horiz_fill
│   │   ├── special_commands
│   │   ├── text_8x16
│   │   ├── text_basic
│   │   └── vscroll_lowlevel
│   ├── text_8x16
│   ├── text_basic
│   ├── variable_font
│   └── vscroll_lowlevel
├── petit_fatfs
│   └── cat_readme
├── rf24
│   ├── test_receiver
│   ├── test_sender
│   └── test_sender_oled
├── rotary_encoder
├── servo
│   ├── atmega_multi_sweep
│   ├── atmega_servo_pot
│   ├── timy85_pot_servo_control_with_display
│   └── tiny85_auto_servo_control
├── software_uart
│   ├── echo
│   ├── echo_external
│   └── reliability
├── test_input_attiny_85
├── time
│   ├── passive_buzzer
│   ├── stop_watch
│   └── stop_watch_big
├── twi_rtc_lowlevel
├── uart
└── uart_recv_oled
```

Also some more complex projects:

```
battery_discharge_monitor  light_bulb_changer.txt         simple_soil_sensor_nrf2401  voltage_meter_lipo_1s_v2
coded_door                 motion_detector_rf24_receiver  square_wave_generator       voltage_monitor_for_siggen
epaper_clock_readme.txt    motion_detector_rf24_sender    truck_backup_beeper
led_kitchen_lights         servo_arm_draw                 voltage_cutoff_lipo_1s
led_lamp_readme.txt        simple_soil_sensor             voltage_meter_lipo_1s
```
