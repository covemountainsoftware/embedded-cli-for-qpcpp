#include <cstdio>
#include <cstdlib>
#include <array>
#include <thread>
#include <unistd.h>
#include "qpcpp.hpp"
#include "embeddedCliEvent.hpp"
#include "embeddedCliService.hpp"

struct SmallEventElement
{
    union {
        std::array<uint8_t, 16> data;
    };
};

struct MediumEventElement
{
    union {
        std::array<uint8_t, 64> data;
    };
};
static_assert(sizeof (cms::EmbeddedCLI::Event) == sizeof(SmallEventElement), "blah");

static std::array<SmallEventElement, 32> smallPoolStorage;
static std::array<MediumEventElement, 16> mediumPoolStorage;
static QP::QSubscrList subscriberStorage[MAX_PUB_SUB_SIG];
static std::array<QP::QEvt const *, 10> cliQueueSto;

class LinuxCharacterDevice : public cms::interfaces::CharacterDevice
{
public:
    LinuxCharacterDevice() :
        mCallback(nullptr),
        mCallbackUserData(nullptr),
        mReader(&LinuxCharacterDevice::Reader, this)
    {
    }

    bool WriteAsync(uint8_t byte) override
    {
        printf("%c", (char)byte);
        fflush(stdout);
        return true;
    }

    void RegisterNewByteCallback(NewByteCallback callback, void* userData) override
    {
        mCallback = callback;
        mCallbackUserData = userData;
    }

private:
    void Reader()
    {
        char buf;
        ssize_t result = -1;
        do
        {
            result = read(STDIN_FILENO, &buf, 1);
            if (mCallback != nullptr)
            {
                mCallback(mCallbackUserData, buf);
            }
        }  while (result > 0);
    }

    NewByteCallback mCallback = nullptr;
    void* mCallbackUserData = nullptr;
    std::thread mReader = {};
};

static void InitFramework()
{
    QP::QF::init();
    QP::QF::poolInit(smallPoolStorage.data(), sizeof(smallPoolStorage), sizeof(SmallEventElement));
    QP::QF::poolInit(mediumPoolStorage.data(), sizeof(mediumPoolStorage), sizeof(MediumEventElement));
    QP::QActive::psInit(subscriberStorage, Q_DIM(subscriberStorage));
}

extern "C" {

Q_NORETURN Q_onError(char const* const module, int_t const id)
{
    fprintf(stderr, "Q_onError in %s:%d", module, id);
    QP::QF::onCleanup();
    exit(-1);
}

} // extern "C"

namespace QP {

void QF::onStartup()
{
}

void QF::onCleanup()
{
}

void QF::onClockTick() {
    QTimeEvt::TICK_X(0U, &l_clock_tick);   // process time events at rate 0
}

} //namespace QP


int main()
{
    using namespace QP;
    printf("Greetings, this is an example of the embedded-cli-for-qpcpp running in Linux\n");

    InitFramework();

    auto cli = new cms::EmbeddedCLI::Service(nullptr, 0, 0, "CLI> ");
    cli->start(1, cliQueueSto.data(), cliQueueSto.size(), nullptr, 0);
    cli->BeginCliAsync(new LinuxCharacterDevice());
    return QP::QF::run();
}
