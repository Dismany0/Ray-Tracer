#ifndef TEXTURE_H
#define TEXTURE_H
#include "rtw_stg_image.h"
#include "rtweekend.h"

class texture {
  public:
    virtual ~texture() = default;

    virtual color value(double u, double v, const point3& p) const = 0;
};

class solid_color : public texture {
  public:
    solid_color(const color& albedo) : albedo(albedo) {}

    solid_color(double red, double green, double blue) : solid_color(color(red,green,blue)) {}

    color value(double u, double v, const point3& p) const override {
        return albedo;
    }

  private:
    color albedo;
};

class checker_texture : public texture {
  public:
  //we can use other texture as part of the checker texture
  checker_texture(double scale, shared_ptr<texture> even, shared_ptr<texture> odd)
  : inv_scale(1.0/scale), even(even), odd(odd) {}

  checker_texture(double scale, const color& c1, const color& c2)
  : inv_scale(1.0/scale),
  even(make_shared<solid_color>(c1)),
  odd(make_shared<solid_color>(c2)) {}

  color value(double u, double v, const point3& p) const override {
    //Divide the coords and take the left side floor. Use either even or odd texture
    auto xInt = int(std::floor(inv_scale * p.x()));
    auto yInt = int(std::floor(inv_scale * p.y()));
    auto zInt = int(std::floor(inv_scale * p.z()));

    bool isEven = (xInt + yInt + zInt) % 2 == 0;

    return isEven ? even->value(u, v, p) : odd->value(u,v,p);
  }

  private:
    double inv_scale;
    shared_ptr<texture> even;
    shared_ptr<texture> odd;
};

class image_texture : public texture {
  public:
  image_texture(const char* filename) : image(filename) {}

  color value(double u, double v, const point3& p) const override {
    // Return cyan if no image data found
    if (image.height() <= 0 || image.width() <= 0) return color(0,1,1);

    u = interval(0.0, 1.0).clamp(u);
    v = 1.0 - interval(0.0, 1.0).clamp(v); // flip v coords

    auto i = static_cast<int>(u * image.width());
    auto j = static_cast<int>(v * image.height());
    auto pixel = image.pixel_data(i, j);

    auto color_scale = 1.0 / 255.0;
    return color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
  }

  private:
    rtw_image image;

};

#endif