#include <iostream>
#include <string>
#include "ConstructSolid/ConstructSolid.h"
#include "Renderer/Renderer.h"
using namespace std;
int main()
{
	std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  B-rep construction �����ҵ $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl;
	std::cout << "                                                                       by: ������ 22121031" << std::endl << std::endl;
	std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$          ������ʾ            $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl << std::endl;
	std::cout << "W  A  S  D  - �ֱ���������ǰ�������ƶ�" << std::endl;
	std::cout << "E  Q  -  �ֱ�������������/�½�" << std::endl;
	std::cout << "���  - ��������ӽǵ��ƶ�" << std::endl;
	std::cout << "F1  -  ����ͼ�л�Ϊ����ģʽ��default��" << std::endl;
	std::cout << "F2  -  ����ͼ�л�Ϊ�ǹ���ģʽ" << std::endl;
	std::cout << "F3  -  ����ͼ�л�Ϊ�����λ����߿�ģʽ" << std::endl;
	std::cout << "F4  -  ����ͼ�л�Ϊʵ���߿�ģʽ" << std::endl;
	std::cout << "ESC  -  �˳�����" << std::endl << std::endl;

	std::cout << "����1��չʾ�Դ��Ĺ��캯��CreateCube" << std::endl;
	std::cout << "����2��չʾ�Դ��Ĺ��캯��CreateZero����һ���ڻ���" << std::endl;
	std::cout << "����0��ͨ����ȡ�ļ�test.solid����ʵ�岢չʾ��Ĭ���ļ��е�ʵ����������ڻ��������޸��ļ����޸�ʵ�����ͣ��ļ���ʽ��˵���ĵ���" << std::endl;
	int mode;
	do {
		std::cout << "�����룺" ;
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