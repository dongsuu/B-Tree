#include<stdio.h>
#include <stdlib.h>
#include<time.h>

typedef struct BTreeNode {
	int* key; // node가 가지는 key배열.
	//child는 2*t만큼 가질 수 있다. 
	struct BTreeNode* child[18];  // node가 가지는 child pointer t=2일때. (t값 바꾸려면 이 값도 바꿔줘야함)
	int leaf; // leaf node인지 아닌지. 1:true, 0: false
	int numOfKeys; // node가 가지고 있는 key 개수. 
}BTreeNode;

typedef struct BTree {
	BTreeNode* root; // tree의 root 
}BTree;

// 할당 받은 node 초기화 
void initialize_Node(BTreeNode* node, int t) {
	node->key = malloc(sizeof(int) * 2 * t - 1);  // key는 2*(degree) - 1개 만큼 가질 수 있다. 
	node->leaf = 1;
	node->numOfKeys = 0;
}

// Tree 생성하는 함수 
void createBTree(BTree* tree, int t) {
	BTreeNode* node = malloc(sizeof(BTreeNode));

	if (node == NULL) { // 할당 실패시.
		printf("할당 실패했습니다.");
		return;
	}
	// root node 초기화 
	initialize_Node(node, t);

	//tree의 root node가 된다. 
	tree->root = node;
}

// Display 함수. Tree의 형태 보여준다. 
void Display(BTreeNode* node, int height)
{
	printf("[%d] : ", height);
	for (int i = 0; i < node->numOfKeys; i++)
	{
		printf("%d ", node->key[i]);
	}
	printf("\n");
	if (node->leaf == 0)
	{
		for (int i = 0; i < node->numOfKeys + 1; i++)
		{
			Display(node->child[i], height + 1);
		}
	}
}

void display(BTree* tree) {
	BTreeNode* r = tree->root;
	Display(r, 0);
}


// split함수
void split(BTreeNode* x, int i, int t) {  // i는 x(cur)의 몇번째 child에서 split이 이뤄지는지 넘겨줌. 
	// root node 이면 인자로 받은 x가 middle key가 들어있는 것이고, x가 root가 된다. root node가 아니면 x에는 split하려는 node의 parent node가 들어있음. 
	BTreeNode* z = malloc(sizeof(BTreeNode));  // z는 middle key보다 큰 값 가지는 node (x의 오른쪽 child로 감)
	initialize_Node(z,t);
	BTreeNode* y = x->child[i];               // y는 x의 child를 가리킴. 즉, y는 원래 full node의 복사본인 셈이다. (numOfKeys로 조절할 것)
	z->leaf = y->leaf; // z와 y는 항상 같은 level에 있으므로 leaf node 여부도 동일하다. (BTree는 depth를 동일하게 유지하므로)
	z->numOfKeys = t - 1; // split할 때 right child의 key개수는 항상 t-1이 됨.(2t-1개에서 1개는 root로 나머지는 left,right가 각각 가지므로)
	y->numOfKeys = t - 1;  // y는 full node의 복사본이라 key가 3개가 들어있지만 x,y에 할당해주고 numOfKeys를 degree에 맞게 조정해주면서 관리한다. 

	// x,z 에 해당하는 key값 넣기& key개수 update. 
	x->numOfKeys = x->numOfKeys + 1; // x의 numOfKeys +1 
	for (int j = 0; j < t - 1; j++)
		z->key[j] = y->key[j + t];

	if (y->leaf == 0) { // y가 leaf node가 아니면 
		for (int j = 0; j < t; j++)  // z에 child 달아준다. 즉, x->key[i]와 z->key[i] 사이의 key 값을 가진 y에서 가져온다. 
			z->child[j] = y->child[j + t];
	}
	
	// x의 child와 key update. 
	for (int j = x->numOfKeys-1; j > i; j--) { // x->child가 있으면 가장 큰 것이 z로 빠져나가므로 자리 땡겨준다.
		x->child[j + 1] = x->child[j];
	}
	x->child[i + 1] = z; // x의 key보다 큰 값을 가지는 z는 child[1]으로 온다. (child[0]가 x의 key값보다 작은 y가 온다.)
	
	for (int j = x->numOfKeys - 2; j > i-1; j--) { // x의 key 자리 땡겨준다. 
		x->key[j + 1] = x->key[j];
	}
	x->key[i] = y->key[t - 1]; // x(새로운 root)는 middle key를 가진다.
}

