#ifndef BOUNDINGBOXES_H
#define BOUNDINGBOXES_H

class axisallignedBB {
public:
	interval x, y, z;

	axisallignedBB() {}

	axisallignedBB(const interval& x, const interval& y, const interval& z) : x(x), y(y), z(z) {
		pad_to_minimums();
	}

	axisallignedBB(const point3& a, const point3& b) {
		x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
		y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
		z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);

		pad_to_minimums();
	}

	axisallignedBB(const axisallignedBB& box1,const axisallignedBB& box2) {
		x = interval(box1.x, box2.x);
		y = interval(box1.y, box2.y);
		z = interval(box1.z, box2.z);
	}

	const interval& axis_interval(int n) const {
		if (n == 1) return y;
		if (n == 2) return z;
		return x;
	}

	int longest_axis() const {
		if (x.size() > y.size()) {
			return x.size() > z.size() ? 0 : 2;
		}
		else
			return y.size() > z.size() ? 1 : 2;
	}

	static const axisallignedBB empty, universe;

	bool hit(const ray& r, interval ray_t) const {
		const point3& ray_origin = r.origin();
		const vec3& ray_dir = r.direction();

		for (int axis = 0; axis < 3; axis++) {
			const interval& ax = axis_interval(axis);
			const double adinv = 1.0 / ray_dir[axis];

			auto t0 = (ax.min - ray_origin[axis]) * adinv;
			auto t1 = (ax.max - ray_origin[axis]) * adinv;

			if (t0 < t1) {
				if (t0 > ray_t.min) ray_t.min = t0;
				if (t1 < ray_t.max) ray_t.max = t1;
			}
			else {
				if (t1 > ray_t.min) ray_t.min = t1;
				if (t0 < ray_t.max) ray_t.max = t0;
			}

			if (ray_t.max <= ray_t.min)
				return false;
		}
		return true;
	}
private:
	void pad_to_minimums() {
		double delta = 0.0001;
		if (x.size() < delta) x = x.expand(delta);
		if (y.size() < delta) y = y.expand(delta);
		if (z.size() < delta) z = z.expand(delta);
	}
};

const axisallignedBB axisallignedBB::empty = axisallignedBB(interval::empty, interval::empty, interval::empty);
const axisallignedBB axisallignedBB::universe = axisallignedBB(interval::universe, interval::universe, interval::universe);

axisallignedBB operator+(const axisallignedBB& bbox, const vec3& offset) {
	return axisallignedBB(bbox.x + offset.x(), bbox.y + offset.y(), bbox.z + offset.z());
}

axisallignedBB operator+(const vec3& offset, const axisallignedBB& bbox) {
	return bbox + offset;
}

#endif