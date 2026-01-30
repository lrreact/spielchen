// Arena Shooter - Player Model
// Simple humanoid figure (~1.8m tall)
// Units in meters

$fn = 32; // Smoothness

// Player dimensions
player_height = 1.8;
body_width = 0.4;
body_depth = 0.25;
body_height = 0.7;
head_radius = 0.12;
arm_length = 0.6;
arm_radius = 0.05;
leg_length = 0.8;
leg_radius = 0.07;

// Colors (for preview, ignored in OBJ export)
body_color = [0.2, 0.4, 0.8];
head_color = [0.9, 0.7, 0.6];

module player() {
    // Legs (bottom)
    translate([0, 0, leg_length/2]) {
        // Left leg
        translate([-0.1, 0, 0])
            color([0.3, 0.3, 0.3])
            cylinder(h=leg_length, r=leg_radius, center=true);

        // Right leg
        translate([0.1, 0, 0])
            color([0.3, 0.3, 0.3])
            cylinder(h=leg_length, r=leg_radius, center=true);
    }

    // Body (torso)
    translate([0, 0, leg_length + body_height/2])
        color(body_color)
        scale([body_width, body_depth, body_height])
        sphere(r=0.5);

    // Arms
    translate([0, 0, leg_length + body_height * 0.8]) {
        // Left arm
        translate([-(body_width/2 + arm_radius), 0, 0])
            rotate([0, 15, 0])
            color(body_color)
            cylinder(h=arm_length, r=arm_radius, center=true);

        // Right arm
        translate([(body_width/2 + arm_radius), 0, 0])
            rotate([0, -15, 0])
            color(body_color)
            cylinder(h=arm_length, r=arm_radius, center=true);
    }

    // Neck
    translate([0, 0, leg_length + body_height])
        color(head_color)
        cylinder(h=0.1, r=0.05);

    // Head
    translate([0, 0, leg_length + body_height + head_radius + 0.1])
        color(head_color)
        sphere(r=head_radius);
}

// Generate the model
player();
