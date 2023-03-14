#include "Fractol.h"

ClData::ClData()
{
	cl_platform_id	platform;
	cl_device_id	dev;
	int				err;

	err = clGetPlatformIDs(1, &platform, NULL);

	if (err < 0)
	{
		std::cout << "Couldn't identify a platform\n";
	}

	err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);

	if (err == CL_DEVICE_NOT_FOUND)
	{
		err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
	}

	if (err < 0)
	{
		std::cout << "Couldn't access any devices\n";
	}

	DeviceId = dev;
	Return = err;
}

void ClData::InitBuffers()
{
	IntMem = clCreateBuffer(Context, CL_MEM_READ_WRITE, sizeof(int) * 32, NULL, &Return);
	ImgMem = clCreateBuffer(Context, CL_MEM_READ_ONLY, sizeof(int) * WIDTH * HEIGHT, NULL, &Return);
	DoubleMem = clCreateBuffer(Context, CL_MEM_READ_WRITE, sizeof(double) * 32, NULL, &Return);
	Palette = clCreateBuffer(Context, CL_MEM_READ_WRITE, sizeof(int) * 101, NULL, &Return);

	Return = clSetKernelArg(Kernel, 0, sizeof(cl_mem), &IntMem);
	Return = clSetKernelArg(Kernel, 1, sizeof(cl_mem), &DoubleMem);
	Return = clSetKernelArg(Kernel, 2, sizeof(cl_mem), &ImgMem);
	Return = clSetKernelArg(Kernel, 3, sizeof(cl_mem), &Palette);
}

ClRunner::ClRunner()
	: ClData()
{
	size_t kernelProgramSize;

	ClData.Context = clCreateContext(NULL, 1, &ClData.DeviceId, NULL, NULL, &ClData.Return);
	ClData.Queue = clCreateCommandQueue(ClData.Context, ClData.DeviceId, 0, &ClData.Return);

	std::ifstream kernelFile(KERNEL_PROGRAM_FILE);

	std::string kernelProgram(
		(std::istreambuf_iterator<char>(kernelFile)),
		std::istreambuf_iterator<char>()
	);

	const char* kernelCharArray = new char[kernelProgram.size()];
	kernelCharArray = kernelProgram.c_str();

	if (!kernelFile.is_open())
	{
		std::cout << "Couldn't find cl program\n";
		exit(-1);
	}

	kernelProgramSize = kernelProgram.size();

	ClData.Program = clCreateProgramWithSource(ClData.Context, 1, (const char**)&kernelCharArray, &kernelProgramSize, &ClData.Return);

	if (ClData.Return < 0)
	{
		std::cout << "Couldn't create the program'\n";
	}

	ClData.Return = clBuildProgram(ClData.Program, 0, NULL, NULL, NULL, NULL);

	if (ClData.Return < 0)
	{
		std::cout << "OpenCL build error\n";
		size_t log_size;
		clGetProgramBuildInfo(ClData.Program, ClData.DeviceId, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		char* log = (char*)malloc(log_size);
		clGetProgramBuildInfo(ClData.Program, ClData.DeviceId, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
		std::cout << log;
	}

	ClData.Kernel = clCreateKernel(ClData.Program, "test", &ClData.Return);

	if (ClData.Return < 0)
	{
		std::cout << "OpenCL create Kernel error\n";
	}

	ClData.InitBuffers();
}

void ClRunner::Run()
{
	size_t		kernel_num;
	int			ret;
	int			mem[32];
	double		dmem[32];

	kernel_num = WIDTH * HEIGHT;
	SetData(mem, dmem);
	ret = clEnqueueWriteBuffer(ClData.Queue, ClData.IntMem, CL_TRUE, 0, sizeof(int) * 32, mem, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(ClData.Queue, ClData.DoubleMem, CL_TRUE, 0, sizeof(double) * 32, dmem, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(ClData.Queue, ClData.Palette, CL_TRUE, 0, sizeof(int) * (FractalData.Iteration + 1), FractalData.Palette, 0, NULL, NULL);
	ret = clEnqueueNDRangeKernel(ClData.Queue, ClData.Kernel, 1, NULL, &kernel_num, NULL, 0, NULL, NULL);
	ret = clEnqueueReadBuffer(ClData.Queue, ClData.ImgMem, CL_TRUE, 0, sizeof(int) * WIDTH * HEIGHT, (int*)Surface->pixels, 0, NULL, NULL);
}

void ClRunner::SetData(int* IBuffer, double* DBuffer)
{
	IBuffer[0] = WIDTH;
	IBuffer[1] = HEIGHT;
	IBuffer[2] = 1;
	IBuffer[3] = FractalData.Iteration;
	IBuffer[5] = FractalData.I;
	IBuffer[6] = FractalData.Mi;
	IBuffer[7] = FractalData.Type;
	IBuffer[8] = FractalData.ColorType;
	DBuffer[0] = FractalData.Cx;
	DBuffer[1] = FractalData.Cy;
	DBuffer[2] = FractalData.Zx;
	DBuffer[3] = FractalData.Zy;
	DBuffer[4] = FractalData.XTemp;
	DBuffer[5] = FractalData.Left;
	DBuffer[6] = FractalData.Top;
	DBuffer[7] = FractalData.Scale;
	DBuffer[8] = FractalData.JulesX;
	DBuffer[9] = FractalData.JulesY;
}