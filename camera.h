#ifndef CAMERA_H
#define CAMERA_H
#include <iostream>
#include "color.h"
#include "ray.h"
#include "sphere.h"
#include "hittable_list.h"
#include "math_materials.h"
#include "material.h"
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

    // 相机朝向
    point3 look_from;
    point3 look_to;
    point3 vup;

    // 相机坐标系
    point3 u;
    point3 v;
    point3 w;

    // 焦距
    double focal_dist;

    // 垂直视角
    double fov;

    // 视口信息
    double viewport_height;
    double viewport_width;

    // 模型信息
    hittable_list models;

    // 视口坐标系基向量
    vec3 viewport_u;
    vec3 viewport_v;

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

    // 继续进行下一次bounce的概率
    double next_bounce_ratio;

    // 设置最多弹射的次数
    int max_bounce_times;

    // 实现景深效果
    double defocus_angle;

    // 设置快门速度
    double shutter_time;

    // 镜片光圈在世界坐标系中的基坐标
    vec3 defocus_disk_u;
    vec3 defocus_disk_v;

   private:
    // *函数
    void initialize() {
        // 设置图像
        image_height = image_width / aspect_ratio;

        // 设置相机在世界坐标系中的位置
        // 为相机从什么地方看，相机看到什么的地方就是视口平面
        camera_center = look_from;

        // 计算相机坐标系在世界坐标系中的三个基轴
        // *vup 不一定垂直于w
        w = unit_vector(look_from - look_to);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // 设置视口信息
        auto tan_half_theta = tan(degree_to_radians(fov / 2));
        viewport_height = 2 * tan_half_theta * focal_dist;
        viewport_width = viewport_height * (double(image_width) / image_height);

        // 设置视口坐标系基向量
        viewport_u = viewport_width * u;
        viewport_v = -viewport_height * v;

        // 设置视口坐标系一像素的xy基向量
        viewport_delta_u = viewport_u / image_width;
        viewport_delta_v = viewport_v / image_height;

        // 设置视口坐标系在世界坐标系中的位置和像素原点
        viewport_origin_in_world = camera_center - viewport_u / 2 - viewport_v / 2 - (focal_dist * w);
        pixel_origin_in_world = viewport_origin_in_world + viewport_delta_u / 2 + viewport_delta_v / 2;

        // 计算光圈基向量
        auto half_defocus_angle = degree_to_radians(defocus_angle / 2);
        auto defocus_radius = tan(half_defocus_angle) * focal_dist;
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;

        // 默认antialiasing是false
        if_antialiasing = false;

        // 初始化color_buf
        color_buf = new color*[image_height];
        for (int i = 0; i < image_height; i++)
            color_buf[i] = new color[image_width];
    }

    double depth_color(const hit_record& rec) {
        return (rec.p - camera_center).length();
    }

    // *render a single light
    color raycolor(const ray& r, const hittable& models) {
        hit_record rec;
        bool if_hit = models.hit(r, interval(min_double_error, infinity), rec);

        if (if_hit) {
            // *to generate true Lambertian reflectance
            ray scattered_ray;
            color attenuation;
            rec.mat->scatter(r, rec, attenuation, scattered_ray);
            if (if_next_bounce(next_bounce_ratio))
                return attenuation * raycolor(scattered_ray, models) / next_bounce_ratio;
            return color(0.0, 0.0, 0.0);
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
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

    point3 defocus_disk_sample() const {
        auto p = random_in_unit_disk();
        return camera_center + p[0] * defocus_disk_u + p[1] * defocus_disk_v;
    }

    // * get a single light
    // 对一个像素区域内的各个点进行采样求平均
    // 若光圈大小不为0,则对其进行光圈模糊处理
    // 若快门速度不为0,则对其进行快门模糊处理
    ray get_ray(int i, int j) {
        auto bias = get_squard_bias();
        auto pixel_center = pixel_origin_in_world + i * viewport_delta_v + j * viewport_delta_u + bias;
        auto ray_orig = (defocus_angle <= min_double_error) ? camera_center : defocus_disk_sample();
        auto ray_dir = pixel_center - ray_orig;
        auto ray_time = random_double(0.0, shutter_time);
        return ray(ray_orig, ray_dir, ray_time);
    }

   public:
    camera() {}
    camera(double image_w, double aspect_r, double _fov)
        : image_width(image_w),
          aspect_ratio(aspect_r),
          fov(_fov) {
        // *对一些非关键的量进行初值的设定，将其变量的变化放在后面set部分来设置
        // 设置相机信息
        look_from = point3(0, 0, 0);
        look_to = point3(0, 0, -1);
        vup = vec3(0, 1, 0);

        // 初始化像素采样光线数量
        samples_per_pixel = 1;

        // 初始化下一次弹射的概率
        // *这是无偏的估计量
        next_bounce_ratio = 0.5;

        // 初始化最大的弹射次数
        // *这是有偏的估计量
        max_bounce_times = 10;

        // 初始化焦距
        focal_dist = 1.0;

        // 初始化光圈大小
        defocus_angle = 0.0;

        // 初始化快门速度
        shutter_time = 0.0;
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
    void set_look_from(const vec3& v) {
        look_from = v;
    }
    void set_look_to(const vec3& v) {
        look_to = v;
    }
    void set_vup(const vec3& v) {
        vup = v;
    }
    void set_samples_per_pixel(int num) {
        samples_per_pixel = num;
    }
    void set_next_bounce_ratio(double ratio) {
        next_bounce_ratio = ratio < 1.0 ? ratio : 0.99;
    }
    void set_max_bounce_times(int num) {
        max_bounce_times = num;
    }
    void set_focal_dist(double focal_d) {
        focal_dist = focal_d;
    }
    void set_defocus_angle(double defocus_a) {
        defocus_angle = defocus_a;
    }
    void set_shutter_time(double time) {
        shutter_time = time;
    }
    void Initialize() {
        initialize();
    }
    // *render
    void render() {
        for (int i = 0; i < image_height; i++) {
            std::clog << "\rScanlines remaining:" << image_height - i << std::endl;
            for (int j = 0; j < image_width; j++) {
                // produce a ray
                color ans_color(0.0, 0.0, 0.0);
                for (int k = 0; k < samples_per_pixel; k++) {
                    auto current_ray = get_ray(i, j);
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