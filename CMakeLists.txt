cmake_minimum_required(VERSION 3.12)

project(C7Raytracer)

add_executable(${PROJECT_NAME}
	src/main.cpp
	src/camera.h
	src/light.h
	src/objects.h
	src/raymath.h
	src/scene.h)