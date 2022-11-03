#ifndef FILM_H
#define FILM_H

#include <nanogui/vector.h>

#include <vector>

/** This is a film class that holds the color buffer for the raytracer.
 */
class Film {
public:
    /** Constructor
     * @param resolution The image resolution.
     */
    Film(const nanogui::Vector2i& resolution)
        : resolution{(resolution.x() > 0 && resolution.y() > 0) ? resolution : nanogui::Vector2i{}}
    {
        const size_t size = resolution.x() * resolution.y();
        pixels.resize(size);
        pixelWeights.resize(size);
    }

    /** Add a color to a pixel in the buffer.
     * @param pixel_coordinate The pixel that should be updated.
     * @param color The color that should be added.
     */
    void addPixelColor(const nanogui::Vector2i& pixelCoordinate, const nanogui::Color& color)
    {
        size_t index = pixelCoordinate.x() + resolution.x() * pixelCoordinate.y();
        if (index >= pixels.size())
            return;

        // incremental mean: mean_{n+1} = (x + mean_{n}) (weight_{n+1} / sum_{i=0}^{n+1} weight_i)
        pixels[index] += (color - pixels[index]) * (1.0f / ++pixelWeights[index]);
    }

    const nanogui::Vector2i getResolution() const { return resolution; }

    const std::vector<nanogui::Color>& getPixels() const { return pixels; }

    /** Clears the buffers */
    void clear()
    {
        std::fill(pixels.begin(), pixels.end(), nanogui::Color{0.0f});
        std::fill(pixelWeights.begin(), pixelWeights.end(), 0.0f);
    }

private:
    nanogui::Vector2i resolution;
    std::vector<nanogui::Color> pixels;
    std::vector<float> pixelWeights;
};

#endif // !FILM_H
