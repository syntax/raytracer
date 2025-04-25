#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include <vector>
#include <memory>

class hittable_list : public hittable {
  public:
    std::vector<std::shared_ptr<hittable>> objects;

    hittable_list() {}

    void add(std::shared_ptr<hittable> obj) { objects.push_back(obj); }

    virtual bool hit(const ray& r, const interval& t_range, hit_record& rec) const override {
        hit_record temp;
        bool hit_anything = false;
        double closest = t_range.max();

        for (const auto& object : objects) {
            if (object->hit(r, interval(t_range.min(), closest), temp)) {
                hit_anything = true;
                closest = temp.t;
                rec = temp;
            }
        }
        
        return hit_anything;
    }

    virtual bool bounding_box(aabb& output_box) const override {
        if (objects.empty()) return false;
        aabb temp_box;
        bool first = true;
        for (auto& obj : objects) {
            if (!obj->bounding_box(temp_box))
                return false;
            output_box = first ? temp_box : surrounding_box(output_box, temp_box);
            first = false;
        }
        return true;
    }
};

#endif