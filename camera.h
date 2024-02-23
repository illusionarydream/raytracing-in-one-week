#ifndef CAMERA_H
#define CAMERA_H
#include <iostream>
#include "color.h"
#include "ray.h"
#include "sphere.h"
#include "hittable_list.h"
#include "math_materials.h"
// *相机类型用于照相，所有的整体渲染过程都在这个类型里完成，就像是相机拍摄得到一张照片
class camera {
   private:
    // *变量
    // set up the rendered image;
    int image_width;
    double aspect_ratio;
    int image_height;

    // 相机位置
    point3 camera_center;

    // 焦距
    double focal_length;

    // 视口信息
    double viewport_height;
    double viewport_width;

    // 模型信息
    hittable_list models;

    // 视口坐标系中xy轴模长一像素的向量
    vec3 viewport_delta_u;
    vec3 viewport_delta_v;

    // 视口坐标系原点在世界坐标系中的位置，（0,0）像素中心在世界坐标系中的位置
    // 注意：需要加上相机位置向量
    vec3 viewport_origin_in_world = camera_center;
    vec3 pixel_origin_in_world;

    // 是否进行3*3的高斯模糊去锯齿
    bool if_antialiasing;
    double gaussion_kernel[3][3] = {
        {1.0 / 16, 2.0 / 16, 1.0 / 16},
        {2.0 / 16, 4.0 / 16, 2.0 / 16},
        {1.0 / 16, 2.0 / 16, 1.0 / 16}};

    // 图片RGB缓存
    color** color_buf;

    // 对一个像素点采样的光线数量
    int samples_per_pixel;

   private:
    // *函数
    void initialize() {
        // 设置图像
        image_height = image_width / aspect_ratio;

        // 设置相机在世界坐标系中的位置
        camera_center = point3(0, 0, 0);

        // 设置视口信息
        viewport_height = 2.0;
        viewport_width = viewport_height * (double(image_width) / image_height);

        // 设置视口坐标系一像素的xy基向量
        viewport_delta_u = vec3(viewport_width, 0, 0) / image_width;
        viewport_delta_v = vec3(0, -viewport_height, 0) / image_height;

        // 设置视口坐标系在世界坐标系中的位置和像素原点
        viewport_origin_in_world = camera_center + point3(-viewport_width / 2, viewport_height / 2, -focal_length);
        pixel_origin_in_world = viewport_origin_in_world + viewport_delta_u / 2 + viewport_delta_v / 2;

        // 默认antialiasing是false
        if_antialiasing = false;

        // 初始化color_buf
        color_buf = new color*[image_height];
        for (int i = 0; i < image_height; i++)
            color_buf[i] = new color[image_width];

        // 初始化像素采样光线数量
        samples_per_pixel = 10;
    }
    color raycolor(const ray& r, const hittable& models) {
        hit_record rec;
        bool if_hit = models.hit(r, interval(0.0, infinity), rec);
        return if_hit == false ? color(1.0, 1.0, 1.0) : rec.normal / 2 + color(0.5, 0.5, 0.5);
        // auto depth=(r.at(rec.t)-r.origin()).length();s
        // return if_hit==true?color(0.0,0.0,0.0):color(255.0,255.0,255.0);
    }
    void antialiasing() {
        if (if_antialiasing == true) {
            std::clog << "start antiliasing !\n";
            for (int i = 1; i < image_height - 1; i++)
                for (int j = 1; j < image_width - 1; j++) {
                    color res_color(0.0, 0.0, 0.0);
                    for (int delta_i = -1; delta_i < 2; delta_i++)
                        for (int delta_j = -1; delta_j < 2; delta_j++)
                            res_color += color_buf[i + delta_i][j + delta_j] * gaussion_kernel[1 + delta_i][1 + delta_j];
                    color_buf[i][j] = res_color;
                }
            std::clog << "antiliasing finished!\n";
        }
        return;
    }
    void print_iamge() {
        std::cout << "P3\n"
                  << image_width << ' ' << image_height << "\n255\n";
        std::clog << "P3\n"
                  << image_width << ' ' << image_height << "\n255\n";
        antialiasing();
        for (int i = 0; i < image_height; i++)
            for (int j = 0; j < image_width; j++)
                write_color(std::cout, color_buf[i][j]);
        std::clog << "print finished!\n";
    }
    vec3 get_squard_bias() {
        auto random_x = random_double() - 0.5;
        auto random_y = random_double() - 0.5;
        return random_x * viewport_delta_u + random_y * viewport_delta_v;
    }
    // 对一个像素区域内的各个点进行采样求平均
    ray get_ray(int i, int j) {
        auto bias = get_squard_bias();
        auto pixel_center = pixel_origin_in_world + i * viewport_delta_v + j * viewport_delta_u + bias;
        auto ray_dir = pixel_center - camera_center;
        return ray(camera_center, ray_dir);
    }

   public:
    camera() {}
    camera(double image_w, double aspect_r, double focal_l) : image_width(image_w),
                                                              aspect_ratio(aspect_r),
                                                              focal_length(focal_l) {
        initialize();
    }
    void add_model(shared_ptr<hittable> model) {
        models.add(model);
    }
    void clear_model() {
        models.clear();
    }
    void set_if_antialising(bool sign) {
        if_antialiasing = sign;
    }
    void render() {
        for (int i = 0; i < image_height; i++) {
            std::clog << "\rScanlines remaining:" << image_height - i << std::endl;
            for (int j = 0; j < image_width; j++) {
                // produce a ray
                color ans_color(0.0, 0.0, 0.0);
                for (int k = 0; k < samples_per_pixel; k++) {
                    auto current_ray = get_ray(i, j);
                    // auto current_color = raycolor(current_ray, models);
                    // std::clog << current_color.x() << ' ' << current_color.y() << ' ' << current_color.z() << std::endl;
                    ans_color += raycolor(current_ray, models);
                }
                ans_color /= samples_per_pixel;
                color_buf[i][j] = ans_color;
            }
        }
        std::clog << "render finished!\n";
        print_iamge();
    }
};

#endif