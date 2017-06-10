#version 330

uniform bool u_density_effect;
uniform int u_density_value;

uniform bool u_funny_effect;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f

in vec4 v_color;
in vec3 v_position_EC;

layout (location = 0) out vec4 final_color;

void main(void) {
	if (u_density_effect) {
		vec3 object_pos = v_position_EC - vec3(100, 0, 0);

		if (u_funny_effect) {
			vec2 center = vec2(75.0f, 75.0f);
			float standard_angle = 0.0f * TO_RADIAN;

			float x_angle = pow(object_pos.x - center.x, 2.0f) * TO_RADIAN;
			float y_angle = pow(object_pos.y - center.y, 2.0f) * TO_RADIAN;

			if ( cos(x_angle) * cos(y_angle) >= 0 )
				discard;
		}

		else if (u_density_value != 0) {
			float value_casting = float(u_density_value);
			float part_totalLength = 150 / value_casting;
			float part_blankStart = part_totalLength * 0.2f;
			float part_blankEnd = part_totalLength  - part_blankStart;

			int i, j;
			for (i = 0; i < u_density_value; i++) {
				for (j = 0 ; j < u_density_value; j++) {
					if (part_totalLength * i + part_blankStart <= object_pos.x &&
						object_pos.x <= part_totalLength * i + part_blankEnd &&
						part_totalLength * j + part_blankStart <= object_pos.y &&
						object_pos.y <= part_totalLength * j + part_blankEnd)
						discard;
				}
			}
		}
	}

    final_color = v_color;
}