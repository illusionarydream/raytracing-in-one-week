#include "camera.h"
int main() {
    // *设置图片为：
    // *image_width=400
    // *aspect_ratio=16.0/9.0
    // *fov=90
    int image_width = 200;
    double aspect_ratio = 16.0 / 9.0;
    double fov = 80;
    Camera cam(image_width, aspect_ratio, fov);

    // *启用抗锯齿
    // cam.set_if_antialising(true);

    // *设置纹理
    auto pertext = make_shared<Noise_texture>(4);

    // *设置背景颜色
    cam.set_background_color(color(0.7, 0.8, 1.00));

    // *设置表面
    auto left_red = make_shared<Lambertian>(color(1.0, 0.2, 0.2));
    auto back_green = make_shared<Lambertian>(color(0.2, 1.0, 0.2));
    auto right_blue = make_shared<Lambertian>(color(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<Lambertian>(color(1.0, 0.5, 0.0));
    auto lower_teal = make_shared<Lambertian>(color(0.2, 0.8, 0.8));

    // *设置模型
    cam.add_model(make_shared<Quad>(point3(-3, -2, 5), Vec3(0, 0, -4), Vec3(0, 4, 0), left_red));
    cam.add_model(make_shared<Quad>(point3(-2, -2, 0), Vec3(4, 0, 0), Vec3(0, 4, 0), back_green));
    cam.add_model(make_shared<Quad>(point3(3, -2, 1), Vec3(0, 0, 4), Vec3(0, 4, 0), right_blue));
    cam.add_model(make_shared<Quad>(point3(-2, 3, 1), Vec3(4, 0, 0), Vec3(0, 0, 4), upper_orange));
    cam.add_model(make_shared<Quad>(point3(-2, -3, 5), Vec3(4, 0, 0), Vec3(0, 0, -4), lower_teal));

    // *设置相机视角
    cam.set_look_from(point3(0, 0, 9));
    cam.set_look_to(point3(0, 0, 0));
    cam.set_vup(point3(0, 1, 0));

    // *设置相机快门速度
    cam.set_shutter_time(0);

    // *使用BVH优化
    cam.set_if_BVH_optimization(true);

    // *输出深度图
    // cam.set_if_depth_output(true);

    // *使用四个线程
    cam.set_thread_num(6);

    // *初始化
    cam.Initialize();

    // *渲染
    cam.render();
}