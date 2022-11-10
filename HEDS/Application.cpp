#include <iostream>
#include <string>
#include "ConstructSolid/ConstructSolid.h"
#include "Renderer/Renderer.h"
using namespace std;
int main()
{
	std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  B-rep construction 大程作业 $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl;
	std::cout << "                                                                       by: 贺云鹏 22121031" << std::endl << std::endl;
	std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$          操作提示            $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl << std::endl;
	std::cout << "W  A  S  D  - 分别控制相机的前后左右移动" << std::endl;
	std::cout << "E  Q  -  分别控制相机的上升/下降" << std::endl;
	std::cout << "鼠标  - 控制相机视角的移动" << std::endl;
	std::cout << "F1  -  将视图切换为光照模式（default）" << std::endl;
	std::cout << "F2  -  将视图切换为非光照模式" << std::endl;
	std::cout << "F3  -  将视图切换为三角形绘制线框模式" << std::endl;
	std::cout << "F4  -  将视图切换为实体线框模式" << std::endl;
	std::cout << "ESC  -  退出程序" << std::endl << std::endl;

	std::cout << "输入1：展示自带的构造函数CreateCube" << std::endl;
	std::cout << "输入2：展示自带的构造函数CreateZero（有一个内环）" << std::endl;
	std::cout << "输入0：通过读取文件test.solid构造实体并展示（默认文件中的实体具有两个内环，可以修改文件以修改实体造型，文件格式见说明文档）" << std::endl;
	int mode;
	do {
		std::cout << "请输入：" ;
		std::cin >> mode;
	} while (mode != 0 && mode != 1 && mode != 2);
	ConstructSolid solid;
	if(!mode) solid.constructFromFile("res/solidConstructFiles/test.solid");
	if (mode == 1) solid.CreateCube();
	if (mode == 2) solid.CreateZero();
	Renderer renderer;
	renderer.SetSolid(solid.GetSolid());
	renderer.Draw();
	return 0;
}