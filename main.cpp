#include "camera.h"
#include "geometry.h"
#include "translate.h"
int main() {
    // *设置图片为：
    // *image_width=400
    // *aspect_ratio=16.0/9.0
    // *fov=90
    int image_width = 400;
    double aspect_ratio = 1.0;
    double fov = 40;
    Camera cam(image_width, aspect_ratio, fov);

    // *启用抗锯齿
    // cam.set_if_antialising(true);

    // *设置纹理

    // *设置背景颜色
    cam.set_background_color(Color(0.0, 0.0, 0.0));

    // *设置表面
    auto red = make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = make_shared<Lambertian>(Color(.12, .45, .15));
    auto light = make_shared<Diffuse_light>(Color(15, 15, 15));

    // *设置模型
    cam.add_model(make_shared<Quad>(Point3(555, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), green));
    cam.add_model(make_shared<Quad>(Point3(0, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), red));
    cam.add_model(make_shared<Quad>(Point3(343, 554, 332), Vec3(-130, 0, 0), Vec3(0, 0, -105), light));
    cam.add_model(make_shared<Quad>(Point3(0, 0, 0), Vec3(555, 0, 0), Vec3(0, 0, 555), white));
    cam.add_model(make_shared<Quad>(Point3(555, 555, 555), Vec3(-555, 0, 0), Vec3(0, 0, -555), white));
    cam.add_model(make_shared<Quad>(Point3(0, 0, 555), Vec3(555, 0, 0), Vec3(0, 555, 0), white));

    shared_ptr<Hittable> box1 = produce_box(Point3(0, 0, 0), Point3(165, 330, 165), white);
    box1 = make_shared<Rotate_y>(box1, 15);
    box1 = make_shared<Translate>(box1, Vec3(265, 0, 295));
    cam.add_model(box1);

    shared_ptr<Hittable> box2 = produce_box(Point3(0, 0, 0), Point3(165, 165, 165), white);
    box2 = make_shared<Rotate_y>(box2, -18);
    box2 = make_shared<Translate>(box2, Vec3(130, 0, 65));
    cam.add_model(box2);

    // *设置相机视角
    cam.set_look_from(Point3(278, 278, -800));
    cam.set_look_to(Point3(278, 278, 0));
    cam.set_vup(Point3(0, 1, 0));

    // *设置光线采样数目
    cam.set_samples_per_pixel(1000);

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