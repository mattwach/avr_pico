use <mattwach/util.scad>

power_brick_width = 50;
power_brick_height = 31.3;
mount_wall_thickness = 3;
brick_to_mount_gap = 1.5;

overlap = 0.01;

module power_brick() {
  power_brick_length = 113;

  color("#222") cube([power_brick_width, power_brick_height, power_brick_length]);
}

module ac_adapter_mount() {
  cooling_vent_width = 5;
  cooling_vent_hpad = 5;
  cooling_vent_span = cooling_vent_width + cooling_vent_hpad;

  mount_zsize = 50;
  mount_xsize = power_brick_width + mount_wall_thickness * 2 + brick_to_mount_gap * 2;
  mount_ysize = power_brick_height + mount_wall_thickness * 2 + brick_to_mount_gap * 2;

  module outer_shell() {
    cube([mount_xsize, mount_ysize, mount_zsize]);
  }

  module power_brick_opening() {
    translate([
      mount_wall_thickness,
      mount_wall_thickness,
      mount_wall_thickness
    ])cube([
        power_brick_width + brick_to_mount_gap * 2,
        power_brick_height + brick_to_mount_gap * 2,
        mount_zsize - mount_wall_thickness + overlap
    ]);
  }

  module bottom_hole() {
    bottom_hole_pad = mount_wall_thickness + 5;
    translate([
        bottom_hole_pad,
        bottom_hole_pad,
        -overlap
    ]) cube([
        mount_xsize - bottom_hole_pad * 2,
        mount_ysize - bottom_hole_pad * 2,
        mount_wall_thickness + overlap * 2
    ]);
  }

  module cooling_vent() {
    cooling_vent_zpad = 6;

    module end_peg() {
      cylinder(d=cooling_vent_width, h=mount_xsize + overlap * 2);
    }

    translate([
      -overlap,
      0,
      cooling_vent_width / 2 + cooling_vent_zpad    
    ]) ry(90) hull() {
      end_peg();
      tx(-(mount_zsize - cooling_vent_width - cooling_vent_zpad * 2)) end_peg();
    }
  }

  module cooling_vents(part_width) {
    side_pad = 10;
    vent_count = floor((part_width - side_pad) / cooling_vent_span);
    overall_width = (vent_count - 1) * cooling_vent_width + (vent_count - 1) * cooling_vent_hpad;
    start_y = (part_width - overall_width) / 2;

    for (i = [0:vent_count-1]) {
      ty(start_y + i * cooling_vent_span) cooling_vent();
    }
  }

  module screw_mount() {
    hole_size = 4.5;
    hole_pad = 4;
    screw_mount_thickness = 3;

    module hole() {
      tz(-overlap) cylinder(d=hole_size, h=screw_mount_thickness + overlap * 2);
    }

    module mount() {
      angle = 120;

      module rim() {
        cylinder(d=hole_size + hole_pad * 2, h=screw_mount_thickness);
      }

      module end_point() {
        end_diameter = 2;
        tx(point_distance) cylinder(d=end_diameter, h = screw_mount_thickness);
      }

      point_distance = hole_pad + hole_size / 2 + 5;
      hull() {
        rim();
        rz(angle / 2) end_point();
        rz(-angle / 2) end_point();
      }
    }

    ty(mount_ysize) rx(90) difference() {
      mount();
      hole();
    }
  }

  module screw_mounts() {
    module row() {
      hole_gap = 26;
      tx(-6) tz(mount_zsize / 2 - hole_gap / 2) {
        screw_mount();
        tz(hole_gap) screw_mount();
      }
    }

    row();
  }

  union() {
    difference() {
      outer_shell();
      power_brick_opening();
      bottom_hole();
      cooling_vents(mount_ysize);
      tx(mount_xsize) rz(90) cooling_vents(mount_xsize);
    }
    screw_mounts();
    tx(mount_xsize) tz(mount_zsize) ry(180) screw_mounts();
  }
}

$fa=2.0;
$fs=0.5;
*translate([
    mount_wall_thickness + brick_to_mount_gap,
    mount_wall_thickness + brick_to_mount_gap,
    mount_wall_thickness
]) power_brick();
ac_adapter_mount();

