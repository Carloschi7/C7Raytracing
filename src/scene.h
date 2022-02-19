#pragma once
#include <memory>
#include "light.h"
#include "objects.h"

class scene
{
public:
	scene() {}
	scene(const scene&) = default;
	
	//Add objects or lights
	template<class obj, typename... T>
	void add_elem(T&&... elems)
	{
		static_assert(std::is_base_of_v<object, obj> || std::is_same_v<obj, light>, "The class provided is not valid");

		if constexpr (std::is_base_of_v<object, obj>)
			m_ObjectList.push_back(std::make_shared<obj>(std::forward<T>(elems)...));
		else
			m_Lights.emplace_back(std::forward<T>(elems)...);
	}

	void replace_with(const object_list& objs)
	{
		m_ObjectList = objs;
	}

	const object_list& get_objects() const { return m_ObjectList; }
	const std::vector<light>& get_lights() const { return m_Lights; }

	static scene generate_random()
	{
		scene ret;
		ret.add_elem<sphere>(vec3(0.0f, -2000.0f, 0.0f), 1999.0f, std::make_shared<coarse>(vec3(0.5f, 0.5f, 0.5f)));
		//Big sphers
		vec3 pos[2] = { vec3(1.0f, 0.0f, -3.0f), vec3(-1.0f, 0.0f, -7.0f) };
		float fRadius = 1.0f;
		ret.add_elem<sphere>(pos[0], fRadius, std::make_shared<metal>(vec3(0.0f, 1.0f, 1.0f)));
		ret.add_elem<sphere>(pos[1], fRadius, std::make_shared<glass>(vec3(1.0f, 1.0f, 1.0f), 1.5f));

		std::shared_ptr<material> mat;
		for (int i = -5; i < 5; i++)
		{
			for (int j = -15; j < 5; j++)
			{
				float mat_rand = random_float(0.0f, 1.0f);
				vec3 color = vec3::ranged_random(0.0f, 1.0f);
				if (mat_rand < 0.5f)
					mat = std::make_shared<coarse>(color);
				else if (mat_rand < 0.8f)
					mat = std::make_shared<metal>(color);
				else
					mat = std::make_shared<glass>(vec3(1.0f, 1.0f, 1.0f), 1.5f);

				vec3 local_pos;
				int max_iter = 10;
				do {
					max_iter--;
					local_pos = vec3((float)i + random_float(0.0f, 0.5f), -0.8f, (float)j + random_float(0.0f, 0.5f));
				} while (max_iter > 0 && ((local_pos - pos[0]).length() < 1.2f || (local_pos - pos[1]).length() < 1.2f));//1.2f is the sum of the radii

				if (max_iter == 0)
					continue;

				ret.add_elem<sphere>(local_pos, 0.2f, mat);
			}
		}

		//Add lights
		ret.add_elem<light>(vec3(0.0f, 4.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f));
		ret.add_elem<light>(vec3(6.0f, 1.0f, 2.0f), vec3(1.0f, 0.0f, 1.0f));
		
		return ret;
	}

private:
	object_list m_ObjectList;
	std::vector<light> m_Lights;
};
