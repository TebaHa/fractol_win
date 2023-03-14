typedef struct		s_fractol
{
	int				type;
	int				width;
	int				height;
	int				iter;
	int				x;
	int				y;
	int				i;
	int				mi;
	int				color_type;
	int				color1;
	int				color2;
	int				color;
	__global int* palette;
	int				s_pallete[4];
	double			cx;
	double			cy;
	double			zx;
	double			zy;
	double			xtemp;
	double			left;
	double			top;
	double			xside;
	double			yside;
	double			scale;
	double			log_zn;
	double			nu;
	double			inter;
	double			julesx;
	double			julesy;
}					t_fractol;

double				lerp(double s, double e, double t)
{
	return (s + (e - s) * t);
}

void				put_on_image(int x, int y, int color, t_fractol data, __global int* mem_img)
{
	size_t	offset;

	if (x >= 0 && x < data.width && y >= 0 && y < data.height)
	{
		offset = x + data.width * y;
		mem_img[offset] = color;
	}
}

void				burningship(t_fractol data, __global int* palette, __global int* mem_img)
{
	data.cx = data.x * data.scale + data.left;
	data.cy = data.y * data.scale + data.top;
	data.zx = 0;
	data.zy = 0;
	data.i = 0;
	while ((data.zx * data.zx + data.zy * data.zy) < 4 && data.i < data.iter)
	{
		data.xtemp = data.zx * data.zx - data.zy * data.zy + data.cx;
		data.zy = fabs((2.0 * data.zx * data.zy)) + data.cy;
		data.zx = fabs(data.xtemp);
		data.i++;
	}
	if (data.color_type == 1)
		put_on_image(data.x, data.y, palette[data.i], data, mem_img);
	else
		put_on_image(data.x, data.y, data.s_pallete[data.i % 4], data, mem_img);
}

void				julia(t_fractol data, __global int* palette, __global int* mem_img)
{
	data.cx = data.x * data.scale + data.left;
	data.cy = data.y * data.scale + data.top;
	data.i = 0;
	data.mi = data.iter;
	while (((data.cx * data.cx + data.cy * data.cy) < 4) && data.i < data.iter)
	{
		data.xtemp = data.cx;
		data.cx = data.cx * data.cx - data.cy * data.cy + data.julesx;
		data.cy = 2.0 * data.xtemp * data.cy + data.julesy;
		data.i++;
	}
	if (data.color_type == 1)
		put_on_image(data.x, data.y, palette[data.i], data, mem_img);
	else
		put_on_image(data.x, data.y, data.s_pallete[data.i % 4], data, mem_img);
}

void				mandelbrot(t_fractol data, __global int* palette, __global int* mem_img)
{
	data.cx = data.x * data.scale + data.left;
	data.cy = data.y * data.scale + data.top;
	data.zx = 0.0;
	data.zy = 0.0;
	data.i = 0;
	data.mi = data.iter;
	while (((data.zx * data.zx + data.zy * data.zy) <= (1 << 16)) && data.i < data.iter)
	{
		data.xtemp = data.zx * data.zx - data.zy * data.zy + data.cx;
		data.zy = 2.0 * data.zx * data.zy + data.cy;
		data.zx = data.xtemp;
		data.i++;
	}
	/*
	if (data.i < data.mi)
	{
		data.log_zn = (double)log((double)((data.zx) * (data.zx) + (data.zy) * (data.zy))) / 2.0;
		data.nu = (double)log((double)data.log_zn / log(2.0)) / log(2.0);
		data.inter = data.i + 1 - data.nu;
	}
	else
	{
		data.inter = data.i;
	}
	data.color1 = data.palette[data.i];
	data.color2 = data.palette[(data.i + 1)];
	data.color = lerp(data.color1, data.color2, (double)modf(data.i, &data.nu));*/
	if (data.color_type == 1)
		put_on_image(data.x, data.y, palette[data.i], data, mem_img);
	else
		put_on_image(data.x, data.y, data.s_pallete[data.i % 4], data, mem_img);
}

static void			render(t_fractol data, __global int* mem_img)
{
	mandelbrot(data, data.palette, mem_img);
}

__kernel void		test(__global int* int_mem, __global double* double_mem, __global int* mem_img, __global int* palette)
{
	t_fractol		data;
	int 			gid;
	int 			flg;

	gid = get_global_id(0);
	flg = int_mem[2];
	data.width = int_mem[0];
	data.height = int_mem[1];
	data.x = gid % data.width;
	data.y = gid / data.width;
	data.iter = int_mem[3];
	data.s_pallete[0] = 0xFFFFFFFF;
	data.s_pallete[1] = 0x0;
	data.s_pallete[2] = 0x0;
	data.s_pallete[3] = 0x0;
	data.color_type = int_mem[8];
	data.cx = double_mem[0];
	data.cy = double_mem[1];
	data.zx = double_mem[2];
	data.zy = double_mem[3];
	data.left = double_mem[5];
	data.top = double_mem[6];
	data.scale = double_mem[7];
	data.palette = palette;
	data.type = int_mem[7];
	data.julesx = double_mem[8];
	data.julesy = double_mem[9];
	render(data, mem_img);
}