#ifndef HITTABLE
#define HITTABLE

class material;

struct hit_record {
    public:
        point3 p;
        vec3 normal;
        shared_ptr<material> material_ptr;
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

        virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};


#endif