// insert함수인데 node의 key개수가 최대가 아닐 때 insert 하는 함수. 
void insert_not_full(BTreeNode* cur, int k, int t) {
 // k가 들어갈 곳이 leaf node가 될때까지 반복. 
		int num = cur->numOfKeys; // 현재 노드의 key 개수. key의 index로 사용할 것이다. 
		if (cur->leaf) { // leaf node이면 자리 찾고 key 삽입
			num--;
			while (num >= 0 && k < cur->key[num]) { // k(insert key)가 현 node의 key[]보다 작으면 자리 땡겨준다.
				cur->key[num + 1] = cur->key[num];
				num--;
			}
			cur->key[num + 1] = k;               // 자리 찾았으니 실제 값 삽입
			cur->numOfKeys = cur->numOfKeys + 1; // insert했으니 개수 update.
		}

		// 현재 노드가 leaf node가 아닐 때. 즉, child로 내려가야함. 
		else {
			while (num >= 1 && k < cur->key[num - 1])  // 현 node에서 key들어가야하는 위치 찾는다. (어느 child로 내려가야할지 정하는 것임)
				num--;
			if ((cur->child[num])->numOfKeys == 2 * t - 1) { // 현 node의 child의 key가 2*t-1 즉, 최대이면 split 필요함.
				split(cur, num, t);
				if (k > cur->key[num]) {
					num++;
				}
			}
			insert_not_full(cur->child[num], k, t); // 현 node의 child가 최대 key 가지고 있지 않으면 current pointer child로 옮겨줌.
		}
	
}
// insert 함수 (parameter로 tree, k: insert하려는 값, t:degree를 받는다. ) 
void insert(BTree* tree, int k, int t) {
	BTreeNode* cur = malloc(sizeof(BTreeNode));
	initialize_Node(cur, t);
	cur = tree->root;  // tree의 root부터 내려간다. 

	if (cur->numOfKeys == 2 * t - 1) { // root node가 full일 때 split 필요. 
		BTreeNode* s = malloc(sizeof(BTreeNode));
		initialize_Node(s, t);
		if (s == NULL)
			return;
		tree->root = s;
		s->leaf = 0; // s는 이제 새로운 root이니 leaf node가 아니다!! 
		s->child[0] = cur; // s는 새로운 root node가 될 것이고 cur은 s의 child[0]이 되고 cur도 update 할 것이다. 

		split(s, 0, t);
		insert_not_full(s, k, t); // split하고나서 다시 insert 해야한다. 이때 split 했으므로 insert_not_full로 호출
	}
	// split할 필요없이 key가 들어갈 위치 찾아서 insert하면 된다. 
	else {
		insert_not_full(cur, k, t);
	}
}

int main() {
	double start, end;
	float result;
	start = (double)clock();
	BTree* tree = malloc(sizeof(BTree));
	createBTree(tree, 9); // tree 생성. = root node 생성
	int rand_arr[10000];
	int count = 0;
	int flag = 0;

	// 중복제거를 위한 Logic. 
	srand(time(NULL));
	while (count != 1000) {
		flag = 0;
		int random_value = rand() % 9999;
		for (int i = 0; i < count; i++) {
			if (rand_arr[i] == random_value) {
				flag = 1;
				break;
			}
		}

		if(flag == 0) {
			insert(tree, random_value, 9);
			rand_arr[count] = random_value;
			count++;
		}
	}
	end = (double)clock();
	printf("*** B-Tree 결과 *** \n");
	display(tree);

	printf("걸린시간: %lfms", end-start);
	return 0;
}