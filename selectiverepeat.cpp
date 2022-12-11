#include <cstdio>
#include <bits/stdc++.h>
#define MAX_PKT 5

using namespace std;

typedef enum
{
  dat,
  ack,
  nak
} frameKind;

typedef enum
{
  timeout,
  checksumError,
  frameArrival,
  networkLayerReady
} eventType;

typedef struct
{
  unsigned char data;
} packet;

typedef struct
{
  packet *info;
  frameKind kind;
  unsigned int seq;
  unsigned int ack;
} frame;

class Protocol
{
public:
  eventType event;
  bool noNak, errorDetected;
  int MAX_SEQ, flag, err, buf;
  int frameExpected, frameToSend;

  packet dataPacket;
  frame senderFrame, receiverFrame;

  Protocol()
  {
    noNak = true;
    err = buf = -1;
    flag = frameToSend = frameExpected = 0;
    errorDetected = false;
  }

  int waitForEvent(eventType e)
  {
    return e == frameArrival;
  }

  string showkind(frameKind k)
  {
    switch (k)
    {
    case dat:
      return "data";
      break;
    case ack:
      return "ack";
      break;
    case nak:
      return "nak";
      break;
    }
    return "";
  }

  // Network -> Data Link Interface
  void fromNetworkLayer(packet &i)
  {
    printf("\nEncapsulating Packet<data='%c'> ...", i.data);
    senderFrame.seq = frameToSend;
    senderFrame.kind = dat;
    senderFrame.info = &i;
    frameToSend = (frameToSend + 1) % (MAX_SEQ + 1);
  }

  // Data Link -> Physical Interface
  void toPhysicalLayer(frame &f)
  {
    if (event == timeout)
    {
      cout << "\nTimeout period expired. Resending frame with sequence no. " << err;
      f.seq = err;
      err = -1;
      frameToSend = (err + 1) % (MAX_SEQ + 1);
      event = frameArrival;
    }
    else if (f.kind == dat)
      printf("\nSending DataFrame<kind=%s, sequence=%i> to Physical Layer ...",
             showkind(f.kind).c_str(), f.seq);
    else
    {
      if (err != -1)
      {
        if (!noNak)
        {
          f.kind = nak;
          f.ack = err;
          noNak = true;
        }
        else
        {
          f.kind = ack;
          f.ack = err - 1;
        }
      }
      else if (buf != -1)
      {
        f.ack = buf;
        frameExpected = (buf + 1) % (MAX_SEQ + 1);
        frameToSend = frameExpected;
        buf = -1;
      }
      printf("\nSending ControlFrame<kind=%s, ack=%i> to Physical Layer ...",
             showkind(f.kind).c_str(), f.ack);
    }
  }

  // Data Link -> Network Interface
  void toNetworkLayer(packet &p)
  {
    printf("\nSending Packet<data='%c'> to Network Layer ...", p.data);
    receiverFrame.seq = frameToSend - 1;
    receiverFrame.kind = ack;
    receiverFrame.ack = frameExpected;
    frameExpected = (frameExpected + 1) % (MAX_SEQ + 1);
  }

  // Physical -> Data Link Interface
  void fromPhysicalLayer(frame &f)
  {
    printf("\nReceived DataFrame<kind=%s, sequence=%i> from Physical Layer ...",
           showkind(f.kind).c_str(), f.seq);
    printf("\nValidating Sequence Number ... ");
    {
      if (frameExpected == f.seq)
      {
        if (f.seq == 1 && flag == 0) // Error Simulation
        {
          cout << "\nError in received frame ...";
          flag = 1;
          noNak = false;
          errorDetected = true;
          err = f.seq;
        }
        else
        {
          printf("\nDecapsulating Frame ...");
          noNak = true;
          toNetworkLayer(dataPacket);
        }
      }
      else
      {
        printf("\nFrame out of order. Storing in buffer ...");
        buf = f.seq;
      }
    }
  }
};
void getch()
{
  cin.ignore();
  cin.get();
  return;
}

void clrscr()
{
#ifdef _WIN32
  system("cls");
#elif __unix__
  system("clear");
#endif
  return;
}

class selectiveRepeatSlidingWindow : public Protocol
{
public:
  string in_buf;

  selectiveRepeatSlidingWindow(int n, string s)
  {
    MAX_SEQ = n;
    in_buf = s;
  }

  void sender();
  void receiver();
};

void selectiveRepeatSlidingWindow::sender()
{
  event = frameArrival;
  printf("\n\nSENDER\n======");
  if (frameToSend ==
          (err + (MAX_SEQ / 2)) %
              (MAX_SEQ + 1) &&
      errorDetected == true &&
      err >= 0)
  {
    event = timeout;
    frameToSend = err;
    errorDetected = 0;
  }
  else if (frameToSend == MAX_SEQ &&
           frameToSend != frameExpected &&
           errorDetected == true)
  {
    fromNetworkLayer(dataPacket);
    frameToSend = frameExpected;
    errorDetected = false;
  }
  else if (event == frameArrival)
  {
    printf("\nEncapsulating Data '%c' into a Packet ...", in_buf[frameToSend]);
    dataPacket.data = in_buf[frameToSend];
    printf("\nPassing Packet to Data Link Layer ...");
    fromNetworkLayer(dataPacket);
  }
  toPhysicalLayer(senderFrame);
  receiver();
}

void selectiveRepeatSlidingWindow::receiver()
{
  printf("\n\nRECEIVER\n========");
  fromPhysicalLayer(senderFrame);
  toPhysicalLayer(receiverFrame);
  getch();
  clrscr();
  sender();
}

int main()
{
  int n;
  cout << "\nEnter bits needed to identify window: ";
  cin >> n;

  char temp[50];
  printf("Enter Data: ");
  scanf("%s", temp);

  selectiveRepeatSlidingWindow *obj =
      new selectiveRepeatSlidingWindow(
          pow(2, n) - 1,
          string(temp));
  obj->sender();
  delete obj;

  return 0;
}