#include <cstdio>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <fstream>
#include <ctime>
using namespace std;

#define customer_number 21
struct customer
{
    double position_x;
    double position_y;
    double demands;
} c1[customer_number];

struct solution
{
    int sequence[20];
    int operatori;
    int operatorj;
    double totalDistance;
    int carNumber;
    int result[5][20];
} s[210];

bool tabuoperator[21][21] = {}; //如果第i号和第j号进行了交换操作，则记录tabulist[i][j],tabulist[j][i]都为true；禁忌操作记录表；
struct tabuList
{
    int step; //禁忌步数设置为10
    bool tag;
} tabu[21][21]; //记录禁忌表以及步数

int swapOpreator[200][2];
bool visitedCustomer[customer_number] = {false};          //是否访问过客户的标记
int S[5][20] = {0};                                       //存储已经访问过的客户
double distance_matrix[customer_number][customer_number]; //客户之间的距离矩阵
double weight_matrix[customer_number][customer_number];
int visitedNum = 0; //已经访问的客户数量
int road1[5][10] = {};

solution temp;

//************************function****************************
void initial_first_Solution(solution *s);
int cmpfunc(const void *a, const void *b)
{
    solution s1 = *(solution *)a;
    solution s2 = *(solution *)b;
    return (s1.totalDistance - s2.totalDistance);
}

bool cmp(solution a, solution b)
{
    return a.totalDistance < b.totalDistance;
}
double calculateDistace(double p1_x, double p1_y, double p2_x, double p2_y); //计算距离
void initial_distance_matrix(customer cust[], int n_cust);                   //计算客户之间的距离矩阵
double TSP(int visited[], double carCapacity, double TravelDistance);        //一辆车的满足条件的行驶
int verifySequence(solution *s);                                             //判断序列是否满足条件
void fun()
{
    int i = 0;
    for (int j = 0; j < 19; j++)
    {
        for (int k = j + 1; k <= 19; k++)
        {
            swapOpreator[i][0] = j;
            swapOpreator[i][1] = k;
            i++;
        }
    }
}
void swap(int *a, int *b);
int neighbor(solution a);
solution taBuSearch_Cvrp(solution s0, int iterations);
//************************function***************************

int main(int argc, char const *argv[])
{
    ifstream fin("temp.txt");
    fun();
    for (int i = 0; i < 21; i++)
    {
        //scanf("%lf%lf%lf", &c1[i].position_x, &c1[i].position_y, &c1[i].demands);
        fin >> c1[i].position_x;
        fin >> c1[i].position_y;
        fin >> c1[i].demands;
    }
    initial_distance_matrix(c1, customer_number);
    initial_first_Solution(&temp);

    clock_t start = clock();
    int iterations;
    printf("number of iterations :");
    scanf("%d", &iterations);
    solution res = taBuSearch_Cvrp(temp, iterations);
    clock_t end = clock();

    puts("*************************");

    for (int j = 0; j < res.carNumber; j++)
    {
        printf("car %d -->", j + 1);
        int k;
        for (k = 1; k < res.result[j][0] + 1; k++)
        {
            printf("%d ", res.result[j][k]);
        }
        printf("distance : %d ", res.result[j][k]);
        puts("");
    }
    puts("Best sequence: ");
    for (int i = 0; i < 20; i++)
    {
        printf("%d ", res.sequence[i]);
    }
    puts("");
    printf("run time cost : %.3f second\n", ((double)(end)-start) / CLOCKS_PER_SEC);
    printf("total dist: %.2f car number: %d\n", res.totalDistance, res.carNumber);
    puts("***************************\n");

    return 0;
}

double calculateDistace(double p1_x, double p1_y, double p2_x, double p2_y)
{
    //2点之间距离
    return sqrt((p1_x - p2_x) * (p1_x - p2_x) + (p1_y - p2_y) * (p1_y - p2_y));
}

void initial_distance_matrix(customer cust[], int n_cust)
{
    //计算距离权重矩阵
    for (int i = 0; i < n_cust; i++)
    {
        for (int j = 0; j < n_cust; j++)
        {
            if (i != j)
            {
                distance_matrix[i][j] = calculateDistace(cust[i].position_x, cust[i].position_y, cust[j].position_x, cust[j].position_y);
                //weight_matrix[i][j] = cust[j].demands / distance_matrix[i][j];
            }
            //printf("%9.2f", weight_matrix[i][j]);
        }
        //puts("");
    }
}

