#include "camera.h"
int main() {
    // *设置图片为：
    // *image_width=400
    // *aspect_ratio=16.0/9.0
    // *fov=90
    // *focus_angle =10.0
    // *focus_dist=3.4;
    int image_width = 1200;
    double aspect_ratio = 16.0 / 9.0;
    double fov = 20;
    double defocus_angle = 0.6;
    double defocus_dist = 10.0;
    camera cam(image_width, aspect_ratio, fov, defocus_dist, defocus_angle);
    auto ground_material = make_shared<Lambertian>(color(0.5, 0.5, 0.5));
    cam.add_model(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = random_vector() * random_vector();
                    sphere_material = make_shared<Lambertian>(albedo);
                    cam.add_model(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = random_vector(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    cam.add_model(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(color(1.0, 1.0, 1.0), 1.5);
                    cam.add_model(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(color(1.0, 1.0, 1.0), 1.5);
    cam.add_model(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<Lambertian>(color(0.4, 0.2, 0.1));
    cam.add_model(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    cam.add_model(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    // *启用抗锯齿
    // cam.set_if_antialising(true);

    // *设置相机视角
    cam.set_look_from(point3(13, 2, 3));
    cam.set_look_to(point3(0, 0, 0));
    cam.set_vup(point3(0, 1, 0));

    // *初始化
    cam.Initialize();

    // *渲染
    cam.render();
}