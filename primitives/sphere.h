#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

class sphere : public hittable {
public:
	//stationary sphere
	sphere(const point3& static_center, double radius, shared_ptr<material> mat) : 
		center(static_center, vec3(0,0,0)), radius(std::fmax(0, radius)), mat(mat) {
		auto rvec = vec3(radius, radius, radius);
		bbox = axisallignedBB(static_center - rvec, static_center + rvec);
	}

	sphere(const point3& center1, const point3& center2, double radius, shared_ptr<material> mat):
	center(center1, center2 - center1), radius(std::fmax(0, radius)), mat(mat){
		auto rvec = vec3(radius, radius, radius);
		axisallignedBB box1(center.at(0) - rvec, center.at(0) + rvec);
		axisallignedBB box2(center.at(1) - rvec, center.at(1) + rvec);
		bbox = axisallignedBB(box1, box2);
	}


	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
		point3 current_center = center.at(r.time());
		vec3 oc = current_center - r.origin();
		auto a = r.direction().length_squared();
		auto h = dot(r.direction(), oc);
		auto c = oc.length_squared() - radius * radius;

		auto discriminant = h * h - a * c;
		if (discriminant < 0)
			return false;

		auto sqrtd = std::sqrt(discriminant);

		//Find the nearest root in the range
		auto root = (h - sqrtd) / a;
		if (!ray_t.surrounds(root)) {
			root = (h + sqrtd) / a;
			if (!ray_t.surrounds(root))
				return false;
		}
		rec.t = root;
		rec.p = r.at(rec.t);
		rec.normal = (rec.p - current_center) / radius;
		vec3 outward_normal = (rec.p - current_center) / radius;
		rec.set_face_normal(r, outward_normal);
		get_sphere_uv(outward_normal, rec.u, rec.v);
		rec.mat = mat;

		return true;
	}

	axisallignedBB bounding_box() const override { return bbox; }

private:

	static void get_sphere_uv(const point3& p, double& u, double& v) {
		//p: given point on sphere of radius 1 at origin
		//u: return val [0, 1] of angle around Y axis from X = -1.
		//v: return val [0, 1] of angle from Y= -1 to Y = +1.


		auto theta = std::acos(-p.y());
		auto phi = std::atan2(-p.z(), p.x()) + pi;

		u = phi / (2 * pi);
		v = theta / pi;
	}

	ray center;
	double radius;
	shared_ptr<material> mat;
	axisallignedBB bbox;
};

#endif