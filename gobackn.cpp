#include <iostream>
#include <cmath>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
using namespace std;
class slideframe
{
    int n, m, w, j;

public:
    slideframe()
    {
        j = 0;
    }
    void input()
    {
        cout << "Enter number of frames:";
        cin >> n;
        cout << "Enter window size:";
        cin >> m;
    }
    int windowsize()
    {
        cout << "Total window size:" << pow(2, m) - 1 << endl;
        return pow(2, m) - 1;
    }
    void send()
    {
        cout << "\n##SENDER'S SIDE" << endl;
        for (int i = 0; i < w; i++)
        {
            if (j < n)
                cout << "Sending frames: " << ++j << endl;
            else
            {
                ++j;
                cout << "No frame" << endl;
            }
        }
    }
    void sender()
    {
        int c;
        w = windowsize();
        int i = 0;
        for (i; i < n; i++)
        {
            if (j == 0)
            {
            }
            else
                cout << "\n\t\tSLIDING ONTO NEXT FRAME" << endl;
            send();
        start:
            getch();
            srand(time(NULL));
            c = (rand() % 3) + 1;
            if (c == 1)
            {
                cout << "Frame:" << i + 1 << " sent from sender's-side\n";
                Receive(i);
                cout << "Acknowledgement Received at Sender-Side \n";
                j -= w - 1;
            }
            else if (c == 2)
            {

                cout << "\t\tFRAME:" << i + 1 << " IS LOST\n";
                j -= w;
                cout << "\t\t\t\t\tSending the window again " << endl;
                send();
                goto start;
            }
            else
            {
                cout << "Frame:" << i + 1 << " sent from sender's-side\n";
                Receive(i);
                cout << "\t\t acknowledgement For Frame:" << i + 1 << " is Not Received at Senders Side\n";
                j -= w;
                cout << "\t\t\t\t\tSending the window again " << endl;
                send();
                goto start;
            }
            cout << "##Frame sent\n";
        }
    }
    void Receive(int x)
    {
        getch();
        cout << "\t\t ##RECEIVER'S SIDE\n ";
        cout << "\t\t Frame:" << x + 1 << " received at receiver side\n";
        cout << endl;
    }
    void display()
    {
        cout << "\n\n\t*NO MORE FRAMES*\n";
        cout << "\t\t\t\t\t\t\t\tAll data sent successfully!!!" << endl;
    }
};
int main()
{
    slideframe p;
    p.input();
    p.sender();
    p.display();
    return 0;
}