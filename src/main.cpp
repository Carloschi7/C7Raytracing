#include <iostream>
#include <string>
#include <fstream>
#include "raymath.h"
#include "scene.h"
#include "camera.h"


vec3 get_scene_color(const ray& r, const scene& world, int32_t max_iterations)
{
	hit_data data;
	vec3 color;
	data = world.get_objects().RayHasHitAny(r, 0.01f, std::numeric_limits<float>::infinity());
	if (max_iterations > 0 && data.bHasHit)
		return get_scene_color(data.mat->scatter(r, data, color), world, max_iterations - 1) * color;
	
	vec3 versor = r.direction.norm();
	/*float gradient = (versor.y() + 1.0f) * 0.5f;
	vec3 color1(1.0f, 1.0f, 1.0f);
	vec3 color2(0.5f, 0.7f, 1.0f);
	return color1 * gradient + color2 * (1.0f - gradient);*/

	//Handling lights

	if (max_iterations == 10)
		return vec3(0.3f);

	vec3 output;
	float fBrightness = 4.0f;
	for (const light& l : world.get_lights())
	{
		vec3 light_from_hitpoint = (l.position() - r.origin);
		float fFactor = std::max(dot(versor, light_from_hitpoint.norm()), 0.3f) / light_from_hitpoint.length() * fBrightness;
		fFactor = std::min(fFactor, 1.0f);
		output += l.color() * fFactor;
	}
	return output / world.get_lights().size();
}

void output_color(std::ofstream& file, vec3 color)
{
	file << std::to_string(color.x() * 256) << " "
		<< std::to_string(color.y() * 256) << " "
		<< std::to_string(color.z() * 256) << "\n";
}

int main()
{
	const int width = 1280, height = 768;
	int32_t iterations = 10;
	float fAspectRatio = (float)width / (float)height;
	std::ofstream file("src/output.ppm");

	std::string header = "P3\n" + std::to_string(width) + " " + std::to_string(height) + "\n" + "256\n";
	file.write(header.c_str(), header.size());

	//Framework logic
	camera cam(vec3(0.0f, 3.0f, 10.0f), vec3(0.0f, 0.0f, -2.0f), 20.0f, fAspectRatio);

	//Objects in the scene
	scene s = scene::generate_random();

	//write to a ppm file
	//Swap the height so the bottom left gets mapped to the actual bottom of the picture
	for (int j = height - 1; j >= 0; j--)
	{
		for (int i = 0; i < width; i++)
		{
			vec3 color;
			for (int k = 0; k < iterations; k++)
			{
				float u = ((float)i + random_float(0.0f, 1.0f)) / (float)(width - 1);
				float v = ((float)j + random_float(0.0f, 1.0f)) / (float)(height - 1);

				color += get_scene_color(cam.GetRay(u, v), s, iterations);
			}
			color /= iterations;
			output_color(file, color);
		}
	}

	file.close();
	return 0;
}