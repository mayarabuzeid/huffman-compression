#include <iostream>
#include <fstream>
#include<bits/stdc++.h>
#include <bitset>
using namespace std;
class Node
{
public:
    char data;
    int freq;
    string number;
    Node * left, *right;
    Node(int freq,char data)
    {
        this->data = data;
        this->freq = freq;
        left = right =NULL;
        number="";
    }

};
class compare
{
public:
    bool operator()(const Node * n1, const Node * n2) const
    {
        bool result=n1->freq > n2->freq;
        return result;
    }
};
time_t start, endt;
vector<char> letters;
unordered_map <char, int> mp;
unordered_map <char,string> order;
string concChar,Ascii,decomConc;
int decomCounter,decomlenString,counter=0,g=0;
std::priority_queue<Node*, std::vector<Node*>, compare> pq;
Node * reconsTree = new Node(NULL,NULL);
Node * huffmanTree();
Node * node;
void reconstruct(char,string);
void compress(string);
void decompress(string);
char searchCode(char);
void concatente();
void append(Node*);
void print();

Node* huffmanTree()
{
    while(pq.size()>1)
    {
        Node*first = pq.top();
        pq.pop();
        Node*second = pq.top();
        pq.pop();
        int third=first->freq+second->freq;
        Node * root = new Node(third,NULL);
        root->left=first;
        root->right=second;
        pq.push(root);
    }
    Node* result=pq.top();
    pq.pop();
    return result;
}

void print(string filename)
{
    cout<<"Byte\tCode\t\tNew Code"<<endl;
    for(std::unordered_map<char,string>::iterator itr = order.begin(); itr!=order.end(); ++itr)
    {
        cout<<(int)(itr->first)<<"\t"<<bitset<8>((int)(itr->first)).to_string()<<"\t"<<itr->second<<endl;
    }
    ifstream is;
    is.open (filename, ios::binary );
    is.seekg (0, ios::end);
    float  lengthIN = is.tellg();
    is.close();
    ifstream os;
    os.open ("compressed.txt", ios::binary );
    os.seekg (0, ios::end);
    float  lengthOut = os.tellg();
    os.close();
    float CDratio=(lengthOut/lengthIN)*100;
    cout<<"compression to decompression ratio:"<<CDratio<<endl;
}

void append(Node* node)
{
    if (node == NULL)
        return;
    string value=node->number;
    if(node->left!=NULL)
    {
        node->left->number=value+"0";
        if(node->left->data!=NULL)
        {
            order.insert(pair<char,string>(node->left->data,node->left->number));
        }
    }
    append(node->left);
    if(node->right!=NULL)
    {
        node->right->number=value+"1";
        if(node->right->data!=NULL)
        {
            order.insert(pair<char,string>(node->right->data,node->right->number));
        }
    }
    append(node->right);

}
void concatente()
{
    for(char x:letters)
    {
        concChar+=order[x];
    }
    while(concChar.size()%8!=0)
    {
        concChar+="0";
        counter++;

    }
    for(int i=0; i<concChar.size(); i+=8)
    {
        string m=concChar.substr(i,8);
        bitset<8> bits(m);
        int newLetter = (int)(bits.to_ulong());
        Ascii+=char(newLetter);
    }
}
char searchCode(char c)
{
    if(c=='0'&&node->left!=NULL)
    {
        node=node->left;
    }
    else if(c=='1'&&node->right!=NULL)
    {
        node=node->right;
    }
    return node->data;
}

