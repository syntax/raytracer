#ifndef HITTABLE
#define HITTABLE

#include "aabb.h"

class material;

struct hit_record {
    public:
        point3 p;
        vec3 normal;
        const material* material_ptr;
        double t;
        bool front_face;

        inline void set_face_normal(const ray& r, const vec3& outward_normal) {
            front_face = dot(r.direction(), outward_normal) < 0;
            normal = front_face ? outward_normal : -outward_normal;
        }
};

class hittable {
    public:
        virtual ~hittable() = default;

        virtual bool hit(const ray& r, const interval& ray_t, hit_record& rec) const = 0;

        virtual bool bounding_box(aabb& output_box) const = 0;
};


#endif