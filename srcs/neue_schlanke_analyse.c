/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   neue_schlanke_analyse.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: srobert- <srobert-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/24 17:46:45 by srobert-          #+#    #+#             */
/*   Updated: 2019/11/01 22:26:45 by srobert-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"
#include "errno.h"

static void parse_necessary(const cJSON *object, t_obj *obj, t_json *parse)
{
	parse->position = cJSON_GetObjectItemCaseSensitive(object, "position");
	obj->position = parse_vec3(parse->position);
	if (isnan(obj->position.v4[0]) && obj->type != TRIANGLE)
		terminate("missing data of obj position!\n");
	obj->position = sum_cfloat3(obj->position, obj->composed_pos);
	parse->v = cJSON_GetObjectItemCaseSensitive(object, "dir");
	obj->v = parse_vec3(parse->v);
	if (isnan(obj->v.v4[0]) && obj->type != SPHERE && obj->type != TRIANGLE)
		terminate("missing data of obj dir !\n");
	obj->v =normalize(sum_cfloat3(obj->v, obj->composed_v));
	parse->color = cJSON_GetObjectItemCaseSensitive(object, "color");
	obj->color = parse_vec3(parse->color);
	if (isnan(obj->color.v4[0]))
		terminate("missing data of obj color vector!\n");
	parse->radius = cJSON_GetObjectItemCaseSensitive(object, "radius");
	if (parse->radius != NULL)
		obj->radius = parse->radius->valuedouble;
	else if (obj->type != PLANE && obj->type != TRIANGLE)
		terminate("missing data of obj radius!\n");
}

static void parse_facing(const cJSON *object, t_obj *obj, t_json *parse, t_game *game)
{
	parse->emition = cJSON_GetObjectItemCaseSensitive(object, "emition");
	if (parse->emition != NULL)
	{
		obj->emission = parse_vec3(parse->emition);
		if (isnan(obj->position.v4[0]))
		   terminate("missing data of obj emition vector!\n");
	}
	else
		obj->emission = create_cfloat3(0.0, 0.0, 0.0);
	parse->reflection = cJSON_GetObjectItemCaseSensitive(object, "reflection");
	if (parse->reflection != NULL)
		obj->reflection = parse->reflection->valuedouble;
	else
		obj->reflection = 0.0;	
	parse->texture = cJSON_GetObjectItemCaseSensitive(object, "texture");


	
	if (parse->texture != NULL)
	{
		obj->texture = compare_in_dict(game, game->texture_list, parse->texture->valuestring);
		if (obj->texture == -1)
			obj->texture = 0;
	}

	
	else
		obj->texture = 0;
	parse->normal = cJSON_GetObjectItemCaseSensitive(object, "normal");
	if (parse->normal != NULL)
		obj->normal = (int)parse->normal->valuedouble;
	else
		obj->normal = 0;
	parse->shift = cJSON_GetObjectItemCaseSensitive(object, "shift");
	if (parse->shift != NULL)
	{
		obj->shift = parse_vec2(parse->shift);
		if (isnan(obj->shift.s[0]))
		   terminate("missing data of obj shift vector!\n");
	}
	else
		obj->shift = create_cfloat2(0.0, 0.0);

}

static void plane_basis(const cJSON *object, t_obj *obj, t_json *parse)
{
	parse->x_basis = cJSON_GetObjectItemCaseSensitive(object, "x_basis");
	if (parse->x_basis != NULL)
	{
		obj->basis[0] = parse_vec3(parse->x_basis);
		if (isnan(obj->basis[0].v4[0]))
		   terminate("missing data of plane x_basis vector!\n");
	}
	else
	{
		if (obj->v.s[0] != 0.0f || obj->v.s[1] != 0.0f)
			obj->basis[0] = normalize(create_cfloat3(obj->v.s[1], -obj->v.s[0], 0));
		else
			obj->basis[0] = create_cfloat3(0.0f, 1.0f, 0.0f);
	}
	parse->y_basis = cJSON_GetObjectItemCaseSensitive(object, "y_basis");
	if (parse->y_basis != NULL)
	{
		obj->basis[1] = parse_vec3(parse->y_basis);
		if (isnan(obj->basis[1].v4[0]))
		   terminate("missing data of plane y_basis vector!\n");
	}
	else
		obj->basis[1] = cross(obj->v, obj->basis[0]);
	parse->z_basis = cJSON_GetObjectItemCaseSensitive(object, "z_basis");
	if (parse->z_basis != NULL)
	{
		obj->basis[2] = parse_vec3(parse->z_basis);
		if (isnan(obj->basis[2].v4[0]))
		   terminate("missing data of plane z_basis vector!\n");
	}
	else
		obj->basis[2] = cross(obj->basis[0], obj->basis[1]);
}

static void sphere_basis(const cJSON *object, t_obj *obj, t_json *parse)
{
	parse->x_basis = cJSON_GetObjectItemCaseSensitive(object, "x_basis");
	if (parse->x_basis != NULL)
	{
		obj->basis[0] = parse_vec3(parse->x_basis);
		if (isnan(obj->basis[0].v4[0]))
		   terminate("missing data of sphere x_basis vector!\n");
	}
	else
		obj->basis[0] = create_cfloat3(1.0, 0.0, 0.0);
	parse->y_basis = cJSON_GetObjectItemCaseSensitive(object, "y_basis");
	if (parse->y_basis != NULL)
	{
		obj->basis[1] = parse_vec3(parse->y_basis);
		if (isnan(obj->basis[1].v4[0]))
		   terminate("missing data of sphere y_basis vector!\n");
	}
	else
		obj->basis[1] = create_cfloat3(0.0, 1.0, 0.0);
	parse->z_basis = cJSON_GetObjectItemCaseSensitive(object, "z_basis");
	if (parse->z_basis != NULL)
	{
		obj->basis[2] = parse_vec3(parse->z_basis);
		if (isnan(obj->basis[2].v4[0]))
		   terminate("missing data of sphere z_basis vector!\n");
	}
	else
		obj->basis[2] = cross(obj->basis[0], obj->basis[1]);
}

static void cylinder_basis(const cJSON *object, t_obj *obj, t_json *parse)
{
	parse->x_basis = cJSON_GetObjectItemCaseSensitive(object, "x_basis");
	if (parse->x_basis != NULL)
	{
		obj->basis[0] = parse_vec3(parse->x_basis);
		if (isnan(obj->basis[0].v4[0]))
		   terminate("missing data of cylinder x_basis vector!\n");
	}
	else
	{
		if (obj->v.s[0] != 0.0f || obj->v.s[1] != 0.0f)
			obj->basis[0] = normalize(create_cfloat3(obj->v.s[1], -obj->v.s[0], 0));
		else
			obj->basis[0] = create_cfloat3(0.0f, 1.0f, 0.0f);
	}
	parse->y_basis = cJSON_GetObjectItemCaseSensitive(object, "y_basis");
	if (parse->y_basis != NULL)
	{
		obj->basis[1] = parse_vec3(parse->y_basis);
		if (isnan(obj->basis[1].v4[0]))
		   terminate("missing data of cylinder y_basis vector!\n");
	}
	else
		obj->basis[1] = cross(obj->basis[0], obj->v);
	parse->z_basis = cJSON_GetObjectItemCaseSensitive(object, "z_basis");
	if (parse->z_basis != NULL)
	{
		obj->basis[2] = parse_vec3(parse->z_basis);
		if (isnan(obj->basis[2].v4[0]))
		   terminate("missing data of cylinder z_basis vector!\n");
	}
	else
		obj->basis[2] = create_cfloat3(0.0, 0.0, 1.0);
}

static void parse_basis(const cJSON *object, t_obj *obj, t_json *parse)
{
	if (obj->type == SPHERE)
		sphere_basis(object, obj, parse);
	else if (obj->type == CYLINDER)
		cylinder_basis(object, obj, parse);
	else
		plane_basis(object, obj, parse);
}

static void parse_rest(const cJSON *object, t_obj *obj, t_json *parse)
{
	parse->rotation = cJSON_GetObjectItemCaseSensitive(object, "rotation");
	if (parse->rotation != NULL)
	{
		obj->rotation = parse_vec2(parse->rotation);
		if (isnan(obj->rotation.s[0]))
		   terminate("missing data of obj rotation vector!\n");
	}
	else
		obj->rotation = create_cfloat2(0.0, 0.0);
	parse->prolapse = cJSON_GetObjectItemCaseSensitive(object, "prolapse");
	if (parse->prolapse != NULL)
	{
		obj->prolapse = parse_vec2(parse->rotation);
		if (isnan(obj->prolapse.s[0]))
           terminate("missing data of cylinder prolapse vector!\n");
    }
    else
    	obj->prolapse = create_cfloat2(1000.0, 1000.0);
}

 static cl_float3 triangle_norm(cl_float3 *vertices)
{
	cl_float3 ab;
	cl_float3 ac;

	ab = vector_diff(vertices[1], vertices[0]);
	ac = vector_diff(vertices[2], vertices[0]);
	return(normalize(cross(ab, ac)));
}

static void parse_triangle_vert(const cJSON *object, t_obj *obj, t_json *parse)
{
	parse->a = cJSON_GetObjectItemCaseSensitive(object, "a");
	obj->vertices[0] = parse_vec3(parse->a);
	if (isnan(obj->vertices[0].v4[0]))
		terminate("missing data of triangle vertice!\n");
	parse->b = cJSON_GetObjectItemCaseSensitive(object, "b");
	obj->vertices[1] = parse_vec3(parse->b);
	if (isnan(obj->vertices[01].v4[0]))
		terminate("missing data of triangle vertice!\n");
	parse->c = cJSON_GetObjectItemCaseSensitive(object, "c");
	obj->vertices[2] = parse_vec3(parse->c);
	if (isnan(obj->vertices[2].v4[0]))
		terminate("missing data of triangle vertice!\n");
	obj->v = triangle_norm(obj->vertices);
}

static void parse_composed(const cJSON *composed, t_game *game, t_json *parse, int id)
{
    const cJSON *object = NULL;
    const cJSON *objects = NULL;
	parse->composed_pos = cJSON_GetObjectItemCaseSensitive(composed, "position");
	parse->composed_v = cJSON_GetObjectItemCaseSensitive(composed, "dir");

    objects = cJSON_GetObjectItemCaseSensitive(composed, "objects");
	cJSON_ArrayForEach(object, objects)
	{
		check_object(object, game, parse->composed_pos, parse->composed_v, id);
	}
}

void check_object(const cJSON *object, t_game *game, cJSON *composed_pos, cJSON *composed_v, int id)
{
	t_obj *obj;
	t_json parse;

	obj = (t_obj*)malloc(sizeof(t_obj));
	if (composed_pos != NULL)
	{
		obj->composed_pos = parse_vec3(composed_pos);
		if (isnan(obj->composed_pos.v4[0]))
			terminate("missing data of position of composed object!\n");
	}
	else
		obj->composed_pos = create_cfloat3(0.0, 0.0, 0.0);
	if (composed_v != NULL)
	{
		obj->composed_v = parse_vec3(composed_v);
		if (isnan(obj->composed_v.v4[0]))
			terminate("missing data of direction of composed object!\n");
	}
	else
		obj->composed_v = create_cfloat3(0.0, 0.0, 0.0);
	parse.type = cJSON_GetObjectItemCaseSensitive(object, "type");
	if (ft_strcmp(parse.type->valuestring, "plane") == 0)
		obj->type = PLANE;
	else if (ft_strcmp(parse.type->valuestring, "sphere") == 0)
		obj->type = SPHERE;
	else if (ft_strcmp(parse.type->valuestring, "cylinder") == 0)
		obj->type = CYLINDER;
	else if (ft_strcmp(parse.type->valuestring, "cone") == 0)
		obj->type = CONE;
	else if (ft_strcmp(parse.type->valuestring, "triangle") == 0)
		obj->type = TRIANGLE;
	else if (ft_strcmp(parse.type->valuestring, "composed") == 0)
	{
		parse_composed(object, game, &parse, id);
		return;
	}
	obj->id = id;
	parse_necessary(object, obj, &parse);
	parse_facing(object, obj, &parse, game);
	parse_basis(object, obj, &parse);
	if (obj->type == TRIANGLE)
		parse_triangle_vert(object, obj, &parse);
	parse_rest(object, obj, &parse);
	ft_object_push(game, obj);
}

void check_cam(const cJSON *cam, t_game *game)
{
	t_json parse;
	t_cam *camera;

	camera = (t_cam*)malloc(sizeof(t_cam));
	parse.position = cJSON_GetObjectItemCaseSensitive(cam, "position");
	camera->position = parse_vec3(parse.position);
	if (isnan(camera->position.v4[0]))
		terminate("missing data of cam start pos vector!\n");
	parse.v = cJSON_GetObjectItemCaseSensitive(cam, "dir");
	camera->direction = normalize(parse_vec3(parse.v));
	if (isnan(camera->direction.v4[0]))
		terminate("missing data of cam dir vector!\n");
	parse.normal = cJSON_GetObjectItemCaseSensitive(cam, "normal");
	if (parse.normal != NULL)
	{
		camera->normal = parse_vec3(parse.normal);
		if (isnan(camera->normal.v4[0]))
		   terminate("missing data of cam normal vector!\n");
	}
	else
		camera->normal = create_cfloat3(0.0, 1.0, 0.0);
	camera->fov = M_PI / 3;
	reconfigure_camera(camera);
	ft_cam_push(game, camera);
}



void read_scene(char *argv, t_game *game)
{

	free(game->textures);
	free(game->normals);
	FILE *fp;
	char buffer[8096];
	fp = fopen(argv, "r");
	if (!fp || ft_strcmp(&argv[ft_strlen(argv) - 5], ".json") != 0)
		terminate("fuck you and your file!\n");
	fread(buffer, 8096, 1, fp);
	cJSON *json = cJSON_Parse(buffer);

	const cJSON *texture = NULL;
	const cJSON *textures = NULL;
	// char **mass = NULL;
	int i = 0;
	textures = cJSON_GetObjectItemCaseSensitive(json, "textures");
	cJSON_ArrayForEach(texture, textures)
	{
		if (compare_in_dict(game, game->texture_list, texture->valuestring) == -1)
			ft_texture_push(game, &(game->texture_list), texture->valuestring);
	}
	game->textures = (t_txture*)malloc(sizeof(t_txture) * game->textures_num);
	while(i < game->textures_num)
	{
		get_texture(game->texture_list[i], &(game->textures[i]), "./textures/");
		// printf("%s\n", mass[i]);
		i++;
	}
	const cJSON *normal = NULL;
	const cJSON *normals = NULL;
	int k = 0;
	normals = cJSON_GetObjectItemCaseSensitive(json, "normals");
	game->normals_num = cJSON_GetArraySize(normals);
	game->normals = (t_txture*)malloc(sizeof(t_txture) * game->normals_num);
	cJSON_ArrayForEach(normal, normals)
	{
		get_texture(normal->valuestring, &(game->normals[k]), "./normals/");
		k++;
	}
	const cJSON *object = NULL;
	const cJSON *objects = NULL;
	int id = 0;
	objects = cJSON_GetObjectItemCaseSensitive(json, "objects");
	cJSON_ArrayForEach(object, objects)
	{
		check_object(object, game, NULL, NULL, id);
		id++;
	}
	const cJSON *cam = NULL;
	const cJSON *cameras = NULL;
	cameras = cJSON_GetObjectItemCaseSensitive(json, "cameras");
	cJSON_ArrayForEach(cam, cameras)
	{
		check_cam(cam, game);
	}
	cJSON_Delete(json);
}