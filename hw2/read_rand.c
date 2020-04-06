#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>
//�ʿ��ϸ� header file �߰� ����


#define SUFFLE_NUM	10000	// �� ���� ������� ������ �� ����

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);
// �ʿ��� �Լ��� ������ �� �߰��� �� ����

//
// argv[1]: ���ڵ� ���ϸ�
//
int main((int argc, char **argv)
{
	int *read_order_list;
	int num_of_records;

	// �Ʒ� �Լ��� �����ϸ� 'read_order_list' �迭�� ���� �����ϰ� �о�� �� ���ڵ� ��ȣ���� ������� �����Ǿ� �����
            // 'num_of_records'�� ���ڵ� ���Ͽ� ����Ǿ� �ִ� ��ü ���ڵ��� ���� �ǹ���
	GenRecordSequence(read_order_list, num_of_records);


	// 'read_order_list'�� �̿��Ͽ� ǥ�� �Է����� ���� ���ڵ� ���Ϸκ��� ���ڵ带 random �ϰ� �о���̰�,
            // �̶� �ɸ��� �ð��� �����ϴ� �ڵ� ������


	return 0;
}

void GenRecordSequence(int *list, int n)
{
	int i, j, k;

	srand((unsigned int)time(0));

	for(i=0; i<n; i++)
	{
		list[i] = i;
	}
	
	for(i=0; i<SUFFLE_NUM; i++)
	{
		j = rand() % n;
		k = rand() % n;
		swap(&list[j], &list[k]);
	}

	return;
}

void swap(int *a, int *b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;

	return;
}
