#ifndef SPHERE_H
#define SPHERE_H
#include "vec3.h"
#include "ray.h"
#include "hittable.h"
class sphere: public hittable{
public:
    sphere() {}
    sphere(const point3 &cen,double r):center(cen),radius(r) {}
    bool if_hit_sphere(const ray &r){
        auto D=r.direction();
        auto O=r.origin();
        auto C=center;
        auto R=radius;
        auto a=dot(D,D);
        auto half_b=dot(D,O-C);
        auto c=dot(O-C,O-C)-R*R;
        auto discriminant=half_b*half_b-a*c;

        return (discriminant>=0);
    }// 1--hit the sphere, 0--non-hit the sphere
    double hit_sphere(const ray &r){
        auto D=r.direction();
        auto O=r.origin();
        auto C=center;
        auto R=radius;
        auto a=dot(D,D);
        auto half_b=dot(D,O-C);
        auto c=dot(O-C,O-C)-R*R;
        auto discriminant=half_b*half_b-a*c;

        if(discriminant>=0)
            return (-half_b-sqrt(discriminant))/a;
        else
            return -1.0;
    }
    bool hit(const ray& r,double ray_tmin,double ray_tmax,hit_record &rec) const override{
        auto D=r.direction();
        auto O=r.origin();
        auto C=center;
        auto R=radius;
        auto a=dot(D,D);
        auto half_b=dot(D,O-C);
        auto c=dot(O-C,O-C)-R*R;
        auto discriminant=half_b*half_b-a*c;
        
        if(discriminant<0)
            return false;
        auto sqrd=sqrt(discriminant);
        auto root=(-half_b-sqrd)/a;
        if(root<=ray_tmin||root>=ray_tmax){
            root=(-half_b+sqrd)/a;
            if(root<=ray_tmin||root>=ray_tmax)
                return false;
        }
        
        rec.p=r.at(root);
        rec.t=root;
        vec3 outward_normal=(rec.p-center)/radius;
        rec.set_surface_normal(r,outward_normal);
        
        return true;
    }
    vec3 get_center(){
        return center;
    }
    double get_radius(){
        return radius;
    }
private:
    point3 center;
    double radius;
};
#endif