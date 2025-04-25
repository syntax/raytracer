#ifndef SPHERE
#define SPHERE

#include "hittable.h"
#include <memory>

class sphere : public hittable {
    public:
        sphere(const point3& center, double radius, std::unique_ptr<material> material_ptr) : center(center), radius(std::fmax(0,radius)), material_ptr(std::move(material_ptr)) {
        }

        virtual bool bounding_box(aabb& output_box) const override {
            output_box = aabb(
              center - vec3(radius, radius, radius),
              center + vec3(radius, radius, radius)
            );
            return true;
        }

        virtual bool hit(const ray& r, const interval& ray_t, hit_record& rec) const override {
            vec3 oc = center - r.origin();
            auto a = r.direction().length_squared();
            auto h = dot(r.direction(), oc);
            auto c = oc.length_squared() - radius*radius;
    
            auto discriminant = h*h - a*c;
            if (discriminant < 0)
                return false;
            
            // TODO: I want to use the quake3 fast inverse square root here
            auto sqrtd = std::sqrt(discriminant);
    
            // find the nearest root that lies in the acceptable range.
            auto root = (h - sqrtd) / a;
            if (!ray_t.surrounds(root)) {
                root = (h + sqrtd) / a;
                if (!ray_t.surrounds(root))
                    return false;
            }
    
            rec.t = root;
            rec.p = r.at(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.set_face_normal(r, rec.normal);
            rec.material_ptr = material_ptr.get();
    
            return true;
        }
    
    private:
        point3 center;
        double radius;    
        std::unique_ptr<material> material_ptr;
};

#endif