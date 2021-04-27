#pragma once

namespace visuals
{
	extern D3DXMATRIX view_matrix;
	__forceinline void transpose_matrix(D3DXMATRIX* m, const D3DXMATRIX* a)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				m->m[i][j] = a->m[j][i];
	}

	inline bool world_to_screen(vec3 in, vec3* out)
	{
		D3DXMATRIX temp_matrix;
		visuals::transpose_matrix(&temp_matrix, &visuals::view_matrix);

		vec3 translation_vector = { temp_matrix._41, temp_matrix._42, temp_matrix._43 };
		vec3 up = { temp_matrix._21, temp_matrix._22, temp_matrix._23 };
		vec3 right = { temp_matrix._11, temp_matrix._12, temp_matrix._13 };

		float w = translation_vector.dot(in) + temp_matrix._44;
		if (w < 0.098f)
			return false;

		float x = in.dot(right) + temp_matrix._14;
		float y = in.dot(up) + temp_matrix._24;

		out->x = overlay::screen_center.x * (1.0f + x / w);
		out->y = overlay::screen_center.y * (1.0f - y / w);
		return true;
	}

	void on_render();
}