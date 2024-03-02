#include "camera.h"
int main() {
    // *设置图片为：
    // *image_width=400
    // *aspect_ratio=16.0/9.0
    // *fov=90
    int image_width = 400;
    double aspect_ratio = 16.0 / 9.0;
    double fov = 20;
    Camera cam(image_width, aspect_ratio, fov);

    // *启用抗锯齿
    // cam.set_if_antialising(true);

    // *设置纹理
    auto pertext = make_shared<Noise_texture>(4);

    // *设置模型
    cam.add_model(make_shared<Sphere>(point3(0, -1000, 0), 1000, make_shared<Lambertian>(color(1.0, 1.0, 1.0), pertext)));
    cam.add_model(make_shared<Sphere>(point3(0, 2, 0), 2, make_shared<Lambertian>(color(1.0, 1.0, 1.0), pertext)));

    // *设置相机视角
    cam.set_look_from(point3(13, 2, 3));
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