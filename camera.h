#ifndef CAMERA_H
#define CAMERA_H
#include <iostream>
#include "color.h"
#include "ray.h"
#include "sphere.h"
#include "hittable_list.h"
#include "math_materials.h"
#include "material.h"
#include "BVH.h"
#include "thread"
#include "quad.h"
// *相机类型用于照相，所有的整体渲染过程都在这个类型里完成，就像是相机拍摄得到一张照片
class Camera {
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
    Hittable_list models;

    // 背景颜色
    color background_color;

    // 视口坐标系基向量
    Vec3 viewport_u;
    Vec3 viewport_v;

    // 视口坐标系中xy轴模长一像素的向量
    Vec3 viewport_delta_u;
    Vec3 viewport_delta_v;

    // 视口坐标系原点在世界坐标系中的位置，（0,0）像素中心在世界坐标系中的位置
    // 注意：需要加上相机位置向量
    Vec3 viewport_origin_in_world = camera_center;
    Vec3 pixel_origin_in_world;

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
    Vec3 defocus_disk_u;
    Vec3 defocus_disk_v;

    // 是否使用BVH树优化
    bool if_BVH_optimization;

    // 多线程数量
    int thread_num;

    // 是否输出深度图
    bool if_depth_output;

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

        // 初始化color_buf
        color_buf = new color*[image_height];
        for (int i = 0; i < image_height; i++)
            color_buf[i] = new color[image_width];

        // 初始化BVH tree
        if (if_BVH_optimization == true)
            models = Hittable_list(make_shared<BVH_node>(models));
    }

    double depth_color(const Hit_record& rec) {
        return (rec.p - camera_center).length();
    }

    // *render the real image
    color render_true_color(const Ray& r, const Hittable& models) {
        // 设置交点
        Hit_record rec;
        // 是否击打到相关物体表面
        bool if_hit = models.hit(r, Interval(min_double_error, infinity), rec);
        // 如果没有击打到物体的表面的话，就返回背景颜色
        if (!if_hit)
            return background_color;
        // 如果击打到物体的表面的话，就随机确定下一次是否继续进行光的弹射
        if (!if_next_bounce(next_bounce_ratio))
            return color(0.0, 0.0, 0.0);
        // 得到自发光的颜色
        color emission_color = rec.mat->emitted(rec.u, rec.v, rec.p);
        // 设置下一次发射的光线
        Ray scattered_ray;
        color attenuation;
        bool if_scattered = rec.mat->scatter(r, rec, attenuation, scattered_ray);
        // 如果没有发生继续的散射，则直接返回光源颜色
        if (!if_scattered)
            return emission_color;
        // 如果发生继续的散射，则返回散射颜色和自发光颜色
        color scatter_color = attenuation * render_true_color(scattered_ray, models) / next_bounce_ratio;
        return scatter_color + emission_color;
    }

    // *render the depth image
    color render_depth_color(const Ray& r, const Hittable& models) {
        Hit_record rec;
        bool if_hit = models.hit(r, Interval(min_double_error, infinity), rec);
        if (if_hit)
            return color(1.0, 1.0, 1.0);
        auto distance = (rec.p - r.origin()).length();
        auto depth = (distance - 4) / 10;
        return color(depth, depth, depth);
    }

    // *render a single light
    color raycolor(const Ray& r, const Hittable& models) {
        color ans;
        if (if_depth_output == true)
            ans = render_depth_color(r, models);
        else
            ans = render_true_color(r, models);
        return ans;
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
    Vec3 get_squard_bias() {
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
    Ray get_ray(int i, int j) {
        auto bias = get_squard_bias();
        auto pixel_center = pixel_origin_in_world + i * viewport_delta_v + j * viewport_delta_u + bias;
        auto ray_orig = (defocus_angle <= min_double_error) ? camera_center : defocus_disk_sample();
        auto ray_dir = pixel_center - ray_orig;
        auto ray_time = random_double(0.0, shutter_time);
        return Ray(ray_orig, ray_dir, ray_time);
    }

   public:
    Camera() {}
    Camera(double image_w, double aspect_r, double _fov)
        : image_width(image_w),
          aspect_ratio(aspect_r),
          fov(_fov) {
        // *对一些非关键的量进行初值的设定，将其变量的变化放在后面set部分来设置
        // 设置相机信息
        look_from = point3(0, 0, 0);
        look_to = point3(0, 0, -1);
        vup = Vec3(0, 1, 0);

        // 初始化像素采样光线数量
        samples_per_pixel = 200;

        // 初始化下一次弹射的概率
        // *这是无偏的估计量
        next_bounce_ratio = 0.95;

        // 初始化最大的弹射次数
        // *这是有偏的估计量
        max_bounce_times = 10;

        // 初始化焦距
        focal_dist = 1.0;

        // 初始化光圈大小
        defocus_angle = 0.0;

        // 初始化快门速度
        shutter_time = 0.0;

        // 是否使用BVH树优化，默认不使用
        if_BVH_optimization = false;

        // 初始化进程数量
        thread_num = 1;

        // 初始化输出不是深度图
        if_depth_output = false;

        // 初始化不启用抗锯齿
        if_antialiasing = false;

        // 设置背景颜色为（1.0,1.0,1.0）
        background_color = color(1.0, 1.0, 1.0);
    }
    void add_model(shared_ptr<Hittable> model) {
        models.add(model);
    }
    void clear_model() {
        models.clear();
    }
    void set_if_antialising(bool sign) {
        if_antialiasing = sign;
    }
    void set_look_from(const Vec3& v) {
        look_from = v;
    }
    void set_look_to(const Vec3& v) {
        look_to = v;
    }
    void set_vup(const Vec3& v) {
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
    void set_if_BVH_optimization(bool if_BVH) {
        if_BVH_optimization = if_BVH;
    }
    void set_thread_num(int _num) {
        thread_num = _num;
    }
    void set_if_depth_output(bool if_depth) {
        if_depth_output = if_depth;
    }
    void set_background_color(const color& _bc) {
        background_color = _bc;
    }
    void Initialize() {
        initialize();
    }
    // *render
    void render() {
        std::thread thread_list[thread_num];
        int thread_span = image_height / thread_num;
        auto renderRow = [&](int k) {
            int start_height;
            int end_height;
            if (k == thread_num - 1) {
                start_height = k * thread_span;
                end_height = image_height;
            } else {
                start_height = k * thread_span;
                end_height = (k + 1) * thread_span;
            }
            for (int i = start_height; i < end_height; i++) {
                std::clog << "thread " << k << " : Row " << i << " finished\n";
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
        };
        for (int i = 0; i < thread_num; i++)
            thread_list[i] = std::thread(renderRow, i);
        for (int i = 0; i < thread_num; i++)
            thread_list[i].join();
        std::clog << "render finished!\n";
        print_iamge();
    }
};

#endif