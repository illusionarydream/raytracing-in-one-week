#include <iostream>
#include "color.h"
#include "ray.h"
color raycolor(const ray& r){
    auto l=r.direction().length();
    return color(l-0.999,l-0.999,l-0.999);
}
int main(){
    // set up the rendered image;
    int image_width = 800;
    int aspect_ratio = 16.0/9.0;
    int image_height = image_width/aspect_ratio;

    // 视口
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height*(double(image_width)/image_height);
    
    // 总信息
    auto camera_center=point3(0,0,0);
    auto focal_length=1.0;

    // 视口坐标系中xy轴模长一像素的向量
    auto viewport_delta_u=vec3(viewport_width,0,0)/image_width;
    auto viewport_delta_v=vec3(0,-viewport_height,0)/image_height;

    // 视口坐标系原点在世界坐标系中的位置，（0,0）像素中心在世界坐标系中的位置
    // 注意：需要加上相机位置向量
    auto viewport_origin_in_world=camera_center+point3(-viewport_width/2,viewport_height/2,-focal_length);
    auto pixel_origin_in_world=viewport_origin_in_world+viewport_delta_u/2+viewport_delta_v/2;

    std::cout<<"P3\n"<<image_width<<' '<<image_height<<"\n255\n";
    
    for(int i=0;i<image_height;i++){
        std::clog<<"\rScanlines remaining:"<<image_height-i<<std::endl;
        for(int j=0;j<image_width;j++){
            // produce a ray
            auto current_pixel_center=pixel_origin_in_world+i*viewport_delta_v+j*viewport_delta_u;
            auto current_ray_dir=current_pixel_center-camera_center;
            ray current_ray=ray(current_pixel_center,current_ray_dir);

            auto current_pixel_color=raycolor(current_ray);
            write_color(std::cout, current_pixel_color);
        }
    }
    std::clog<<"finished!\n";
}