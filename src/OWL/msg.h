#pragma once

#include <SDL2/SDL.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <functional>
#include <queue>
#include <vector>
#include <memory>

namespace OWL
{
    /**
     * @brief Simple message class for commmunication between systems.
     *
     * @param msg message string
     */
    class Message
    {
    public:
        Message(const std::vector<std::string> params)
            : timestamp{SDL_GetTicks()}, messageParameters{params} {}

        uint32_t getTime()
        {
            return timestamp;
        }
        std::vector<std::string> getParameters()
        {
            return messageParameters;
        }
        std::string getParameter(int i)
        {
            return messageParameters[i];
        }

    private:
        uint32_t timestamp;
        std::vector<std::string> messageParameters;
    };

    /**
     * @brief A bus to store all messeges send by systems.
     * @details All components that can receive messages are added to the receivers vector.
     *          All messages MessageBus receives are stored in messages queue in FIFO order.
     */
    class MessageBus
    {
    public:
        MessageBus() {}

        /**
        * @brief Add a component to MessageBus's receivers.
        * 
        * @details Add all the components that should get messages
        * std::function can contain almost any object that acts like a function pointer in how you call it.
        * This way we can accept receivers that don't inherit any kind Observer class
        * Also, the function name we pass can be different as long as the function has the same signature. 
        * 
        * @param messageReceiver by default this is BusNode's getNotifyFunc(). 
        */
        void addReceiver(std::function<void(Message)> messageReceiver)
        {
            receivers.push_back(messageReceiver);
        }

        /**
        * @brief Add a new message that the bus will send to all receivers.
        * @details All messages are added to queue, so it is FIFO.
        * @param msg the message that is to be send
        */
        void sendMessage(Message msg)
        {
            messages.push(msg);
            std::cout << msg.getParameter(0) << std::endl;
        }

        /// @brief Notify will send all the messages in the queue to the receivers. FIFO.
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

    /**
    * @brief Base class that all components that use MessageBus inherit from.
    * @details When constructed, it adds the message receiver function (getNotifyfunc) to the MessageBus.
    * @param msgBus A reference to the MessageBus object. 
    */
    class BusNode
    {
    public:
        BusNode(std::shared_ptr<MessageBus> msgBus, std::string name = "unnamed BusNode") : messageBus{msgBus}
        {
            messageBus->addReceiver(this->getNotifyFunc());
            send({name + " created."});
        }

        virtual void update(){};

    protected:
        std::shared_ptr<MessageBus> messageBus = nullptr;

        /// add this object to the MessageBus receivers.
        std::function<void(Message)> getNotifyFunc()
        {
            auto messageListener = [=](Message msg) -> void {
                this->onNotify(msg);
            };
            return messageListener;
        }

        void send(std::vector<std::string> params)
        {
            messageBus->sendMessage(params);
        }

        // This is called when MessageBus sends messages out.
        virtual void onNotify(Message msg) {}
    };

} // namespace OWL