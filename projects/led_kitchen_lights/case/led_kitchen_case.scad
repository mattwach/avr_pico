use <mattwach/util.scad>
use <mattwach/vitamins/electronics/barrel_connector.scad>
use <mattwach/vitamins/bolts.scad>
use <pcb_module.scad>

module led_kitchen_case() {
  width = 39;
  length = 75;
  height = 18;

  base_thickness = 2.7;
  cover_thickness = 2;
  cover_bolt_hole_size = 1.8;
  base_height = height - cover_thickness;
  corner_fillet = 5;
  pcb_back_offset = 15;
  pcb_bolt_length = 7.8;
  pcb_base_gap = 3.3;
  pcb_hole_size = 1.8;
  pcb_hole_spacing = 26;
  pcb_hole_xoffset = 2;
  pcb_thickness = 1.6;
  pcb_width = 30;
  wall_thickness = 4;
  barrel_connector_z = base_thickness + 7;
  barrel_connector_y = (width - pcb_width) / 2 + 9;
  cover_bolt_inset_x = 5;
  cover_bolt_inset_y = wall_thickness / 2;
  cover_bolt_inset_z = 1;

  overlap = 0.01;

  module slice(inset, slice_height) {
    hull() {
      txy(corner_fillet + inset, corner_fillet + inset)
        cylinder(r=corner_fillet, h=slice_height);
      txy(length - corner_fillet - inset, corner_fillet + inset)
        cylinder(r=corner_fillet, h=slice_height);
      txy(length - corner_fillet - inset, width - corner_fillet - inset)
        cylinder(r=corner_fillet, h=slice_height);
      txy(corner_fillet + inset, width - corner_fillet - inset)
        cylinder(r=corner_fillet, h=slice_height);
    }
  }

  module base() {
    module end_port() {
      end_port_width = pcb_width + 1;
      end_port_fillet = 3;
      end_port_thickness = wall_thickness + corner_fillet + overlap * 2;
      translate([
          length - wall_thickness - corner_fillet,
          (width - end_port_width) / 2,
          0,
      ]) hull() {
        tz(base_height - 1) cube([
          end_port_thickness,
          end_port_width,
          1.1
        ]);
        ty(end_port_fillet) tz(base_thickness + end_port_fillet) ry(90)
          cylinder(r=end_port_fillet, h=end_port_thickness);
        ty(end_port_width - end_port_fillet) tz(base_thickness + end_port_fillet) ry(90)
          cylinder(r=end_port_fillet, h=end_port_thickness);
      } 
    }

    module barrel_connector_slot() {
      barrel_diameter = 7.2;
      translate([
        0,
        barrel_connector_y,
        barrel_connector_z
      ]) union() {
        ry(90) cylinder(d=barrel_diameter, h=wall_thickness + overlap * 2);
        ty(-barrel_diameter / 2) cube([
            wall_thickness + overlap * 2,
            barrel_diameter,
            base_height - barrel_connector_z + overlap
        ]);
      }
    }

    module pcb_holder() {
      bolt_support_diameter = 5;
      support_inset = 2;

      module support() {
        translate([
            pcb_back_offset,
            wall_thickness,
            base_thickness - overlap
        ]) cube([length - pcb_back_offset, support_inset, pcb_base_gap + overlap]);
      }

      module bolt_support() {
        translate([
            pcb_back_offset + pcb_hole_xoffset,
            (width - pcb_hole_spacing) / 2,
            base_thickness - overlap
        ]) cylinder(d=bolt_support_diameter, h=pcb_base_gap + overlap);
      }

      support();
      ty(width - wall_thickness * 2 - support_inset) support();
      bolt_support();
      ty(pcb_hole_spacing) bolt_support();
    }

    module pcb_nut_hole() {
      nut_depth = 2.2;
      translate([
          pcb_back_offset + pcb_hole_xoffset,
          (width - pcb_hole_spacing) / 2,
          -overlap
      ]) union() {
        solid_nut(4.5, nut_depth + overlap);
        tz(nut_depth) cylinder(
            d=pcb_hole_size,
            h=base_thickness + pcb_base_gap - nut_depth + overlap * 2);
      }
    }

    module base_cover_bolt_hole() {
      tz(-overlap) cylinder(d=cover_bolt_hole_size, h=base_height + overlap * 2);
    }

