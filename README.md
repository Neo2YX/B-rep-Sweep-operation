# CAD�����ҵ

������ 22121031

https://github.com/Neo2YX/B-rep-Sweep-operation

## ����ʹ��

����õĳ�����HEDS/HEDS.exe

�����Դ��������ú�������չʾ��cube�� zero��
����1 �� չʾcube
����2 �� չʾzero����һ���ڻ���

����ͬʱ֧�����ļ���HEDS/res/solidConstructFiles/test.solid������ʵ�岢չʾ�������޸Ĵ��ļ��Ըı����ɵ�ʵ��
����0 �� ���ļ�����ʵ�岢չʾ

## չʾ�������

W A S D  -  ���������ǰ�������ƶ�
E Q  -  �ֱ����������������½�
���  - ��������ӽǵ��ƶ�
F1  -  ����ͼ�л�Ϊ����ģʽ��default��
F2  -  ����ͼ�л�Ϊ�ǹ���ģʽ
F3  -  ����ͼ�л�Ϊ�����λ����߿�ģʽ
F4  -  ����ͼ�л�Ϊʵ���߿�ģʽ
ESC  -  �˳�����

## �������

windows��linux������ͨ��cmake���б���
```
cd build
cmake ..
cmake --build .
```
�������ⲿ�����⣬ʹ�õ�GLAD��GLFW��GLM��poly2tri������ڲ����Ӻ�

## test.solid�ļ���ʽ
�ļ�λ��Ϊ��HEDS/res/solidConstructFiles/test.solid
���ļ���������һ���棬֮��ͨ��sweep��������һ��ʵ�壬 ��ʽ���£�

>line 1         - numLoop numVertex1 numVertex2 ... numVertexn //numLoop - ����Ļ��������� �Լ�ÿ�����Ķ�����
line (2-n)      - x y z  //ÿ�����������λ��
final line      - x y z  //sweep��vector����

![](doc/testSolid.png)

## ���ͼƬ

![](doc/result1.png)
![](doc/result2.png)
![](doc/result3.png)