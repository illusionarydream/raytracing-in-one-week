#include "camera.h"
int main() {
    // *设置图片为：
    // *image_width=800
    // *focal_lenth=1.0
    // *aspect_ratio=16.0/9.0
    int image_width = 400;
    double focal_lenth = 1.0;
    double aspect_ratio = 16.0 / 9.0;
    camera cam(image_width, aspect_ratio, focal_lenth);

    // *添加模型
    cam.add_model(make_shared<sphere>(point3(0, 0, -1), 0.5));
    cam.add_model(make_shared<sphere>(point3(0, -100, 0), 99.5));

    // *启用抗锯齿
    // cam.set_if_antialising(true);

    // *渲染
    cam.render();
}