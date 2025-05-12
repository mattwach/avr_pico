use <mattwach/util.scad>
use <mattwach/vitamins/electronics/buttons.scad>
use <mattwach/vitamins/electronics/capacitor.scad>
use <mattwach/vitamins/electronics/voltage_regulators.scad>
use <mattwach/vitamins/rc/jst.scad>
include <NopSCADlib/core.scad>
include <NopSCADlib/vitamins/axials.scad>
include <NopSCADlib/vitamins/components.scad>
include <NopSCADlib/vitamins/dip.scad>
include <NopSCADlib/vitamins/pcbs.scad>
include <NopSCADlib/vitamins/pin_headers.scad>

overlap = 0.01;

module pcb_module() {
  board_x_offset = 5.85;
  board_y_offset = 3.5;
  board_width = 1.6;
  pin_spacing = 2.54;
  wire_diameter = 1.8;

  function cx(x) = board_x_offset + x * pin_spacing;
  function cy(y) = board_y_offset + y * pin_spacing;

  module tiny85_socket() {
    translate([cx(8.5), cy(4.5), board_width]) dil_socket(4, 7.62);
  }

  module tiny85() {
    translate([cx(8.5), cy(4.5), board_width + 3.0])
      pdip(8, "ATTiny85", false);
  }

  module main_board() {
    pcb_length = 54.4;
    pcb_width = 30;
    difference() {
      txy(35, 15) pcb(PERF70x30);
      translate([
          pcb_length,
          -overlap,
          -0.2
      ]) cube([
          70 - pcb_length + overlap,
          30 + overlap * 2,
          2.0]);
    }
  }

  module capacitor(label_text) {
    electrolytic_capacitor(
        diameter=5,
        height=11.2,
        pin_spacing=2.54,
        label_text=label_text,
        hover=0);
  }

  module capacitor_1uf() {
    translate([cx(4), cy(0), board_width + 3]) ry(-90) capacitor("1uF");
  }

  module capacitor_2_2uf() {
    translate([cx(11), cy(1), board_width + 3]) rx(180) ry(90) capacitor("2.2uF");
  }

  module capacitor_100n() {
    translate([cx(11), cy(6), board_width]) rz(-90) ceramic_capacitor("100n");
  }

  module resistor_1k() {
    ax_res(res1_4, 1000);
  }

  module vreg() {
    translate([cx(7.5), cy(1), board_width ]) rz(270) small_vreg();
  }

  module push_switch() {
    translate([cx(0), cy(5), board_width]) rz(-90) push_switch_12x12();
  }

  module mosfet(xrot) {
    translate([
        23.5,
        6,
        board_width + 2
    ]) ry(-5) rx(xrot) rz(-90)TO220("N-FET", lead_length=5.6);
  }

  module wire_pair(invert_colors=false, wire_length=40, add_connector=true) {
    left_color = invert_colors ? "red" : "#222";
    right_color = invert_colors ? "#222" : "red";

    module wire() {
      ry(90) cylinder(d=wire_diameter, h=wire_length);
    }

    module connector() {
      translate([
          wire_length - 3,
          -0.3,
          0
      ]) jst_male();
    }

    color(left_color) wire();
    color(right_color) ty(wire_diameter) wire();
    if (add_connector) {
      connector();
    }
  }

  main_board();
  capacitor_1uf();
  capacitor_2_2uf();
  capacitor_100n();
  vreg();
  push_switch();
  tiny85_socket();
  tiny85();
  translate([cx(12.5), cy(3), board_width]) resistor_1k();
  translate([cx(12.5), cy(4), board_width]) resistor_1k();
  txy(pin_spacing * 16, pin_spacing * 0.9) mosfet(0);
  translate([
      pin_spacing * 16,
      pin_spacing * 5.1,
      4.5
  ]) mosfet(180);
  translate([cx(16), cy(1), -wire_diameter / 2]) wire_pair();
  translate([cx(16), cy(3), -wire_diameter / 2]) wire_pair(true, add_connector=false);
  translate([cx(16), cy(5), -wire_diameter / 2]) wire_pair();
  translate([cx(16), cy(7), -wire_diameter / 2]) wire_pair(true, add_connector=false);
  translate([-7, 6.5, -wire_diameter / 2]) wire_pair(true, 10, false);
}

$fs=0.5;
$fa=2.0;
pcb_module();
