#include <stdio.h>

int main()
{
    return 0;
}

void Swap(int *p, int *q)
{
    int buf;
    buf = *p;
    *p = *q;
    *q = buf;
    return;
}
void QuickSort(int *a, int low, int high, int k)
{
    int i = low;
    int j = high;
    int key = a[low];
    if (low >= high && low == k) //如果low >= high说明排序结束了
    {
        printf("%d",a[low]);
        return;
    }
    while (low < high) //该while循环结束一次表示比较了一轮
    {
        while (low < high && key <= a[high])
        {
            --high; //向前寻找
        }
        if (key > a[high])
        {
            Swap(&a[low], &a[high]);
            ++low;
        }
        while (low < high && key >= a[low])
        {
            ++low; //向后寻找
        }
        if (key < a[low])
        {
            Swap(&a[low], &a[high]);
            --high;
        }
    }
    if(low < k){
        QuickSort(a, i,);
    }
    QuickSort(a, low, low - 1); //用同样的方式对分出来的左边的部分进行同上的做法
    QuickSort(a, low + 1, j); //用同样的方式对分出来的右边的部分进行同上的做法
}