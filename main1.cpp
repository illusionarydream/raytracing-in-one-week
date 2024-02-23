#include <iostream>
#include "color.h"
#include "ray.h"
#include "sphere.h"
#include "hittable_list.h"
#include "math_materials.h"
// 设置一个球模型

color raycolor(const ray& r,const hittable &models){
    hit_record rec;
    bool if_hit=models.hit(r,-0.01,infinity,rec);
    return if_hit==false?color(255.0,255.0,255.0):rec.normal/2+color(0.5,0.5,0.5);
    // auto depth=(r.at(rec.t)-r.origin()).length();
    // return if_hit==true?color(0.0,0.0,0.0):color(255.0,255.0,255.0);
}
int main(){
    // set up the rendered image;
    int image_width = 400;
    double aspect_ratio = 16.0/9.0;
    int image_height = image_width/aspect_ratio;

    // 视口
    auto camera_center=point3(0,0,0);
    auto focal_length=1.0;
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height*(double(image_width)/image_height);
    
    // 模型信息
    hittable_list models;
    models.add(make_shared<sphere>(point3(0,0,-1),0.5));
    models.add(make_shared<sphere>(point3(0,-100,0),99.5));

    // 视口坐标系中xy轴模长一像素的向量
    auto viewport_delta_u=vec3(viewport_width,0,0)/image_width;
    auto viewport_delta_v=vec3(0,-viewport_height,0)/image_height;

    // 视口坐标系原点在世界坐标系中的位置，（0,0）像素中心在世界坐标系中的位置
    // 注意：需要加上相机位置向量
    auto viewport_origin_in_world=camera_center+point3(-viewport_width/2,viewport_height/2,-focal_length);
    auto pixel_origin_in_world=viewport_origin_in_world+viewport_delta_u/2+viewport_delta_v/2;

    std::cout<<"P3\n"<<image_width<<' '<<image_height<<"\n255\n";
    std::clog<<"P3\n"<<image_width<<' '<<image_height<<"\n255\n";
    for(int i=0;i<image_height;i++){
        std::clog<<"\rScanlines remaining:"<<image_height-i<<std::endl;
        for(int j=0;j<image_width;j++){
            // produce a ray
            auto current_pixel_center=pixel_origin_in_world+i*viewport_delta_v+j*viewport_delta_u;
            auto current_ray_dir=current_pixel_center-camera_center;
            ray current_ray=ray(camera_center,current_ray_dir);

            auto current_pixel_color=raycolor(current_ray,models);
            write_color(std::cout, current_pixel_color);
        }
    }
    std::clog<<"finished!\n";
}