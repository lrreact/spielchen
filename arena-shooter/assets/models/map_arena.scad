// Arena Shooter - Map Arena
// 50x50m arena with platforms and cover
// Units in meters

$fn = 16; // Lower for faster rendering

// Arena dimensions
arena_size = 50;
wall_height = 5;
wall_thickness = 1;

// Central structure
center_size = 8;
platform_height = 3;
pillar_size = 1.5;

// Cover crate dimensions
crate_size = 1.5;

module ground() {
    color([0.4, 0.4, 0.4])
    translate([0, 0, -0.1])
        cube([arena_size, arena_size, 0.2], center=true);
}

module walls() {
    color([0.5, 0.5, 0.5]) {
        half = arena_size / 2;

        // North wall
        translate([0, half - wall_thickness/2, wall_height/2])
            cube([arena_size, wall_thickness, wall_height], center=true);

        // South wall
        translate([0, -half + wall_thickness/2, wall_height/2])
            cube([arena_size, wall_thickness, wall_height], center=true);

        // East wall
        translate([half - wall_thickness/2, 0, wall_height/2])
            cube([wall_thickness, arena_size, wall_height], center=true);

        // West wall
        translate([-half + wall_thickness/2, 0, wall_height/2])
            cube([wall_thickness, arena_size, wall_height], center=true);
    }
}

module central_structure() {
    pillar_offset = center_size/2 - pillar_size/2;

    // Four corner pillars
    color([0.6, 0.55, 0.5]) {
        for (x = [-1, 1]) {
            for (y = [-1, 1]) {
                translate([x * pillar_offset, y * pillar_offset, (platform_height + 1)/2])
                    cube([pillar_size, pillar_size, platform_height + 1], center=true);
            }
        }
    }

    // Central platform
    color([0.55, 0.45, 0.35])
    translate([0, 0, platform_height + 0.25])
        cube([center_size, center_size, 0.5], center=true);

    // Ramp to platform (optional access)
    color([0.5, 0.4, 0.3])
    translate([center_size/2 + 2, 0, platform_height/2])
        rotate([0, -30, 0])
        cube([5, 2, 0.3], center=true);
}

module cover_crate(x, y) {
    color([0.6, 0.5, 0.3])
    translate([x, y, crate_size/2])
        cube([crate_size, crate_size, crate_size], center=true);
}

module cover_objects() {
    // Scattered crates around the arena
    // Corner positions
    cover_crate(-15, -15);
    cover_crate(-15, 15);
    cover_crate(15, -15);
    cover_crate(15, 15);

    // Side positions
    cover_crate(-10, 0);
    cover_crate(10, 0);
    cover_crate(0, -10);
    cover_crate(0, 10);

    // Diagonal positions
    cover_crate(-20, -5);
    cover_crate(20, 5);
    cover_crate(-5, -20);
    cover_crate(5, 20);
}

module spawn_markers() {
    // Visual markers for spawn points (debug/preview only)
    spawn_radius = arena_size/2 - 5;
    num_spawns = 8;

    color([0, 1, 0, 0.5])
    for (i = [0:num_spawns-1]) {
        angle = i * 360 / num_spawns;
        x = cos(angle) * spawn_radius;
        y = sin(angle) * spawn_radius;
        translate([x, y, 0.05])
            cylinder(h=0.1, r=0.5);
    }
}

// Generate the complete arena
module arena() {
    ground();
    walls();
    central_structure();
    cover_objects();
    // spawn_markers(); // Uncomment to show spawn points
}

arena();
