#include <stdio.h>
#include <string>
#include <functional>
#include <queue>
#include <vector>
#include <memory>
#include <iostream>

namespace game
{
    // Simple message class. it accepts string msg, this could be changed to Enum?
    class Message
    {
    public:
        Message(const std::string msg)
        {
            messageEvent = msg;
            timestamp = SDL_GetTicks();
        }

        std::string getMessage()
        {
            return messageEvent;
        }
        Uint32 getTime()
        {
            return timestamp;
        }

    private:
        std::string messageEvent;
        Uint32 timestamp; 
    };

    // bus to store all the messages
    class MessageBus
    {
    public:
        MessageBus(){};
        ~MessageBus(){};

        //This should be called when Bus is initialized.
        //Add all the components that should get messages
        //std::function can contain almost any object that acts like a function pointer in how you call it.
        //This way we can accept receivers that don't inherit Observer class
        //Also, the function name we pass can be different as long as the function has the same signature.
        void addReceiver(std::function<void(Message)> messageReceiver)
        {
            receivers.push_back(messageReceiver);
        }

        //Add a new message that the bus will send to all receivers.
        //All messages are added to queue, so it is FIFO.
        void sendMessage(Message msg)
        {
            messages.push(msg);
        }

        //notify will send all the messages to the receivers.
        //first message in the queue will be send first and then popped out
        void notify()
        {
            while (!messages.empty())
            {
                for (auto iter = receivers.begin(); iter != receivers.end(); iter++)
                {
                    (*iter)(messages.front());
                }
                messages.pop();
            }
        }

    private:
        std::vector<std::function<void(Message)>> receivers;
        std::queue<Message> messages;
    };

    //base class that all components that use message bus inherit
    class BusNode
    {
    public:
        //When constructed, it adds it's message receiver function to message bus
        BusNode(std::shared_ptr<MessageBus> msgBus)
        {
            this->messageBus = msgBus;
            this->messageBus->addReceiver(this->getNotifyFunc());
        }

        virtual void update(){};

    protected:
        std::shared_ptr<MessageBus> messageBus = nullptr;

        std::function<void(Message)> getNotifyFunc()
        {
            auto messageListener = [=](Message msg) -> void {
                this->onNotify(msg);
            };
            return messageListener;
        }

        void send(Message msg)
        {
            messageBus->sendMessage(msg);
        }

        virtual void onNotify(Message msg)
        {
            std::cout << "onNotify";
        }
    };

} // namespace game