#include <iostream>
#include <functional>
#include <queue>
#include <vector>

class Message
{
public:
    Message(const std::string event)
    {
        messageEvent = event;
    }

    std::string getEvent()
    {
        return messageEvent;
    }
private:
    std::string messageEvent;
};

class MessageBus
{
public:
    MessageBus() {};
    ~MessageBus() {};

    void addReceiver(std::function<void (Message)> messageReceiver)
    {
        receivers.push_back(messageReceiver);
    }

    void sendMessage(Message message)
    {
        messages.push(message);
    }

    void notify()
    {
        while(!messages.empty()) {
            for (auto iter = receivers.begin(); iter != receivers.end(); iter++) {
                (*iter)(messages.front());
            }

            messages.pop();
        }
    }

private:
    std::vector<std::function<void (Message)>> receivers;
    std::queue<Message> messages;
};

class BusNode
{
public:
    BusNode(MessageBus *messageBus)
    {
        this->messageBus = messageBus;
        this->messageBus->addReceiver(this->getNotifyFunc());
    }

    virtual void update() {}
protected:
    MessageBus *messageBus;

    std::function<void (Message)> getNotifyFunc()
    {
        auto messageListener = [=](Message message) -> void {
            this->onNotify(message);
        };
        return messageListener;
    }

    void send(Message message)
    {
        messageBus->sendMessage(message);
    }

    virtual void onNotify(Message message)
    {
        // Do something here. Your choice. But you could do this.
        // std::cout << "Siopao! Siopao! Siopao! (Someone forgot to implement onNotify().)" << std::endl;
    }
};

// This class will receive a message from ComponentB.
class ComponentA : public BusNode
{
public:
    ComponentA(MessageBus* messageBus) : BusNode(messageBus) {}

private:
    void onNotify(Message message)
    {
        std::cout << "I received: " << message.getEvent() << std::endl;
    }
};

// This class will send a message to ComponentA.
class ComponentB : public BusNode
{
public:
    ComponentB(MessageBus* messageBus) : BusNode(messageBus) {}

    void update()
    {
        Message greeting("Hi!");
        send(greeting);
    }

private:
    void onNotify(Message message)
    {
        std::cout << "I received: " << message.getEvent() << std::endl;
    }
};


int main()
{
    MessageBus messageBus;
    ComponentA compA(&messageBus);
    ComponentB compB(&messageBus);

    // This is supposed to act like a game loop.
    for (int ctr = 0; ctr < 50; ctr++) {
        compA.update();
        compB.update();
        messageBus.notify();
    }

    return 0;
}