#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <vector>
#include <bitset>

using namespace std;

struct Node {
    char ch;
    Node *left, *right;
    Node(char c = '\0') : ch(c), left(nullptr), right(nullptr) {}
};

// Rebuild tree from serialized file
Node* loadTree(ifstream& in) {
    char flag;
    in.get(flag);
    if (flag == '1') {
        char ch;
        in.get(ch);
        return new Node(ch);
    }
    Node* node = new Node();
    node->left = loadTree(in);
    node->right = loadTree(in);
    return node;
}

// Decode bitstream using the Huffman tree
void decompressFile(const string& inputFile, const string& outputFile) {
    ifstream in(inputFile, ios::binary);
    ifstream treeIn(inputFile + ".tree", ios::binary);
    ofstream out(outputFile, ios::binary);

    // Step 1: Load the Huffman Tree
    Node* root = loadTree(treeIn);
    treeIn.close();

    // Step 2: Read encoded file and extract bits
    int extraBits = in.get();  // how many padding bits at the end

    string bitStream = "";
    char byte;
    while (in.get(byte)) {
        bitset<8> bits((unsigned char)byte);
        bitStream += bits.to_string();
    }
    if (extraBits != 8)
        bitStream = bitStream.substr(0, bitStream.size() - extraBits);  // remove padding

    // Step 3: Decode using the tree
    Node* current = root;
    for (char bit : bitStream) {
        if (bit == '0')
            current = current->left;
        else
            current = current->right;

        if (!current->left && !current->right) {
            out.put(current->ch);
            current = root;
        }
    }

    cout << "Decompression complete.\n";
}

int main() {
    string inputFile = "output_huff.txt";
    string outputFile = "output_tree.txt";
    decompressFile(inputFile, outputFile);
    return 0;
}