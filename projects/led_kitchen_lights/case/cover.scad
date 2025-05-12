use <mattwach/util.scad>

overlap = 0.01;

module cover() {
  cover_xsize = 78;
  cover_ysize = 39.2;
  cover_zsize = 20;
  corner_fillet = 15;
  shell_thickness = 1.5;

  side_tab_xsize = 65;
  side_tab_ysize = 20;
  side_tab_zpad = 1;

  slot_width = 2;

  module block(pad=0) {
    module corner() {
      cube([1, 1, cover_zsize - pad]);
    }
    hull() {
      txy(corner_fillet + pad, corner_fillet + pad) cylinder(r=corner_fillet, h=cover_zsize - pad);
      txy(cover_xsize - 1 , pad) corner();
      txy(pad, cover_ysize - 1) corner();
      txy(cover_xsize - 1, cover_ysize - 1) corner();
    }
  }

  module side_tab() {
    fillet_size = 6;
    module side_tab_fillet() {
      difference() {
        ty(side_tab_ysize - fillet_size) tz(-fillet_size)
          cube([side_tab_xsize, fillet_size, fillet_size]);
        tx(-overlap) ty(side_tab_ysize - fillet_size) tz(-fillet_size) ry(90)
          cylinder(r=fillet_size, h=side_tab_xsize + overlap * 2);
      }
    }
    union() {
      cube([side_tab_xsize, side_tab_ysize, shell_thickness]);
      side_tab_fillet();
    }
  }

  module slots() {
    slot_count = 5;
    slot_span = 25;
    module slot(length) {
      module edge() {
        cylinder(d=slot_width, h=shell_thickness + overlap * 3);
      }

      translate([
          slot_width / 2,
          slot_width / 2,
          -overlap
      ]) hull() {
        edge();
        tx(length - slot_width) edge();
      }
    }

    for (i=[0:slot_count - 1]) {
      txy(
          15,
          (cover_ysize - slot_span) / 2 + i * slot_span / (slot_count - 1)
      ) slot(cover_xsize - 20);
    }
  }

  module cable_hole() {
    cable_hole_radius = 6;
    cable_hole_yoffset = 22;
    translate([
        -overlap,
        cable_hole_yoffset,
        cover_zsize
    ]) ry(90) cylinder(r=cable_hole_radius, h=shell_thickness * 2);
  }

  union() {
    difference() {
      block();
      translate([
          overlap,
          overlap,
          shell_thickness + overlap]) block(pad=shell_thickness);
      cable_hole();
      slots();
    }
    translate([
        cover_xsize - side_tab_xsize,
        -side_tab_ysize + overlap,
        cover_zsize - shell_thickness - side_tab_zpad
    ]) side_tab();
  }
}

$fa=2;
$fs=0.5;

cover();
