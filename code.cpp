#include <bits/stdc++.h>
using namespace std;


// Treap Node
struct Node {
    int val;        // actual value at this position
    int minVal;     // minimum in subtree (after lazy)
    int lazy;       // pending increment for subtree
    bool rev;       // pending reverse flag
    int size;
    int priority;
    Node *left, *right;

    Node(int v) : val(v), minVal(v), lazy(0), rev(false),
                  size(1), priority(rng()), left(nullptr), right(nullptr) {}
};

//helpers
int sz(Node* t)     { return t ? t->size   : 0; }
int mn(Node* t)     { return t ? t->minVal : INT_MAX; }

void update(Node* t) {
    if (!t) return;
    t->size   = 1 + sz(t->left) + sz(t->right);
    t->minVal = min({t->val, mn(t->left), mn(t->right)});
}

//Push lazy increment down
void pushIncrement(Node* t, int delta) {
    if (!t) return;
    t->val    += delta;
    t->minVal += delta;
    t->lazy   += delta;
}

//Push reverse flag down
void pushReverse(Node* t) {
    if (!t) return;
    swap(t->left, t->right);
    t->rev = !t->rev;
}

//Propagate all pending lazy to children
void push(Node* t) {
    if (!t) return;
    if (t->lazy) {
        pushIncrement(t->left,  t->lazy);
        pushIncrement(t->right, t->lazy);
        t->lazy = 0;
    }
    if (t->rev) {
        pushReverse(t->left);
        pushReverse(t->right);
        t->rev = false;
    }
}


//Split: left part has exactly k nodes
pair<Node*, Node*> split(Node* t, int k) {
    if (!t) return {nullptr, nullptr};
    push(t);
    int leftSize = sz(t->left);
    if (leftSize >= k) {
        auto [L, R] = split(t->left, k);
        t->left = R;
        update(t);
        return {L, t};
    } else {
        auto [L, R] = split(t->right, k - leftSize - 1);
        t->right = L;
        update(t);
        return {t, R};
    }
}


//Merge
Node* merge(Node* l, Node* r) {
    if (!l) return r;
    if (!r) return l;
    push(l); push(r);
    if (l->priority > r->priority) {
        l->right = merge(l->right, r);
        update(l);
        return l;
    } else {
        r->left = merge(l, r->left);
        update(r);
        return r;
    }
}


//Operations
//Report min(i, j)  [0-indexed, inclusive]
int reportMin(Node*& root, int i, int j) {
    auto [L,  R1] = split(root, i);
    auto [M,  R ] = split(R1, j - i + 1);
    int result = mn(M);
    root = merge(L, merge(M, R));
    return result;
}

//Multi-Increment(i, j, delta)  [0-indexed, inclusive]
void multiIncrement(Node*& root, int i, int j, int delta) {
    auto [L,  R1] = split(root, i);
    auto [M,  R ] = split(R1, j - i + 1);
    pushIncrement(M, delta);
    root = merge(L, merge(M, R));
}

//Rotate(i, j)  [0-indexed, inclusive]
void rotate(Node*& root, int i, int j) {
    auto [L,  R1] = split(root, i);
    auto [M,  R ] = split(R1, j - i + 1);
    pushReverse(M);
    root = merge(L, merge(M, R));
}


//Build treap from array
Node* build(const vector<int>& a) {
    Node* root = nullptr;
    for (int x : a)
        root = merge(root, new Node(x));
    return root;
}

//Print sequence
void inorder(Node* t, vector<int>& out) {
    if (!t) return;
    push(t);
    inorder(t->left, out);
    out.push_back(t->val);
    inorder(t->right, out);
}

void printSeq(Node* root) {
    vector<int> v;
    inorder(root, v);
    cout << "[ ";
    for (int x : v) cout << x << " ";
    cout << "]\n";
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<int> init = {14, 12, 23, 19, 111, 51, 321, -40};
    Node* root = build(init);

    cout << "Initial:          "; printSeq(root);

    rotate(root, 1, 6);
    cout << "After Rotate(1,6): "; printSeq(root);


    vector<int> a = {5, 3, 8, 1, 9, 2, 7, 4, 6};
    root = build(a);
    cout << "Initial:                    "; printSeq(root);

    //Report min(2, 6)
    int m = reportMin(root, 2, 6);
    cout << "ReportMin(2,6) = " << m << "\n";  // min of {8,1,9,2,7} = 1
    cout << "Sequence after:             "; printSeq(root);

    //Multi-Increment(1, 4, 10)
    multiIncrement(root, 1, 4, 10);
    cout << "After MultiIncrement(1,4,10): "; printSeq(root);
    //indices 1..4 each get +10: {3,8,1,9} -> {13,18,11,19}

    //Report min(0, 8) after increment
    m = reportMin(root, 0, 8);
    cout << "ReportMin(0,8) = " << m << "\n";

    //Rotate(0, 4)
    rotate(root, 0, 4);
    cout << "After Rotate(0,4):          "; printSeq(root);

    //Rotate(3, 7)
    rotate(root, 3, 7);
    cout << "After Rotate(3,7):          "; printSeq(root);

    //Multi-Increment on entire array
    multiIncrement(root, 0, 8, -5);
    cout << "After MultiIncrement(0,8,-5): "; printSeq(root);

    // Final min query
    m = reportMin(root, 0, 8);
    cout << "ReportMin(0,8) = " << m << "\n";

    cout << "\n Interactive Mode \n";
    cout << "Enter n: ";
    int n; cin >> n;
    vector<int> b(n);
    cout << "Enter " << n << " integers: ";
    for (int& x : b) cin >> x;
    root = build(b);
    cout << "Sequence: "; printSeq(root);

    cout << "Enter number of queries: ";
    int q; cin >> q;
    while (q--) {
        cout << "Query type (1=ReportMin, 2=MultiIncrement, 3=Rotate): ";
        int type; cin >> type;
        if (type == 1) {
            int i, j; cin >> i >> j;
            cout << "Min(" << i << "," << j << ") = " << reportMin(root, i, j) << "\n";
        } else if (type == 2) {
            int i, j, delta; cin >> i >> j >> delta;
            multiIncrement(root, i, j, delta);
            cout << "After increment: "; printSeq(root);
        } else if (type == 3) {
            int i, j; cin >> i >> j;
            rotate(root, i, j);
            cout << "After rotate: "; printSeq(root);
        } else {
            cout << "Unknown query type.\n";
        }
    }

    return 0;
}
