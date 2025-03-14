#ifndef MATERIAL_H
#define MATERIAL_H


#include "hittable.h"

class material {
  public:
    virtual ~material() = default;

    virtual bool scatter(const ray& r_in,
                        const hit_record& rec,
                        colour& attenuation,
                        ray& scattered) const {return false;}
};

class lambertian : public material {
  public:
    lambertian(const colour& a) : albedo(a) {}

    bool scatter(const ray& r_in,
                        const hit_record& rec,
                        colour& attenuation,
                        ray& scattered) 
    const override {
        vec3 scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

            
        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

  public:
    colour albedo;
};

class metal : public material {
  public:
    metal(const colour& a) : albedo(a) {}

    bool scatter(const ray& r_in,
                        const hit_record& rec,
                        colour& attenuation,
                        ray& scattered) 
    const override {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected);
        attenuation = albedo;
        return true;
    }

  public:
    colour albedo;
};

#endif
