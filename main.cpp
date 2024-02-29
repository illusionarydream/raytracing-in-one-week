#include "camera.h"
int main() {
    // *设置图片为：
    // *image_width=400
    // *aspect_ratio=16.0/9.0
    // *fov=90
    int image_width = 800;
    double aspect_ratio = 16.0 / 9.0;
    double fov = 20;
    camera cam(image_width, aspect_ratio, fov);

    // *启用抗锯齿
    // cam.set_if_antialising(true);
    auto earth_texture = make_shared<image_texture>("/home/illusionary/文档/c++_code/raytracing in one week/earthmap.jpg");
    auto earth_surface = make_shared<Lambertian>(color(1.0, 1.0, 1.0), earth_texture);
    auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

    // *设置模型
    cam.add_model(globe);

    // *设置相机视角
    cam.set_look_from(point3(0, 0, 12));
    cam.set_look_to(point3(0, 0, 0));
    cam.set_vup(point3(0, 1, 0));

    // *设置相机快门速度
    cam.set_shutter_time(0);

    // *使用BVH优化
    cam.set_if_BVH_optimization(true);

    // *使用四个线程
    cam.set_thread_num(6);

    // *初始化
    cam.Initialize();

    // *渲染
    cam.render();
}