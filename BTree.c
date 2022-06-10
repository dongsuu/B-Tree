#include<stdio.h>
#include <stdlib.h>
#include<time.h>

typedef struct BTreeNode {
	int* key; // node�� ������ key�迭.
	//child�� 2*t��ŭ ���� �� �ִ�. 
	struct BTreeNode* child[18];  // node�� ������ child pointer t=2�϶�. (t�� �ٲٷ��� �� ���� �ٲ������)
	int leaf; // leaf node���� �ƴ���. 1:true, 0: false
	int numOfKeys; // node�� ������ �ִ� key ����. 
}BTreeNode;

typedef struct BTree {
	BTreeNode* root; // tree�� root 
}BTree;

// �Ҵ� ���� node �ʱ�ȭ 
void initialize_Node(BTreeNode* node, int t) {
	node->key = malloc(sizeof(int) * 2 * t - 1);  // key�� 2*(degree) - 1�� ��ŭ ���� �� �ִ�. 
	node->leaf = 1;
	node->numOfKeys = 0;
}

// Tree �����ϴ� �Լ� 
void createBTree(BTree* tree, int t) {
	BTreeNode* node = malloc(sizeof(BTreeNode));

	if (node == NULL) { // �Ҵ� ���н�.
		printf("�Ҵ� �����߽��ϴ�.");
		return;
	}
	// root node �ʱ�ȭ 
	initialize_Node(node, t);

	//tree�� root node�� �ȴ�. 
	tree->root = node;
}

// Display �Լ�. Tree�� ���� �����ش�. 
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


// split�Լ�
void split(BTreeNode* x, int i, int t) {  // i�� x(cur)�� ���° child���� split�� �̷������� �Ѱ���. 
	// root node �̸� ���ڷ� ���� x�� middle key�� ����ִ� ���̰�, x�� root�� �ȴ�. root node�� �ƴϸ� x���� split�Ϸ��� node�� parent node�� �������. 
	BTreeNode* z = malloc(sizeof(BTreeNode));  // z�� middle key���� ū �� ������ node (x�� ������ child�� ��)
	initialize_Node(z,t);
	BTreeNode* y = x->child[i];               // y�� x�� child�� ����Ŵ. ��, y�� ���� full node�� ���纻�� ���̴�. (numOfKeys�� ������ ��)
	z->leaf = y->leaf; // z�� y�� �׻� ���� level�� �����Ƿ� leaf node ���ε� �����ϴ�. (BTree�� depth�� �����ϰ� �����ϹǷ�)
	z->numOfKeys = t - 1; // split�� �� right child�� key������ �׻� t-1�� ��.(2t-1������ 1���� root�� �������� left,right�� ���� �����Ƿ�)
	y->numOfKeys = t - 1;  // y�� full node�� ���纻�̶� key�� 3���� ��������� x,y�� �Ҵ����ְ� numOfKeys�� degree�� �°� �������ָ鼭 �����Ѵ�. 

	// x,z �� �ش��ϴ� key�� �ֱ�& key���� update. 
	x->numOfKeys = x->numOfKeys + 1; // x�� numOfKeys +1 
	for (int j = 0; j < t - 1; j++)
		z->key[j] = y->key[j + t];

	if (y->leaf == 0) { // y�� leaf node�� �ƴϸ� 
		for (int j = 0; j < t; j++)  // z�� child �޾��ش�. ��, x->key[i]�� z->key[i] ������ key ���� ���� y���� �����´�. 
			z->child[j] = y->child[j + t];
	}
	
	// x�� child�� key update. 
	for (int j = x->numOfKeys-1; j > i; j--) { // x->child�� ������ ���� ū ���� z�� ���������Ƿ� �ڸ� �����ش�.
		x->child[j + 1] = x->child[j];
	}
	x->child[i + 1] = z; // x�� key���� ū ���� ������ z�� child[1]���� �´�. (child[0]�� x�� key������ ���� y�� �´�.)
	
	for (int j = x->numOfKeys - 2; j > i-1; j--) { // x�� key �ڸ� �����ش�. 
		x->key[j + 1] = x->key[j];
	}
	x->key[i] = y->key[t - 1]; // x(���ο� root)�� middle key�� ������.
}

// insert�Լ��ε� node�� key������ �ִ밡 �ƴ� �� insert �ϴ� �Լ�. 
void insert_not_full(BTreeNode* cur, int k, int t) {
 // k�� �� ���� leaf node�� �ɶ����� �ݺ�. 
		int num = cur->numOfKeys; // ���� ����� key ����. key�� index�� ����� ���̴�. 
		if (cur->leaf) { // leaf node�̸� �ڸ� ã�� key ����
			num--;
			while (num >= 0 && k < cur->key[num]) { // k(insert key)�� �� node�� key[]���� ������ �ڸ� �����ش�.
				cur->key[num + 1] = cur->key[num];
				num--;
			}
			cur->key[num + 1] = k;               // �ڸ� ã������ ���� �� ����
			cur->numOfKeys = cur->numOfKeys + 1; // insert������ ���� update.
		}

		// ���� ��尡 leaf node�� �ƴ� ��. ��, child�� ����������. 
		else {
			while (num >= 1 && k < cur->key[num - 1])  // �� node���� key�����ϴ� ��ġ ã�´�. (��� child�� ������������ ���ϴ� ����)
				num--;
			if ((cur->child[num])->numOfKeys == 2 * t - 1) { // �� node�� child�� key�� 2*t-1 ��, �ִ��̸� split �ʿ���.
				split(cur, num, t);
				if (k > cur->key[num]) {
					num++;
				}
			}
			insert_not_full(cur->child[num], k, t); // �� node�� child�� �ִ� key ������ ���� ������ current pointer child�� �Ű���.
		}
	
}
// insert �Լ� (parameter�� tree, k: insert�Ϸ��� ��, t:degree�� �޴´�. ) 
void insert(BTree* tree, int k, int t) {
	BTreeNode* cur = malloc(sizeof(BTreeNode));
	initialize_Node(cur, t);
	cur = tree->root;  // tree�� root���� ��������. 

	if (cur->numOfKeys == 2 * t - 1) { // root node�� full�� �� split �ʿ�. 
		BTreeNode* s = malloc(sizeof(BTreeNode));
		initialize_Node(s, t);
		if (s == NULL)
			return;
		tree->root = s;
		s->leaf = 0; // s�� ���� ���ο� root�̴� leaf node�� �ƴϴ�!! 
		s->child[0] = cur; // s�� ���ο� root node�� �� ���̰� cur�� s�� child[0]�� �ǰ� cur�� update �� ���̴�. 

		split(s, 0, t);
		insert_not_full(s, k, t); // split�ϰ��� �ٽ� insert �ؾ��Ѵ�. �̶� split �����Ƿ� insert_not_full�� ȣ��
	}
	// split�� �ʿ���� key�� �� ��ġ ã�Ƽ� insert�ϸ� �ȴ�. 
	else {
		insert_not_full(cur, k, t);
	}
}

int main() {
	double start, end;
	float result;
	start = (double)clock();
	BTree* tree = malloc(sizeof(BTree));
	createBTree(tree, 9); // tree ����. = root node ����
	int rand_arr[10000];
	int count = 0;
	int flag = 0;

	// �ߺ����Ÿ� ���� Logic. 
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
	printf("*** B-Tree ��� *** \n");
	display(tree);

	printf("�ɸ��ð�: %lfms", end-start);
	return 0;
}