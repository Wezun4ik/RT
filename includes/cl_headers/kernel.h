
#ifndef KERNEL_H
#define KERNEL_H
// #ifdef __APPLE__
// #include <OpenCL/opencl.h>
// #else
// #include <Cl/cl.h>
// #endif
// #include <float.h>

typedef struct s_ray
{
	float3 origin;
	float3 dir;
} t_ray;

typedef struct 		s_material
{
	float3			color;
	float3			emission;
	float			t;
}					t_material;


typedef struct		s_intersection
{
	t_ray			ray;
	float3			hit;
	float3			normal;
	t_material		material;
	int 			object_id;
}					t_intersection;


typedef enum e_figure
{
	 SPHERE, CYLINDER, CONE, PLANE
} t_type;

typedef struct Object{
	float radius;
	float3 position;
	float3 color;
	float3 emission;
	float3 v;
	t_type type;
	float refraction;
	float reflection;
	float plane_d;
} t_obj;

typedef struct		s_cam
{
	__float3		pos;
	__float3		dir;
	__float3		rot;
	__float3		updir;
	__float3		ldir;
	__float3		filter;
	double			fov;
	float			f_length;
	float			aperture;
	float			ratio;
	float			pr_pl_w;
	float			pr_pl_h;
	float			dust;
	float			brightness;
	float			refr_coef;
	int				effect;
}					t_cam;

#endif