    module zip_tie_interface() {
      zip_tie_slot_width = 2;
      zip_tie_slot_depth = 1.5;
      zip_tie_hole_diameter = 2.5;
      zip_tie_hole_spacing = 20;
      zip_tie_hole_offset = 3;
      module zip_tie_hole() {
        translate([
            length - zip_tie_hole_offset,
            width / 2,
            -overlap
        ]) cylinder(d=zip_tie_hole_diameter, h=base_thickness + overlap * 2);
      }
      ty(zip_tie_hole_spacing / 2) zip_tie_hole();
      ty(-zip_tie_hole_spacing / 2) zip_tie_hole();
      translate([
          length - zip_tie_hole_offset - zip_tie_slot_width / 2,
          (width - zip_tie_hole_spacing) / 2,
          -overlap
      ]) cube([zip_tie_slot_width, zip_tie_hole_spacing, zip_tie_slot_depth + overlap]);
    }

    difference() {
      union() {
        difference() {
          slice(0, base_height);
          tz(base_thickness) slice(wall_thickness, base_height - base_thickness + overlap);
          end_port();
          barrel_connector_slot();
          ty(pcb_hole_spacing) pcb_nut_hole();
        }
        pcb_holder();
      }
      zip_tie_interface();
      pcb_nut_hole();
      ty(pcb_hole_spacing) pcb_nut_hole();
      txy(cover_bolt_inset_x, cover_bolt_inset_y) base_cover_bolt_hole();
      txy(length - cover_bolt_inset_x, cover_bolt_inset_y) base_cover_bolt_hole();
      txy(length - cover_bolt_inset_x, width - cover_bolt_inset_y) base_cover_bolt_hole();
      txy(cover_bolt_inset_x, width - cover_bolt_inset_y) base_cover_bolt_hole();
    }
  }

  module cover() {
    module plate() {
      color("blue") tz(base_height) slice(0, cover_thickness);
    }

    module button_hole() {
      button_hole_diameter = 15;
      button_hole_offset_x = pcb_back_offset + 12.3;
      button_hole_offset_y = (width - pcb_width) / 2 + 13.6;
      translate([
          button_hole_offset_x,
          button_hole_offset_y,
          base_height - overlap
      ]) cylinder(d=button_hole_diameter, h=cover_thickness + overlap * 2);
    }

    module cover_bolt_hole() {
      module bolt_hole_and_head() {
        module bolt_hole() {
          cylinder(d=2.1, h=cover_thickness + overlap);
        }
        module bolt_head() {
          cylinder(d=6, h=cover_bolt_inset_z + overlap * 2);
        }
        tz(base_height - overlap) union() {
          bolt_hole();
          tz(cover_thickness - cover_bolt_inset_z) bolt_head();
        }
      }
      bolt_hole_and_head();
    }

    difference() {
      plate();
      button_hole();
      txy(cover_bolt_inset_x, cover_bolt_inset_y) cover_bolt_hole();
      txy(length - cover_bolt_inset_x, cover_bolt_inset_y) cover_bolt_hole();
      txy(length - cover_bolt_inset_x, width - cover_bolt_inset_y) cover_bolt_hole();
      txy(cover_bolt_inset_x, width - cover_bolt_inset_y) cover_bolt_hole();
    }
  }

  module pcb() {
    translate([
        pcb_back_offset,
        (width - pcb_width) / 2,
        base_thickness + pcb_base_gap
    ]) pcb_module();
  }

  module barrel_connector() {
    ty(barrel_connector_y)
      tz(barrel_connector_z) rx(90) ry(-90) barrel_mount_female_5_5(hex_nut_gap=wall_thickness);
  }

  module pcb_bolt() {
    translate([
            pcb_back_offset + pcb_hole_xoffset,
            (width - pcb_hole_spacing) / 2,
            0
    ]) {
      tz(base_thickness + pcb_base_gap + pcb_thickness)
        bolt_M2(pcb_bolt_length);
      color("#ddd") nut_M2();
    }
  }

  module cover_bolt() {
    cover_bolt_length = 12.2;
    tz(height - cover_bolt_inset_z) bolt_M2(cover_bolt_length);
  }

  base();
  !cover();
  txy(cover_bolt_inset_x, cover_bolt_inset_y) cover_bolt();
  txy(length - cover_bolt_inset_x, cover_bolt_inset_y) cover_bolt();
  txy(length - cover_bolt_inset_x, width - cover_bolt_inset_y) cover_bolt();
  txy(cover_bolt_inset_x, width - cover_bolt_inset_y) cover_bolt();
  pcb();
  barrel_connector();
  pcb_bolt();
  ty(pcb_hole_spacing) pcb_bolt();
}

$fa=2.0;
$fs=0.5;
led_kitchen_case();
