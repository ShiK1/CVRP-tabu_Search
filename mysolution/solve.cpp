/*
1. 将物流中心和客户都作为源点，求出它们到各自之间的距离，排序；
2. 接下来，就有路可循，从物流中心点开始出发，先找路径最短的点，走过去，计算约束条件，满足的情况下，找距离该点最近的点，且这个点未被访问过！
按照这个方法走，直到该车拉满或者距离不够走。重新从出发点找另外的点开始继续上述操作。直到全部客户都访问到。

上述方法的主要约束条件是   相对距离
*/
#include <cstdio>
#include <cmath>
#include <algorithm>

#define customer_number 21
struct customer
{
    double position_x;
    double position_y;
    double demands;
} c1[customer_number];

struct soulution
{
    double totalDistance;
    int carNumber;
    int result[5][20];
}s0[210];

bool tabuoperator[21][21] = {};//如果第i号和第j号进行了交换操作，则记录tabulist[i][j],tabulist[j][i]都为true；禁忌操作记录表；
struct tabuList
{
    int i,j;
    int step;
}tabu[10];//记录禁忌表以及步数




bool visitedCustomer[customer_number] = {false};                             //是否访问过客户的标记
int S[5][20] = {0};                                                          //存储已经访问过的客户
double distance_matrix[customer_number][customer_number];                    //客户之间的距离矩阵
double calculateDistace(double p1_x, double p1_y, double p2_x, double p2_y); //计算距离
void initial_distance_matrix(customer cust[], int n_cust);                   //计算客户之间的距离矩阵
double TSP(int visited[], double carCapacity, double TravelDistance);        //一辆车的满足条件的行驶
int visitedNum = 0;                                                          //已经访问的客户数量
int verifySequence(int seqi[], int road[5][10]);
int road1[5][10] = {};
int sequencei[20] = {1,7,16,13,6,11,20,12,2,9,15,19,8,5,10,18,17,3,4,14};
double totalD = 0;
int main(int argc, char const *argv[])
{
    for (int i = 0; i < 21; i++)
    {
        scanf("%lf%lf%lf", &c1[i].position_x, &c1[i].position_y, &c1[i].demands);
    }
    initial_distance_matrix(c1, customer_number);
    double distance = 0;
    int carNum = 0;
    while (visitedNum != 20)
    {
        distance += TSP(S[carNum++], 8.0, 50.0);
    }
    int carN = verifySequence(sequencei, road1);
    for (int i = 0; i < carN; i++)
    {
        for (int j = 1; j <= road1[i][0]; j++)
        {
            printf("%d ", road1[i][j]);
        }
        printf("\n");
    }
    printf("%.2f  ", totalD);
    printf("%.2f\n", distance);
    return 0;
}

double calculateDistace(double p1_x, double p1_y, double p2_x, double p2_y)
{
    //2点之间距离
    return sqrt((p1_x - p2_x) * (p1_x - p2_x) + (p1_y - p2_y) * (p1_y - p2_y));
}

void initial_distance_matrix(customer cust[], int n_cust)
{
    //计算距离矩阵
    for (int i = 0; i < n_cust; i++)
    {
        for (int j = 0; j < n_cust; j++)
        {
            distance_matrix[i][j] = calculateDistace(cust[i].position_x, cust[i].position_y, cust[j].position_x, cust[j].position_y);
            printf("%.2f ", distance_matrix[i][j]);
        }
        puts("");
    }
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

    while (carCapacity > 0 && TravelDistance > 0)
    {
        currentMin = 1000.0;
        for (int i = 1; i < customer_number; i++)
        {
            if (visitedCustomer[i] == false && distance_matrix[visited[j - 1]][i] < currentMin) //当前的客户未被访问过，并且上一个已经访问过的客户j到当前客户i的距离小于当前最小距离
            {
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

int verifySequence(int seqi[], int road[5][10])
{
    int carNum = 0;
    int visitedIndex = 0;
    while (visitedIndex < 20)
    {
        double carCap = 8, carDistance = 50;
        double sum1 = 0, sum2 = 0;
        int i = 1;
        road[carNum][i++] = 0;
        while (carCap > 0 && carDistance > 0)
        {
            carCap -= c1[seqi[visitedIndex]].demands;
            carDistance -= distance_matrix[road[carNum][i - 1]][seqi[visitedIndex]];
            if (carCap > 0 && carDistance - distance_matrix[seqi[visitedIndex]][0] > 0 && visitedIndex < 20)
            {
                sum1 += distance_matrix[road[carNum][i - 1]][seqi[visitedIndex]];
                sum2 += c1[seqi[visitedIndex]].demands;
                road[carNum][i++] = seqi[visitedIndex++];
            }
            else
            {
                sum1 += distance_matrix[road[carNum][i - 1]][0];
                printf("distance: %.2f  weight: %.2f\n", sum1, sum2);
                totalD += sum1;
                road[carNum][0] = i;
                road[carNum][i] = 0;
                carNum++;
                break;
            }
        }
    }
    return carNum;
}


void taBuSearch_Cvrp(soulution s){
    soulution sBest = s;

}