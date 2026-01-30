// Arena Shooter - Rifle Model
// Assault rifle style weapon (~80cm long)
// Units in meters

$fn = 24;

// Rifle dimensions
total_length = 0.8;
barrel_length = 0.45;
barrel_radius = 0.012;
body_length = 0.35;
body_height = 0.08;
body_width = 0.04;
stock_length = 0.2;
stock_height = 0.06;
mag_height = 0.12;
mag_width = 0.025;
mag_depth = 0.06;
grip_height = 0.1;
grip_width = 0.03;

module rifle() {
    color([0.2, 0.2, 0.2]) {
        // Main body/receiver
        translate([0, 0, 0])
            cube([body_length, body_width, body_height], center=true);

        // Barrel
        translate([body_length/2 + barrel_length/2, 0, body_height/4])
            rotate([0, 90, 0])
            cylinder(h=barrel_length, r=barrel_radius, center=true);

        // Barrel shroud/handguard
        translate([body_length/2 + barrel_length/3, 0, 0])
            cube([barrel_length * 0.6, body_width * 1.2, body_height * 0.8], center=true);

        // Stock
        translate([-body_length/2 - stock_length/2, 0, -body_height/4])
            cube([stock_length, body_width * 0.8, stock_height], center=true);

        // Stock buttpad
        translate([-body_length/2 - stock_length, 0, -body_height/4])
            cube([0.02, body_width, stock_height * 1.2], center=true);
    }

    // Magazine
    color([0.15, 0.15, 0.15])
    translate([0, 0, -body_height/2 - mag_height/2])
        cube([mag_depth, mag_width, mag_height], center=true);

    // Grip
    color([0.3, 0.2, 0.1])
    translate([-body_length/4, 0, -body_height/2 - grip_height/2])
        rotate([0, -15, 0])
        cube([grip_width, grip_width, grip_height], center=true);

    // Front sight
    color([0.1, 0.1, 0.1])
    translate([body_length/2 + barrel_length * 0.8, 0, body_height/2 + 0.015])
        cube([0.005, 0.02, 0.03], center=true);

    // Rear sight
    color([0.1, 0.1, 0.1])
    translate([body_length/4, 0, body_height/2 + 0.01])
        cube([0.02, 0.03, 0.02], center=true);

    // Trigger guard
    color([0.2, 0.2, 0.2])
    translate([-body_length/6, 0, -body_height/2])
        difference() {
            cube([0.06, body_width * 0.8, 0.04], center=true);
            cube([0.04, body_width, 0.025], center=true);
        }
}

// Generate the model (centered at origin, pointing in +X direction)
rifle();