void initial_first_Solution(solution *s)
{
    printf("enter seqence: ");
    for (int i = 0; i < 20; i++)
    {
        scanf("%d", &s->sequence[i]);
    }
    s->carNumber = verifySequence(s);
    printf(" dis : %.2f\n", s->totalDistance);
}
//约束条件123
/*
1.每条配送路径上各客户的需求量之和不超过配送车辆的载重量；
2.每条配送路径的长度不超过配送车辆一次配送的最大行驶距离；
3.每个客户的需求必须满足,且只能由一台配送车辆送货。
*/
double TSP(int visited[], double carCapacity, double TravelDistance)
{
    double sum = 0;
    double currentMin;
    int j = 0;        //j代表当前已经访问过几个客户了
    visited[j++] = 0; //当前已经访问过的客户的，第一次默认为源点
    int k = 0;        //记录当前客户到最近的客户k
    double currentMaxWeight;
    while (carCapacity > 0 && TravelDistance > 0)
    {
        currentMin = 1000.0;
        currentMaxWeight = 0;
        for (int i = 1; i < customer_number; i++)
        {
            //if (visitedCustomer[i] == false && distance_matrix[visited[j - 1]][i] < currentMin) //当前的客户未被访问过，并且上一个已经访问过的客户j到当前客户i的距离小于当前最小距离
            if (visitedCustomer[i] == false && weight_matrix[visited[j - 1]][i] > currentMaxWeight) //当前的客户未被访问过，并且上一个已经访问过的客户j到当前客户i的距离小于当前最小距离
            {
                currentMaxWeight = weight_matrix[visited[j - 1]][i];
                currentMin = distance_matrix[visited[j - 1]][i];
                k = i;
            }
        }
        double nowCap = carCapacity - c1[k].demands;
        double nowDis = TravelDistance - currentMin - distance_matrix[k][0];
        if (nowCap > 0 && nowDis > 0)
        {
            visited[j] = k;
            visitedCustomer[visited[j++]] = true;
            sum += currentMin;
            carCapacity = nowCap;
            TravelDistance -= currentMin;
        }
        else
        {
            visited[j] = 0; //车回
            sum += distance_matrix[visited[j - 1]][0];
            break;
        }
        printf("%.2f ", sum);
    }
    printf("******%.2f\n", sum);
    for (int i = 0; i <= j; i++)
    {
        printf("%d ", visited[i]);
    }
    visitedNum += j - 1;
    printf("\n%d\n", visitedNum);
    return sum;
}

int verifySequence(solution *s)
{
    //ofstream fout("fout.txt");
    s->carNumber = 0;
    s->totalDistance = 0;
    int visitedIndex = 0;
    while (visitedIndex < 20)
    {
        double carCap = 8, carDistance = 50;
        double sum1 = 0, sum2 = 0;
        int i = 1;
        s->result[s->carNumber][i++] = 0;
        while (carCap > 0 && carDistance > 0)
        {
            carCap -= c1[s->sequence[visitedIndex]].demands;
            carDistance -= distance_matrix[s->result[s->carNumber][i - 1]][s->sequence[visitedIndex]];
            if (carCap > 0 && carDistance - distance_matrix[s->sequence[visitedIndex]][0] > 0 && visitedIndex < 20)
            {
                sum1 += distance_matrix[s->result[s->carNumber][i - 1]][s->sequence[visitedIndex]];
                sum2 += c1[s->sequence[visitedIndex]].demands;
                s->result[s->carNumber][i++] = s->sequence[visitedIndex++];
            }
            else
            {
                sum1 += distance_matrix[s->result[s->carNumber][i - 1]][0];
                s->totalDistance += sum1;
                //printf("distance: %.2f  weight: %.2f totaldis: %.2f\n", sum1, sum2, s->totalDistance);
                s->result[s->carNumber][0] = i;
                s->result[s->carNumber][i] = 0;
                s->result[s->carNumber][i + 1] = (int)sum1;
                s->carNumber++;
                break;
            }
        }
    }
    return s->carNumber;
}
void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}
int neighbor(solution a)
{
    int n = 0;
    for (int i = 0; i < 190; i++)
    {
        s[n] = a;
        // for (int j = 0; j < 20; j++)
        // {
        //     printf("%d ", s[n].sequence[j]);
        // }
        //puts("");
        swap(&s[n].sequence[swapOpreator[i][0]], &s[n].sequence[swapOpreator[i][1]]);
        // for (int j = 0; j < 20; j++)
        // {
        //     printf("%d ", s[n].sequence[j]);
        // }
        //puts("");
        int carN = verifySequence(&s[n]);
        if (carN <= 5)
        {
            //printf("%d %d  %.2f\n", n, carN, s[n].totalDistance);
            s[n].operatori = s[n].sequence[swapOpreator[i][1]];
            s[n].operatorj = s[n].sequence[swapOpreator[i][0]];
            n++;
        }
    }
    sort(s, s + n, cmp);
    //qsort(s, n, sizeof(solution), cmpfunc);
    return n;
}

solution taBuSearch_Cvrp(solution s0, int iterations)
{
    solution sBest = s0;
    solution sNow = s0;
    while (iterations > 0) //未超过迭代次数
    {
        int candidateNum = neighbor(sNow);
        for (int i = 0; i < candidateNum; i++)
        {
            if (tabu[s[i].operatori][s[i].operatorj].tag == true && s[i].totalDistance < sBest.totalDistance)
            {
                sNow = s[i];
                break;
            }
            else if (tabu[s[i].operatori][s[i].operatorj].tag == false)
            {
                sNow = s[i];
                break;
            }
        }
        for (int k = 1; k < 21; k++)
        {
            for (int j = 1; j < 21; j++)
            {
                if (tabu[k][j].tag == true)
                {
                    tabu[k][j].step--;
                    if (tabu[k][j].step == 0)
                    {
                        tabu[k][j].tag = false;
                        tabu[j][k].tag = false;
                    }
                }
            }
        }
        tabu[sNow.operatori][sNow.operatorj].tag = true;
        tabu[sNow.operatori][sNow.operatorj].step = 20;
        tabu[sNow.operatorj][sNow.operatori].tag = true;
        tabu[sNow.operatorj][sNow.operatori].tag = 20;

        if (sNow.totalDistance < sBest.totalDistance)
        {
            sBest = sNow;
        }
        //printf("%d\n", iterations);
        //printf("%.2f\n", sNow.totalDistance);
        iterations--;
    }
    return sBest;
}