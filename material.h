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
    metal(const colour& a, double fuzz) : albedo(a), fuzz(fuzz < 1 ? fuzz : 1) {}

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
    double fuzz;
};

class dielectric : public material {
  public:
    dielectric(double ri) : refraction_index(ri) {}

    bool scatter(const ray& r_in,
                        const hit_record& rec,
                        colour& attenuation,
                        ray& scattered)
    const override {
      attenuation = colour(1.0, 1.0, 1.0);
      double ri = rec.front_face ? (1.0/refraction_index) : refraction_index;

      vec3 unit_direction = unit_vector(r_in.direction());
      //  handling for total intel reflection
      double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
      double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

      bool cannot_refract = ri * sin_theta > 1.0;
      vec3 direction;

      if (cannot_refract || reflectance(cos_theta, ri) > random_double())
        direction = reflect(unit_direction, rec.normal);
      else
        direction = refract(unit_direction, rec.normal, ri);

      scattered = ray(rec.p, direction);
      return true;
    }

  private:
    double refraction_index;

    static double reflectance(double cosine, double refraction_index) {
      // this uses Schlick's approximation for reflectance (simply polynomial)
      auto r0 = (1 - refraction_index) / (1 + refraction_index);
      r0 = r0*r0;
      return r0 + (1-r0)*std::pow((1 - cosine),5);
    }

};

#endif
