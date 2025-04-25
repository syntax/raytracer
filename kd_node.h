#ifndef KD_NODE_H
#define KD_NODE_H

#include "hittable.h"
#include "aabb.h"
#include <vector>
#include <memory>
#include <algorithm>

// a simple median-split k-d tree node for accelerating ray-scene intersection
// trying to implement this as opposed to doing every single cacl on a O(n)
// this should prune 
class kd_node : public hittable {
public:
    // build a k-d tree over objects.  splits along axis = depth%3.
    // stos and makes a leaf when object count <= max_leaf_size.
    kd_node(const std::vector<std::shared_ptr<hittable>>& objects,
            int depth = 0,
            int max_leaf_size = 4) // change this to 8 maybe idk
    {
        // cmp bounding box of all objects
        aabb total_box;
        bool first_box = true;
        for (auto& obj : objects) {
            aabb obj_box;
            if (!obj->bounding_box(obj_box)) continue;
            total_box = first_box ? obj_box : surrounding_box(total_box, obj_box);
            first_box = false;
        }
        bbox = total_box;

        // if few objects, make this a leaf
        if ((int)objects.size() <= max_leaf_size) {
            leaf_objects = objects;
            return;
        }

        // otherwise split along the selected axis
        int axis = depth % 3;
        auto sorted = objects;  // copy for sorting
        std::sort(sorted.begin(), sorted.end(),
            [axis](auto& a, auto& b) {
                aabb box_a, box_b;
                a->bounding_box(box_a);
                b->bounding_box(box_b);
                double ca = 0.5 * (box_a.min()[axis] + box_a.max()[axis]);
                double cb = 0.5 * (box_b.min()[axis] + box_b.max()[axis]);
                return ca < cb;
            }
        );

        auto mid = sorted.begin() + sorted.size() / 2;
        std::vector<std::shared_ptr<hittable>> left_objs(sorted.begin(), mid);
        std::vector<std::shared_ptr<hittable>> right_objs(mid, sorted.end());

        left  = std::make_shared<kd_node>(left_objs,  depth + 1, max_leaf_size);
        right = std::make_shared<kd_node>(right_objs, depth + 1, max_leaf_size);
    }

    // ray intersection with the nodes bounding box, then recurse or test leaf
    virtual bool hit(const ray& r, const interval& t_range, hit_record& rec) const override {
        // cull whole node
        if (!bbox.hit(r, t_range))
            return false;
    
        // leaf: test all objects, still a big one
        if (!left && !right) {
            bool hit_any = false;
            double closest = t_range.max();
            hit_record temp;
            for (auto& obj : leaf_objects) {
                if (obj->hit(r, interval{t_range.min(), closest}, temp)) {
                    hit_any = true;
                    closest = temp.t;
                    rec = temp;
                }
            }
            return hit_any;
        }
    
        // 3) internal node: test both children but choose nearest
        hit_record left_rec, right_rec;
        bool hit_left  = left  && left->hit(r, t_range, left_rec);
        bool hit_right = right && right->hit(r, t_range, right_rec);
    
        if (hit_left && hit_right) {
            // both hit: pick the closer one
            if (left_rec.t < right_rec.t)
                rec = left_rec;
            else
                rec = right_rec;
            return true;
        } else if (hit_left) {
            rec = left_rec;
            return true;
        } else if (hit_right) {
            rec = right_rec;
            return true;
        } else {
            return false;
        }
    }

    // ret this nodes bounding box
    virtual bool bounding_box(aabb& output_box) const override {
        output_box = bbox;
        return true;
    }

private:
    aabb bbox;
    std::shared_ptr<kd_node> left;
    std::shared_ptr<kd_node> right;
    std::vector<std::shared_ptr<hittable>> leaf_objects;
};

#endif
