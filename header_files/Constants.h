#pragma once

#include <vector>

namespace Constants
{
    
    constexpr float sun_radius = 695700000;
    constexpr float sun_size = 100.0f; // --- 
    constexpr float sun_revolution_speed = 1.0f;
    constexpr float sun_distance = 0.0f;

    constexpr float earth_radius = 6378.137f;
    //constexpr float earth_size = earth_radius * (sun_size / sun_radius);
    constexpr float earth_size = 10.0f; // --
    constexpr float earth_distance_from_sun = 150196428.0f;
    constexpr float earth_distance = 500.0f; // --
    constexpr float earth_year = 365.0f;
    constexpr float earth_revolution_speed = 0.3f;

    constexpr float moon_radius = 1738.1f;
    constexpr float moon_size = moon_radius * (earth_size / earth_radius);
    constexpr float moon_distance = 15.0f;
    constexpr float moon_year =  27.3f;
    constexpr float moon_revolution_speed = earth_revolution_speed * (earth_year / moon_year);

    constexpr float mercury_radius = 2439.137f;
    constexpr float mercury_size = mercury_radius * (earth_size / earth_radius);
    constexpr float mercury_distance_from_sun = 58000000.0f;
    constexpr float mercury_distance = mercury_distance_from_sun * (earth_distance / earth_distance_from_sun);
    constexpr float mercury_year = 88.0f;
    constexpr float mercury_revolution_speed = earth_revolution_speed * (earth_year / mercury_year);

    constexpr float venus_radius = 6051.137f;
    constexpr float venus_size = venus_radius * (earth_size / earth_radius);
    constexpr float venus_distance_from_sun = 108000000.0f;
    constexpr float venus_distance = venus_distance_from_sun * (earth_distance / earth_distance_from_sun);
    constexpr float venus_year = 225.0f;
    constexpr float venus_revolution_speed = earth_revolution_speed * (earth_year / venus_year);

    constexpr float mars_radius = 3396.2f;
    constexpr float mars_size = mars_radius * (earth_size / earth_radius);
    constexpr float mars_distance_from_sun = 228000000.0f;
    constexpr float mars_distance = mars_distance_from_sun * (earth_distance / earth_distance_from_sun);
    constexpr float mars_year = 687.0f;
    constexpr float mars_revolution_speed = earth_revolution_speed * (earth_year / mars_year);

    constexpr float jupiter_radius = 69911.0f;
    constexpr float jupiter_size = jupiter_radius * (earth_size / earth_radius);
    constexpr float jupiter_distance_from_sun = 778000000.0f;
    constexpr float jupiter_distance = jupiter_distance_from_sun * (earth_distance / earth_distance_from_sun);
    constexpr float jupiter_year = 4333.0f;
    constexpr float jupiter_revolution_speed = earth_revolution_speed * (earth_year / jupiter_year);

    constexpr float saturn_radius = 60250.0f;
    constexpr float saturn_size = saturn_radius * (earth_size / earth_radius);
    constexpr float saturn_distance_from_sun = 1400000000.0f;
    constexpr float saturn_distance = saturn_distance_from_sun * (earth_distance / earth_distance_from_sun);
    constexpr float saturn_year = 10756.0f;
    constexpr float saturn_revolution_speed = earth_revolution_speed * (earth_year / saturn_year);

    constexpr float uranus_radius = 25559.0f;
    constexpr float uranus_size = uranus_radius * (earth_size / earth_radius);
    constexpr float uranus_distance_from_sun = 2900000000.0f;
    constexpr float uranus_distance = uranus_distance_from_sun * (earth_distance / earth_distance_from_sun);
    constexpr float uranus_year = 30687.0f;
    constexpr float uranus_revolution_speed = earth_revolution_speed * (earth_year / uranus_year);

    constexpr float neptune_radius = 24764.0f;
    constexpr float neptune_size = neptune_radius * (earth_size / earth_radius);
    constexpr float neptune_distance_from_sun = 4500000000.0f;
    constexpr float neptune_distance = neptune_distance_from_sun * (earth_distance / earth_distance_from_sun);
    constexpr float neptune_year = 60190.0f;
    constexpr float neptune_revolution_speed = earth_revolution_speed * (earth_year / neptune_year);

    const std::vector<float> Orbit_Ratios{1.0f, mercury_distance/earth_distance, \
    venus_distance/earth_distance, mars_distance/earth_distance, \
    jupiter_distance/earth_distance, saturn_distance/earth_distance, \
    uranus_distance/earth_distance, neptune_distance/earth_distance};

    const std::vector<float> Planet_Sizes{sun_size, earth_size, moon_size, mercury_size, \
    venus_size, mars_size, jupiter_size, saturn_size, uranus_size, neptune_size};

    const std::vector<float> Planet_Revolution_Speeds{sun_revolution_speed, earth_revolution_speed, \
    moon_revolution_speed, mercury_revolution_speed, venus_revolution_speed, \
    mars_revolution_speed, jupiter_revolution_speed, saturn_revolution_speed, \
    uranus_revolution_speed, neptune_revolution_speed};

    const std::vector<float> Planet_Distances{sun_distance, earth_distance, moon_distance, \
    mercury_distance, venus_distance, mars_distance, jupiter_distance, \
    saturn_distance, uranus_distance, neptune_distance};
}