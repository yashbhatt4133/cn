//networkProtocol
#include <cstdio>
#include <string>
#define MAX_PKT 4

using namespace std;

typedef enum
{
  dat,
  ack,
  nak
} frameKind;

typedef enum
{
  wait,
  frameArrival
} eventType;

typedef struct
{
  unsigned char data[MAX_PKT];
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
  int sentSeq;
  int receivedSeq;

  packet dataPacket;
  frame senderFrame, receiverFrame;

  Protocol()
  {
    sentSeq = receivedSeq = -1;
  }

  int waitForEvent(eventType e)
  {
    return e == frameArrival;
  }

  string showkind(frameKind k) //display the event type
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

  // SENDER: Network -> Data Link Interface
  void fromNetworkLayer(packet &i)
  {
    printf("\nEncapsulating Packet<data='%s'> ...", i.data);
    senderFrame.seq = ++sentSeq;
    senderFrame.kind = dat;
    senderFrame.info = &i;
  }

  // SENDER: Data Link -> Physical Interface
  void toPhysicalLayer(frame &f)
  {
    if (f.kind == dat)
      printf("\nSending DataFrame<kind=%s, sequence=%i> to Physical Layer ...",
             showkind(f.kind).c_str(), f.seq);
    else
      printf("\nSending ControlFrame<kind=%s, ack=%i> to Physical Layer ...",
             showkind(f.kind).c_str(), f.ack);
  }

  // RECEIVER: Physical -> Data Link Interface
  void fromPhysicalLayer(frame &f)
  {
    printf("\nReceived DataFrame<kind=%s, sequence=%i> from Physical Layer ...",
           showkind(f.kind).c_str(), f.seq);
    printf("\nValidating Sequence Number ... ");
    if (receivedSeq != f.seq)
      printf("\nDecapsulating Frame ...");
    else
    {
      printf("\nDuplicate Frame Encountered ...");
      printf("\nDiscarding Frame ...");
    }
  }

  // RECEIVER: Data Link -> Network Interface
  void toNetworkLayer(packet &p)
  {
    printf("\nSending Packet<data='%s'> to Network Layer ...", p.data);
    receivedSeq = senderFrame.seq;
    receiverFrame.seq = 0;
    receiverFrame.kind = ack;
    receiverFrame.ack = senderFrame.seq + 1;
  }
};
class stopAndWait : public Protocol
{
public:
  string buf;
  eventType event;

  bool flag, start;
  int coeff, count, len, lim;

  stopAndWait(string s, int t)
  {
    buf = s;
    coeff = t;
    lim = 1e6;
    flag = false;
    start = false;
    count = 0;
  }

  void sender();
  void receiver();
};

void stopAndWait::sender()
{
  if (!start)
  {
    lim = buf.length() % MAX_PKT == 0
              ? buf.length() / MAX_PKT
              : buf.length() / MAX_PKT + 1;
    printf("\nDividing Data into Groups of %d-bytes Each ...", MAX_PKT);
    start = !start;
  }

  printf("\n\nSENDER\n======");
  if (count > 0)
  {
    if (count % coeff == 0)
    {
      printf("\nERROR: SIMULATED TIMEOUT ...");
      flag = true;
    }
    else
    {
      printf("\nReceived ControlFrame<kind=%s, ack=%d> ...",
             showkind(receiverFrame.kind).c_str(),
             receiverFrame.ack);
      if (flag)
      {
        count--;
        flag = !flag;
      }
    }
    if (receiverFrame.kind == nak || flag)
    {
      printf("\nResending Previous Frame ...");
      count--;
      flag = true;
    }
  }

  if (count == lim)
  {
    printf("\n\nData '%s' Sent Successfully ...", buf.c_str());
    exit(0);
  }

  while (count < lim)
  {
    while (event != wait)
    {
      if (!flag)
      {
        printf("\nEncapsulating Data D%d into a Packet ...", count + 1);
        for (int i = 0; i < MAX_PKT; i++)
          dataPacket.data[i] = buf[i + count * MAX_PKT];
        printf("\nPassing Packet to Data Link Layer ...");
        event = frameArrival;
      }
      if (waitForEvent(event))
      {
        if (!flag)
          fromNetworkLayer(dataPacket);
        toPhysicalLayer(senderFrame);
        event = wait;
      }
      receiver();
    }
  }
}

void stopAndWait::receiver()
{
  printf("\n\nRECEIVER\n========");
  if (event == wait)
  {
    fromPhysicalLayer(senderFrame);
    if (!flag)
    {
      toNetworkLayer(dataPacket);
      count++;
    }
    else
      count += 2;
    toPhysicalLayer(receiverFrame);
    event = frameArrival;
    sender();
  }
}

int main()
{
  char temp[50];
  printf("Enter Data: ");
  scanf("%s", temp);

  int temp2;
  printf("Enter Simulation Noise (>=2): ");
  scanf("%i", &temp2);

  stopAndWait *obj = new stopAndWait(string(temp), temp2);
  obj->sender();
  delete obj;

  return 0;
}