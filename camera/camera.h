#ifndef CAMERA_H
#define CAMERA_H

#include "primitives/hittable.h"
#include "texture/material.h"

class camera {
public:
	double aspect_ratio = 1.0;
	int image_width = 100;
	int samples_per_pixel = 10;
	int max_depth = 10;
	color background;

	double vfov = 90; // Vertical view angle (fov)
	point3 lookfrom = point3(0, 0, 0);
	point3 lookat = point3(0, 0, -1);
	vec3 vup = vec3(0, 1, 0); //Camera-relative "up" direction

	double defocus_angle = 0; //Variation angle of rays through each pixel
	double focus_dist = 10; //Distance from camera lookfrom point to plane of perfect focus


	void render(const hittable& world) {
		initialize();

		std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

		for (int j = 0; j < image_height; j++) {
			std::clog << "\rScanlines remaining: " << (image_width - j) << ' ' << std::flush;
			for (int i = 0; i < image_width; i++) {
				color pixel_color(0, 0, 0);
				for (int sample = 0; sample < samples_per_pixel; sample++) {
					ray r = get_ray(i, j);
					pixel_color += ray_color(r, max_depth, world);
				}
				write_color(std::cout, pixel_samples_scale * pixel_color);
			}
		}
		std::clog << "\rDone.                       \n";
	}

private:
	int image_height; // Rendered img height
	double pixel_samples_scale;
	point3 center; // Cam Center
	point3 pixel00_loc; // Location of pixel 0, 0
	vec3 pixel_delta_u; // Offset to pixel to right
	vec3 pixel_delta_v; // Offset to pixel below
	vec3 u, v, w; // Camera Frame Basis Vectors
	vec3 defocus_disk_u; //Defocus disk horizontal
	vec3 defocus_disk_v; //Defocus disk vertical

	void initialize() {
		image_height = int(image_width / aspect_ratio);
		image_height = (image_height < 1) ? 1 : image_height;

		pixel_samples_scale = 1.0 / samples_per_pixel;

		center = lookfrom;

		//Viewport

		auto theta = degrees_to_radians(vfov);
		auto h = std::tan(theta / 2);
		auto viewport_height = 2.0 * h * focus_dist;
		auto viewport_width = viewport_height * (double(image_width) / image_height);

		//Calc Basis Vectors
		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);


		//Horizontal and Vertical Vectors along edges

		vec3 viewport_u = viewport_width * u; // Vector across viewport horizontal edge
		vec3 viewport_v = viewport_height * -v; // Vector down viewport vertical edge 

		//Calc pixel deltas
		pixel_delta_u = viewport_u / image_width;
		pixel_delta_v = viewport_v / image_height;

		//Upper left pixel

		auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
		pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

		//Camera defocus disk basis vectors
		auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
		defocus_disk_u = u * defocus_radius;
		defocus_disk_v = v * defocus_radius;

	}

	ray get_ray(int i, int j) const {
		//Construct a camera ray originating from defocus disk and directed at randomly samples point around pixel location i, j

		auto offset = sample_square();
		auto pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);
		auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
		auto ray_direction = pixel_sample - ray_origin;


		auto ray_time = random_double();

		return ray(ray_origin, ray_direction, ray_time);
	}

	vec3 sample_square() const {
		//returns vector to random oint in [-.5, -.5] to [+.5, +.5] unit square
		return vec3(random_double() - 0.5, random_double() - 0.5, 0);
	}
	point3 defocus_disk_sample() const {
		auto p = random_in_unit_disk();
		return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
	}

	color ray_color(const ray& r, int depth, const hittable& world) const {
		if (depth <= 0)
			return color(0, 0, 0);

		hit_record rec;

		if (!world.hit(r, interval(0.001, infinity), rec))
			return background;

		ray scattered;
		color attenuation;
		color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

		if (!rec.mat->scatter(r, rec, attenuation, scattered))
			return color_from_emission;
		
		color color_from_scatter = attenuation * ray_color(scattered, depth - 1, world);
		return color_from_emission + color_from_scatter;
	}

};

#endif