#include "msg.h"
#include <iostream>

namespace game
{

    class TestA : public BusNode
    {
    public:
        TestA(MessageBus* messageBus) : BusNode(messageBus) {}

    private:
        void onNotify(Message msg)
        {
            std::cout << "asd23" << std::endl;
            std::cout << "I received: " << msg.getMesssage() << std::endl;
        }
    };

    class Test : public game::BusNode
    {
    public:
        Test(MessageBus* messageBus) : BusNode(messageBus) {}

        void update()
        {
            game::Message greeting("23");
            send(greeting);
        }

    private:
        void onNotify(game::Message msg)
        {
            std::cout << "I received: " << msg.getMesssage() << std::endl;
        }
    };
} // namespace game

int main()
{
    game::MessageBus messageBus;
    game::TestA testA(&messageBus);
    game::Test test(&messageBus);
    test.update();
    testA.update();
    messageBus.notify();

    return 0;
}