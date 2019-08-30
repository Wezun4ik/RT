
static int	ft_rgb_to_hex(int r, int g, int b)
{
	return (r << 16 | g << 8 | b);
}

static float u_clamp(float x)
{
	return x < 0.0f ? 0.0f : x > 1.0f ? 1.0f : x;
}

static int toInt(float x)
{ 
	return int(u_clamp(x) * 255);
}

__kernel void render_kernel(__global int* output, int width, int height)
{
	
	unsigned int work_item_id = get_global_id(0);	/* the unique global id of the work item for the current pixel */
	unsigned int x_coord = work_item_id % width;			/* x-coordinate of the pixel */
	unsigned int y_coord = work_item_id / width;			/* y-coordinate of the pixel */
	 float fx = (float)x_coord / (float)width; 
 	float fy = (float)y_coord / (float)height;
	output[x_coord + y_coord * width] = ft_rgb_to_hex(toInt(fx), toInt(fy), toInt(0)); /* simple interpolated colour gradient based on pixel coordinates */
}