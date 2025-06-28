#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <vector>
#include <bitset>

using namespace std;

struct Node {
    char ch;
    int freq;
    Node *left, *right;
    Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};

void buildCodeMap(Node* root, unordered_map<char, string>& codeMap, string currentCode) {
    if (!root) return;
    if (!root->left && !root->right) {
        codeMap[root->ch] = currentCode;
    }
    buildCodeMap(root->left, codeMap, currentCode + "0");
    buildCodeMap(root->right, codeMap, currentCode + "1");
}

void saveTree(Node* root, ofstream& out) {
    if (!root) return;
    if (!root->left && !root->right) {
        out << '1' << root->ch; // Leaf node
    } else {
        out << '0'; // Internal node
        saveTree(root->left, out);
        saveTree(root->right, out);
    }
}

void compressFile(const string& inputFile, const string& outputFile) {
    ifstream in(inputFile, ios::binary);
    ofstream out(outputFile, ios::binary);
    ofstream treeOut(outputFile + ".tree", ios::binary);

    unordered_map<char, int> freq;
    string content((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    for (char c : content) freq[c]++;

    // Build Huffman tree
    priority_queue<Node*, vector<Node*>, Compare> pq;
    for (auto& p : freq) pq.push(new Node(p.first, p.second));
    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        Node* merged = new Node('\0', left->freq + right->freq);
        merged->left = left; merged->right = right;
        pq.push(merged);
    }
    Node* root = pq.top();

    // Save tree for decompression
    saveTree(root, treeOut);
    treeOut.close();

    // Build code map
    unordered_map<char, string> codeMap;
    buildCodeMap(root, codeMap, "");

    // Encode content
    string encodedStr = "";
    for (char c : content) encodedStr += codeMap[c];

    // Pad with zeroes to make multiple of 8
    int extraBits = 8 - (encodedStr.size() % 8);
    if (extraBits != 8) {
        encodedStr.append(extraBits, '0');
    }
    out.put(extraBits); // store how many bits are padding

    // Write binary
    for (size_t i = 0; i < encodedStr.size(); i += 8) {
        bitset<8> byte(encodedStr.substr(i, 8));
        out.put(static_cast<unsigned char>(byte.to_ulong()));
    }

    cout << "Compression complete.\n";
}

int main() {
    string inputFile = "/Users/utkarsh/Downloads/deb195b6486f11ec.txt.clean.txt";
    string outputFile = "output_huff.txt";
    compressFile(inputFile, outputFile);
    return 0;
}