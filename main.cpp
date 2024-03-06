#include "camera.h"
#include "geometry.h"
#include "translate.h"
#include "constant_media.h"
#include "BVH.h"
int main() {
    // *设置图片为：
    // *image_width=400
    // *aspect_ratio=16.0/9.0
    // *fov=90
    int image_width = 200;
    double aspect_ratio = 1.0;
    double fov = 40;
    Camera cam(image_width, aspect_ratio, fov);

    // *启用抗锯齿
    // cam.set_if_antialising(true);

    // *设置背景
    Hittable_list boxes1;
    auto ground = make_shared<Lambertian>(Color(0.48, 0.83, 0.53));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1, 101);
            auto z1 = z0 + w;

            boxes1.add(produce_box(Point3(x0, y0, z0), Point3(x1, y1, z1), ground));
        }
    }

    // *设置模型
    Hittable_list world;

    world.add(make_shared<BVH_node>(boxes1));

    auto light = make_shared<Diffuse_light>(Color(7, 7, 7));
    world.add(make_shared<Quad>(Point3(123, 554, 147), Vec3(300, 0, 0), Vec3(0, 0, 265), light));

    auto center1 = Point3(400, 400, 200);
    auto center2 = center1 + Vec3(30, 0, 0);
    auto Sphere_material = make_shared<Lambertian>(Color(0.7, 0.3, 0.1));
    world.add(make_shared<Sphere>(center1, 50, center2 - center1, Sphere_material));

    world.add(make_shared<Sphere>(Point3(260, 150, 45), 50, make_shared<Dielectric>(1.5)));
    world.add(make_shared<Sphere>(
        Point3(0, 150, 145), 50, make_shared<Metal>(Color(0.8, 0.8, 0.9), 1.0)));

    auto boundary = make_shared<Sphere>(Point3(360, 150, 145), 70, make_shared<Dielectric>(1.5));
    world.add(boundary);
    world.add(make_shared<Constant_media>(boundary, 0.2, Color(0.2, 0.4, 0.9)));
    boundary = make_shared<Sphere>(Point3(0, 0, 0), 5000, make_shared<Dielectric>(1.5));
    world.add(make_shared<Constant_media>(boundary, .0001, Color(1, 1, 1)));

    auto emat = make_shared<Lambertian>(make_shared<Image_texture>("/home/illusionary/文档/c++_code/raytracing in one week/texture_image/earthmap.jpg"));
    world.add(make_shared<Sphere>(Point3(400, 200, 400), 100, emat));
    auto pertext = make_shared<Noise_texture>(0.1);
    world.add(make_shared<Sphere>(Point3(220, 280, 300), 80, make_shared<Lambertian>(pertext)));

    Hittable_list boxes2;
    auto white = make_shared<Lambertian>(Color(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<Sphere>(random_vector(0, 165), 10, white));
    }

    world.add(make_shared<Translate>(
        make_shared<Rotate_y>(
            make_shared<BVH_node>(boxes2), 15),
        Vec3(-100, 270, 395)));

    cam.add_model(make_shared<Hittable_list>(world));

    // *设置相机视角
    cam.set_look_from(Point3(478, 278, -600));
    cam.set_look_to(Point3(278, 278, 0));
    cam.set_vup(Point3(0, 1, 0));

    // *设置光线采样数目
    cam.set_samples_per_pixel(100);

    // *设置相机快门速度
    cam.set_shutter_time(0.1);

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