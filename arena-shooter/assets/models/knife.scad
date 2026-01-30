// Arena Shooter - Knife Model
// Combat knife (~25cm total length)
// Units in meters

$fn = 24;

// Knife dimensions
total_length = 0.25;
blade_length = 0.15;
blade_width = 0.025;
blade_thickness = 0.003;
handle_length = 0.1;
handle_width = 0.025;
handle_thickness = 0.02;
guard_width = 0.04;
guard_thickness = 0.008;

module knife() {
    // Blade
    color([0.8, 0.8, 0.85])
    translate([blade_length/2, 0, 0])
        hull() {
            // Blade base
            cube([0.01, blade_width, blade_thickness], center=true);

            // Blade tip
            translate([blade_length - 0.01, 0, 0])
                scale([1, 0.3, 0.5])
                cube([0.01, blade_width, blade_thickness], center=true);
        }

    // Blade spine (thicker top edge)
    color([0.7, 0.7, 0.75])
    translate([blade_length/2, blade_width/2 - blade_thickness, 0])
        cube([blade_length * 0.9, blade_thickness * 2, blade_thickness * 1.5], center=true);

    // Guard
    color([0.3, 0.3, 0.3])
    translate([0, 0, 0])
        cube([guard_thickness, guard_width, handle_thickness * 1.2], center=true);

    // Handle
    color([0.4, 0.25, 0.1])
    translate([-handle_length/2, 0, 0])
        scale([1, 0.8, 1])
        hull() {
            // Handle front
            translate([handle_length/2 - 0.01, 0, 0])
                cube([0.01, handle_width, handle_thickness], center=true);

            // Handle middle (slightly wider)
            translate([0, 0, 0])
                cube([0.01, handle_width * 1.1, handle_thickness * 1.1], center=true);

            // Handle back
            translate([-handle_length/2 + 0.01, 0, 0])
                cube([0.01, handle_width * 0.9, handle_thickness * 0.9], center=true);
        }

    // Pommel
    color([0.3, 0.3, 0.3])
    translate([-handle_length - 0.01, 0, 0])
        sphere(r=handle_thickness * 0.6);
}

// Generate the model (centered, blade pointing in +X direction)
knife();
