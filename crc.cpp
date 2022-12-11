#include <iostream>
#include <string>
using namespace std;
void ltrim(string &sub)
{
    while (sub[0] != '1' && sub.length())
        sub = sub.substr(1);
}
string xorr(string &s1, string &s2)
{
    if (s1.length() != s2.length())
        throw "Equal Length Strings Needed for xor";
    string res;
    for (int i = 0; i < s1.length(); i++)
    {
        (s1[i] == s2[i]) ? res.append("0") : res.append("1");
    }
    return res;
}
string remainder(string dividend, string key)
{
    int pos = 0;
    int keylen = key.length();
    int divlen = dividend.length();
    string chunk, temp;
    while (pos < divlen)
    {
        if (chunk.length() < keylen)
        {
            chunk += dividend[pos];
            pos++;
        }
        if (chunk.length() == keylen)
        {
            if (chunk[0] == '1')
            {
                temp = xorr(chunk, key);
                chunk = temp;
            }
            ltrim(chunk);
        }
    }
    while (chunk.length() < keylen-1){
        chunk  = "0"+chunk;
    }
    return chunk;
}
int main()
{
    string msg, key, rmsg;
    cout << "Enter Binary Message > ";
    cin >> msg;
    cout << "Enter Key > ";
    cin >> key;
    string dividend = msg;
    for (int i = 1; i < key.length(); i++)
        dividend.append("0");
    string crc= remainder(dividend, key);
    string frame = msg + crc;
    cout<<"CRC = "<<crc<<"\n";
    cout << "The Frame Sent       = " << frame << "\n";
    cout<<"NOISE SIMULATION\n";
    cout << "Enter Recieved Frame > ";
    cin >> rmsg;
    string rem2 = remainder(rmsg, key);
    cout<<"Recieved Message Remainder = "<<rem2<<"\n";
    ltrim(rem2);
    if (rem2 == "" && frame.length()==rmsg.length())
        cout << "NO ERROR";
    else
        cout << "ERROR FOUND";
    return 0;
}