void decompress(string filename)
{
    std::ifstream ss(filename,ios::binary);
    std::ofstream os("decompressed.txt");
    if (!ss.is_open())
    {
        cout<<"File not found"<<endl;
    }
    else
    {
        string code,name,srch;
        char ch,charc,resultChar;
        ss>>decomCounter>>decomlenString;
        for(int i=0; i<decomlenString; i++)
        {
            ss.get(ch);
            int x=(int)(ch);
            decomConc+=bitset<8>(x).to_string();
        }
        decomConc=decomConc.substr(0,decomConc.size()- decomCounter); // decompressed bits before padding
        ss.get(ch);
        while(ss.good())
        {
            ss.get(charc);
            ss.get(ch);
            getline(ss,code);
            reconstruct(charc,code);
        }
        node = reconsTree;
        while(g!=decomConc.size())
        {
            resultChar=searchCode(decomConc[g]);
            if(resultChar!=NULL)
            {
                os<<resultChar;
                node = reconsTree;
            }
            g++;
        }
        os.close();
    }
}
void compress(string filename)
{
    std::ifstream is(filename);  // open file
    std::ofstream os("compressed.txt",ios::binary);
    if (!is.is_open())
    {
        cout<<"File not found"<<endl;
    }
    else
    {
        char ch;
        int freq=1;
        while (is.get(ch))
        {
            letters.push_back(ch);
            auto it = mp.find(ch);
            if(it!=mp.end())
            {
                it->second++;
            }
            else
            {
                mp.insert(pair<char, int>(ch, freq));
            }
        }
        is.close();

        if(mp.empty())
        {
            cout<<"Empty File"<<endl;
            exit(0);
        }
        for(std::unordered_map<char,int>::iterator itr = mp.begin(); itr!=mp.end(); ++itr)
        {
            Node * root1 = new Node(itr->second,itr->first);
            pq.push(root1);
        }
        if(pq.size()==1)
        {

            pair<char,int> top=*mp.begin();
            order.insert(pair<char,string>(top.first,"1"));
            for(char x:letters)
            {
                concChar+=order[x];
            }
            while(concChar.size()%8!=0)
            {
                concChar+="0";
                counter++;

            }
            for(int i=0; i<concChar.size(); i+=8)
            {
                string m=concChar.substr(i,8);
                bitset<8> bits(m);
                int newLetter = (int)(bits.to_ulong());
                Ascii+=char(newLetter);
            }
        }
        else
        {
            Node * root=huffmanTree();
            append(root);
            concatente();
        }
        os<<counter<<" ";
        os<<Ascii.size();
        os<<Ascii<<endl;
        int c=0;
        for(std::unordered_map<char,string>::iterator itr = order.begin(); itr!=order.end(); ++itr)
        {
            os<<itr->first<<endl;
            if(c==order.size()-1)
            {
                os<<itr->second;

            }
            else
            {
                os<<itr->second<<endl;
            }
            c++;
        }
        os.close();
        time(&endt);
        print(filename);
    }

}

void reconstruct(char charc,string code)
{
    Node* newNode = reconsTree;
    int i;
    for(i=0; i<code.size()-1; i++)
    {
        if(code[i]=='0'&& newNode->left==NULL)
        {
            newNode->left = new Node(NULL,NULL);
            newNode->left->number=code[i];
            newNode=newNode->left;
        }
        else if(code[i]=='0'&& newNode->left!=NULL)
        {
            newNode=newNode->left;
        }
        else if(code[i]=='1'&& newNode->right==NULL)
        {
            newNode->right = new Node(NULL,NULL);
            newNode->right->number=code[i];
            newNode=newNode->right;
        }
        else if(code[i]=='1'&& newNode->right!=NULL)
        {
            newNode=newNode->right;
        }
    }
    if(code[i]=='0')
    {
        newNode->left = new Node(NULL,charc);
        newNode->left->number=code[i];
    }
    else if(code[i]=='1')
    {
        newNode->right = new Node(NULL,charc);
        newNode->right->number=code[i];
    }
}
int main()
{

    int x;
    string filename;
    cout<<"Please enter file name:"<<endl;
    cin>>filename;
    cout<<"Please choose:"<<endl<<"1. Compress"<<endl<<"2. Decompress"<<endl;
    cin>>x;
    switch(x)
    {
    case 1:
        time(&start);
        compress(filename);
        break;
    case 2:
        time(&start);
        decompress(filename);
        time(&endt);
        break;
    default: "Invalid entry"
        ;
    }
    double time_taken = double(endt - start);
    cout << "Time taken by program is : " << fixed
         << time_taken << setprecision(5);
    cout << " sec " << endl;
    return 0;
}
