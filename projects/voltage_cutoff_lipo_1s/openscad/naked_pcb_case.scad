// A Naked PCB case, which shows the components on the top but
// protects / hides the components on the bottom.

use <mattwach/util.scad>
use <mattwach/vitamins/bolts.scad>

pcb_xsize = 53.5;
pcb_ysize = 24.3;
pcb_zsize = 1.4;

case_bottom_thickness = 1;
case_pcb_zpad = 3;
case_pcb_xpad = 5;
case_pcb_ypad = 2;
case_corner_fillet = 2;
case_top_thickness = 1.5;
case_top_inset = 1;

pcb_cutout_pad = 0.2;
pcb_peg_radius = 1.8;

wire_slot_width = 8;
wire_slot_height = pcb_zsize + 1.7;
wire_slot_offset = 2;

overlap = 0.01;

//---------------------------------------------------------------------------

function bolt_y() = (pcb_ysize + case_pcb_ypad) / 2;

module pcb() {
  tz(case_bottom_thickness + case_pcb_zpad)
    color("green", 0.5) cube([pcb_xsize, pcb_ysize, pcb_zsize]);
}

module rounded_cube(zsize) {
  xsize = pcb_xsize + case_pcb_xpad * 2;
  ysize = pcb_ysize + case_pcb_ypad * 2;
  fillet = case_corner_fillet;
  module corner() {
    cylinder(r=fillet, h=zsize);
  }

  txy(-case_pcb_xpad, -case_pcb_ypad) 
    hull() {
        txy(fillet, fillet) corner();
        txy(xsize - fillet, fillet) corner();
        txy(xsize - fillet, ysize - fillet) corner();
        txy(fillet, ysize - fillet) corner();
    }
}

module case_bottom() {
  module bottom_main() {
    rounded_cube(
        case_bottom_thickness + case_pcb_zpad + pcb_zsize);
  }

  module pcb_cutout() {
    translate([
        -pcb_cutout_pad,
        -pcb_cutout_pad,,
        case_bottom_thickness]) cube([
            pcb_xsize + pcb_cutout_pad * 2,
            pcb_ysize + pcb_cutout_pad * 2,
            case_pcb_zpad + pcb_zsize + overlap]);
  }

  module pcb_peg() {
    tz(case_bottom_thickness - overlap)
      cylinder(r=pcb_peg_radius, h=case_pcb_zpad + overlap);
  }

  module stability_puck() {
    stability_puck_diameter = 25;
    fillet = 1.5;
    module side_edge() {
      union() {
        difference() {
          translate([
              -stability_puck_diameter / 2,
              -fillet,
              case_bottom_thickness - overlap]) cube([
                stability_puck_diameter,
                fillet,
                fillet]);
          translate([
              -stability_puck_diameter / 2 - overlap,
              -fillet,
              fillet + case_bottom_thickness]) ry(90) cylinder(
                r=fillet,
                h=stability_puck_diameter + overlap * 2);
        }
      }
      cylinder(d=stability_puck_diameter, h=case_bottom_thickness);
    }

    txy(pcb_xsize / 2, -case_pcb_ypad) side_edge();
    txy(pcb_xsize / 2, pcb_ysize + case_pcb_ypad) rz(180) side_edge();
  }

  module wire_slots() {
    translate([
        -case_pcb_xpad - overlap,
        wire_slot_offset,
        case_pcb_zpad + pcb_zsize + case_bottom_thickness - wire_slot_height])
      cube([
          pcb_xsize + case_pcb_xpad * 2 + overlap * 2,
          wire_slot_width,
          wire_slot_height + overlap]);
  }

  module bottom_bolt_holes() {
    module hole() {
      bottom_bolt_hole_diameter = 2.35;
      translate([
          0,
          bolt_y(),
          -overlap]) cylinder(
          d=bottom_bolt_hole_diameter,
          h=case_bottom_thickness + case_pcb_zpad + pcb_zsize + overlap * 2);

    }

    tx(-case_pcb_xpad / 2) hole();
    tx(pcb_xsize + case_pcb_xpad / 2) hole();
  }

  union() {
    difference() {
      bottom_main();
      pcb_cutout();
      wire_slots();
      bottom_bolt_holes();
    }
    txy(-pcb_cutout_pad, -pcb_cutout_pad) pcb_peg();
    txy(pcb_xsize + pcb_cutout_pad, -pcb_cutout_pad) pcb_peg();
    txy(pcb_xsize + pcb_cutout_pad, pcb_ysize + pcb_cutout_pad) pcb_peg();
    txy(-pcb_cutout_pad, pcb_ysize + pcb_cutout_pad) pcb_peg();
    stability_puck();
  }
}

module case_top() {
  module top_main() {
    rounded_cube(zsize=case_top_thickness);
  }

  module pcb_cutout() {
    translate([
        case_top_inset,
        case_top_inset,
        -overlap]) cube([
        pcb_xsize - case_top_inset * 2,
        pcb_ysize - case_top_inset * 2,
        case_top_thickness + overlap * 2]);
  }

  module wire_slot_block() {
    block_pad = 0.1;
    translate([
        -case_pcb_xpad,
        wire_slot_offset + block_pad,
        -pcb_zsize]) cube([
        case_pcb_xpad - pcb_cutout_pad,
        wire_slot_width - block_pad * 2,
        pcb_zsize + overlap]);
  }

  module top_bolt_holes() {
    module hole() {
      top_bolt_hole_diameter = 2.7;
      translate([
          0,
          bolt_y(),
          -overlap]) cylinder(
          d=top_bolt_hole_diameter,
          h=case_top_thickness + overlap * 2);

    }

    tx(-case_pcb_xpad / 2) hole();
    tx(pcb_xsize + case_pcb_xpad / 2) hole();
  }


  union() {
    difference() {
      top_main();
      pcb_cutout();
      top_bolt_holes();
    }
    wire_slot_block();
    tx(pcb_xsize + case_pcb_xpad + pcb_cutout_pad) wire_slot_block();
  }
}

$fa=2.0;
$fs=0.5;
*case_bottom();
tz(case_bottom_thickness + case_pcb_zpad + pcb_zsize) color("blue", 0.5) case_top();
*pcb();
*translate([
    0,
    bolt_y(),
    case_bottom_thickness + case_pcb_zpad + pcb_zsize + case_top_thickness]) {
  tx(-case_pcb_xpad / 2) bolt_M2_5(6);
  tx(pcb_xsize + case_pcb_xpad / 2) bolt_M2_5(6);
}
