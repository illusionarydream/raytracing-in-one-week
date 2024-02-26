#include "camera.h"
int main() {
    // *设置图片为：
    // *image_width=400
    // *focal_lenth=1.0
    // *aspect_ratio=16.0/9.0
    int image_width = 400;
    double focal_lenth = 1.0;
    double aspect_ratio = 16.0 / 9.0;
    camera cam(image_width, aspect_ratio, focal_lenth);

    // *新建材料
    auto material_ground = make_shared<Lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<Lambertian>(color(0.1, 0.2, 0.5));
    auto material_left = make_shared<dielectric>(color(1.0, 1.0, 1.0), 1.5);
    auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

    // *添加模型
    cam.add_model(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
    cam.add_model(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
    cam.add_model(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
    cam.add_model(make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.4, material_left));
    cam.add_model(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

    // *启用抗锯齿
    // cam.set_if_antialising(true);

    // *渲染
    cam.render();
}