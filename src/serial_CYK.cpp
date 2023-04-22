/*
The GPLv3 License (GPLv3)

Copyright (c) 2023 Yves Wong

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <cstdio>
#include <algorithm>
// #include <omp.h>
#include <time.h>
#include <memory.h>

using namespace std;

#define MAX_PRODUCTION2_NUM 512
#define MAX_PRODUCTION1_NUM 128
#define MAX_VN_NUM 128
#define MAX_VT_NUM 128
#define MAX_STRING_LENGTH 1024

struct BeginAndNum
{
    int begin;
    unsigned num;
};

struct Production2
{
    int parent;
    int child1;
    int child2;
} production2[MAX_PRODUCTION2_NUM];

struct Production1
{
    int parent;
    char child;
} production1[MAX_PRODUCTION1_NUM];

BeginAndNum vnIndex[MAX_VN_NUM][MAX_VN_NUM];
BeginAndNum vtIndex[MAX_VT_NUM];

char str[MAX_STRING_LENGTH];

struct SubTree
{
    int root;
    unsigned num;
} subTreeTable[MAX_STRING_LENGTH][MAX_STRING_LENGTH][MAX_VN_NUM];

int subTreeNumTable[MAX_STRING_LENGTH][MAX_STRING_LENGTH];

int vn_num;
int production2_num;
int production1_num;
int string_length;

int main()
{
    // freopen("input.txt", "r", stdin);
    scanf("%d\n", &vn_num); // vn_num -> 输入的公式里面涉及多少个变量
    scanf("%d\n", &production2_num); // 读入产生式的个数
    for (int i = 0; i < production2_num; i++) // 读入过程
        scanf("<%d>::=<%d><%d>\n", &production2[i].parent, &production2[i].child1, &production2[i].child2); // 可以看到这里仅仅涉及两个，放在结构体当中了
    scanf("%d\n", &production1_num); // 读入变量特化的数量
    for (int i = 0; i < production1_num; i++)
        scanf("<%d>::=%c\n", &production1[i].parent, &production1[i].child); // 对变量进行特化的具体情况
    scanf("%d\n", &string_length); // 读入被测试字符串的长度
    scanf("%s\n", str); // 读入被测试字符串

    // puts("input finished.");

    // 首先对特化的字符串进行排序？排序依据是什么呢？
    sort(production1, production1 + production1_num, [](const Production1& a, const Production1& b)
    {
        return a.child == b.child ? a.parent < b.parent : a.child < b.child; // 如果a相同则看变量编号，如果a不相同则a小的在前面
    });
    /*
     *
	<1>::=a
	<2>::=b
	<3>::=a

	会变成

	<1>::=a
	<3>::=a
	<2>::=b
     *
     */
    // 对一个奇怪的数据结构进行初始化
    for (int i = 0; i < MAX_VT_NUM; i++)
    {
        vtIndex[i].begin = -1;
        vtIndex[i].num = 0;
    }
    for (int i = 0; i < production1_num; i++) // Here we looking at part 2 of the input.
    {
        int t = production1[i].child; // 这里对应的就是变量特化部分，将字母取出来
        if (vtIndex[t].begin == -1)
            vtIndex[t].begin = i; // 这里记录第一个特化表当中的位置
        vtIndex[t].num++; // 这里记录特定特化表的长度
    }
    // possible speedup point.
    for (int i = 0; i < string_length; i++) // 从这里开始看字符串
    {
        int t = str[i]; // 拿出字符串的一位
        int begin = vtIndex[t].begin; // 拿出这一位所对应的特化表的第一个指定
        int end = begin + vtIndex[t].num; // 卡出特化表的最后一个
        for (int j = begin; j < end; j++) // 遍历特化表
        {
            SubTree subTree; // create a new SubTree
            subTree.root = production1[j].parent; // give this new subtree a parent -> the marker
            subTree.num = 1;
            // first every thread doing it's job and then, we need to clever reduce method.
            subTreeTable[i][i][subTreeNumTable[i][i]++] = subTree; // put this subtree into proper place.
        }
    }

    // put all parent together.
    sort(production2, production2 + production2_num, [](const Production2& a, const Production2& b)
    {
        return a.child1 == b.child1 ?
            (a.child2 == b.child2 ? a.parent < b.parent : a.child2 < b.child2)
            : a.child1 < b.child1;
    });
    for (int i = 0; i < vn_num; i++) // itearte the number of symbols.
    {
        for (int j = 0; j < vn_num; j++)
        {
            vnIndex[i][j].begin = -1;
            vnIndex[i][j].num = 0;
        }
    }
    for (int i = 0; i < production2_num; i++) // the formula.
    {
        int n1 = production2[i].child1;
        int n2 = production2[i].child2;
        if (vnIndex[n1][n2].begin == -1)
            vnIndex[n1][n2].begin = i;
        vnIndex[n1][n2].num++;
    }
    // here is where we need to speedup.
    for (int len = 2; len <= string_length; len++) // looking str starting from len 2.
    {
        for (int left = 0; left <= string_length - len; left++)
        {
            // 在串里取一段，然后遍历这个段
            SubTree subTreeBuf[2][MAX_STRING_LENGTH];// 其本身是个二维度量
            //memset(subTreeBuf, 0, sizeof subTreeBuf);
            int curr = 0;
            int last = 1;
            int oldTreeNum = 0;
            for (int right = left + 1; right < left + len; right++)
            {
                // 从调试输出我们知道这里我们把这个段分成两部分（B、C），那么我们应该如何将他们合并起来，然后更新上去呢？
                // 首先我们要对区间本身有概念
                // printf("[%d, %d] = [%d, %d] + [%d, %d]\n", left, left + len, left, right, right, left + len);
                // printf("len = %d, left = %d, right = %d\n", len, left, right);
                // here we define len, left and right.
                // 这里遍历的是什么？注意我们要落到字符串之上
                // 因为B和C本身是集合，所以我们显然要看正两个集合之中的情况
                for (int i1 = 0; i1 < subTreeNumTable[left][right - 1]; i1++) // find all related marker at this position range, for initial state(means i != j) this value seems to be 0.
                {
                    // printf("--> %d\n", i1);
                    SubTree subTreeChild1 = subTreeTable[left][right - 1][i1];
                    for (int i2 = 0; i2 < subTreeNumTable[right][left + len - 1]; i2++) // this algo NOT means split the seg into 2 part.
                    {
                        SubTree subTreeChild2 = subTreeTable[right][left + len - 1][i2];
                        // 好的，现在我们已经拿出分别属于两个集合的东西了，下面进行具体的合并
                        // get the transformation rules.
                        printf("get vnIndex: (%d, %d)\n", subTreeChild1.root, subTreeChild2.root);
                        int begin = vnIndex[subTreeChild1.root][subTreeChild2.root].begin;
                        int end = begin + vnIndex[subTreeChild1.root][subTreeChild2.root].num;
                        if (begin == end)
                        {
                            continue;
                        }
                        swap(last, curr);
                        int newTreeNum = 0;
                        int k = 0; // 这里的K是另一个关键，我们应该如何理解呢？
                        for (int j = begin; j < end; j++) // iterate them.
                        {
                            SubTree subTreeParent; // yes and how can we merge two child into it's parent?
                            subTreeParent.root = production2[j].parent;
                            subTreeParent.num = subTreeChild1.num * subTreeChild2.num; // !
                            // 下面是比较头疼的地方：他在两行数据之间来回折腾
                            while (k < oldTreeNum && subTreeParent.root > subTreeBuf[last][k].root)
                                subTreeBuf[curr][newTreeNum++] = subTreeBuf[last][k++];

                            if (k < oldTreeNum && subTreeParent.root == subTreeBuf[last][k].root)
                                subTreeParent.num += subTreeBuf[last][k++].num; // !
                          
                            subTreeBuf[curr][newTreeNum++] = subTreeParent;
                        }
                        while (k < oldTreeNum) // 从这里来看是把一些东西带回来了
                        {
                            subTreeBuf[curr][newTreeNum++] = subTreeBuf[last][k++];
                        }
                        oldTreeNum = newTreeNum;
                    }
                }
            }
            // 其实我们可以关注一下每次都有哪些元素放到subTreeTable里面了，因为这些东西最终都会放到大表里面！
            subTreeNumTable[left][left + len - 1] = oldTreeNum;
            if (subTreeNumTable[left][left + len - 1] > 0) // 如果产生了新状态就拼接上去
            {
                // 这里相当于将先前创建的状态都更新，而不是在后面拼接! 我们需要格外关注curr会在01之间变动！
                // 不必担心，因为其本身是个滚动数组，dp那味出来了
                // 说实话，我们最关心的是[0][len - 1][0]这个状态，因为他的.num就是答案！
                // 确实这里的curr和迭代的正负有关系阿
                memcpy(subTreeTable[left][left + len - 1], subTreeBuf[curr], subTreeNumTable[left][left + len - 1] * sizeof(SubTree));
                cout << "--------new tree node added-----------" << endl;
                cout << "curr = " << curr << endl;
                cout << "len = " << len << " left = " << left << endl;
                cout << "left = " << left << " end = " << left + len - 1 << endl;
                for(int pp = 0; pp < oldTreeNum; pp ++){
                  SubTree tt = subTreeBuf[curr][pp];
                  cout << "root = " << tt.root << " num = " << tt.num << endl;
                }
                cout << "-------------------" << endl;
            }
        }
    }
    // 从这里我们清晰看到，程序一开始生成所有基础状态，然后经过一通合并之后，得到了根，这个根里面记录的结果就是我们需要的结果。
    // 同时这也提醒我们可以追踪一下num的计算情况
    unsigned treeNum = 0;
    if (subTreeNumTable[0][string_length - 1] > 0)
    {
        if (subTreeTable[0][string_length - 1][0].root == 0)
        {
            treeNum = subTreeTable[0][string_length - 1][0].num;
        }
    }
    printf("%u\n", treeNum);
    return 0;
}
